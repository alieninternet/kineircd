/* $Id$
 * The main class (IRC daemon) constructors and destructors
 * 
 * Copyright (c) 2001,2002 KineIRCd Development Team
 * (See DEV-TEAM file for details)
 *
 * This file is a part of KineIRCd.
 * 
 * KineIRCd is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * KineIRCd is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with KineIRCd; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "kineircd/kineircdconf.h"

#include <cstdio>
#include <cerrno>
#include <cstring>
#include <ctime>

extern "C" {
#include <unistd.h>
   
#ifdef KINE_HAVE_OPENSSL
# include <openssl/ssl.h>
#endif
}

#include "kineircd/daemon.h"
#include "kineircd/version.h"
#include "debug.h"

using namespace Kine;


/* Daemon - Init the server
 * Original 11/08/01 simonb
 */
Daemon::Daemon(Config& conf, Signals& sigs)
  : runlevel(RUNLEVEL_INIT),
    config(conf),
    signals(sigs),
    startTime(time(NULL))
{
#ifdef KINE_DEBUG_ASSERT
   // Check the STL stuff are really empty
   assert(connections.empty());
#endif
   
   // Set up the current time variable for the first time
   setTime();
   
   // Fire-up the modules we have loaded!
   config.getModuleList().startAll(*this);

   // Start listening on all listeners.
   config.getListenerList().startAll();

   // We are ready to go, go into normal running runlevel
   runlevel = RUNLEVEL_NORMAL;
}


/* ~Daemon - Deinit the server
 * Original 11/08/01 simonb
 */
Daemon::~Daemon(void)
{
#ifdef KINE_DEBUG
   debug("Shutting down server");
#endif

}


/* newConnection - Process a new connection
 * Original 03/09/2002 simonb
 */
void Daemon::newConnection(Listener& listener)
{
#ifdef KINE_DEBUG_PSYCHO
   debug("Daemon::newConnection() - New connection on file descriptor " +
	 String::convert(listener.getFD()));
#endif
   
   // Accept the connection
   Socket* const newSocket = listener.accept();
   
   // Check if that worked..
   if (newSocket == 0) {
#ifdef KINE_DEBUG_EXTENDED
      debug("A null socket was returned from the listener!");
#endif
      return;
   }
   
#ifdef KINE_DEBUG_PSYCHO
   debug("New socket FD = " + String::convert(newSocket->getFD()) +
	 "; Remote address is " + newSocket->getRemoteAddress() +
	 " on port " + String::convert(newSocket->getRemotePort()));
#endif
   
   // and.. throw it away :(
   delete newSocket;
}


/* run - The main loop
 * Original 11/08/2001 simonb
 * Note: Unfortuantely not a very nice looking routine..
 */
Exit::status_type Daemon::run(void)
{
   /* Make sure the init was all happy, else there isn't much point us
    * going beyond this point really
    */
   if (runlevel != RUNLEVEL_NORMAL) {
      return Exit::ERR_DAEMON_INIT;
   }
   
#ifdef KINE_DEBUG_EXTENDED
   debug("Daemon::run() - Initialising main loop");
#endif

   // Stuff we need for select()
   fd_set inFDtemp, outFDtemp;
   
   // Set up stuff for select()
   FD_ZERO(&inFDSET);
   FD_ZERO(&outFDSET);
   maxDescriptors = 0;

#ifdef KINE_DEBUG
   debug("Daemon::run() - Entering main loop");
#endif

   // Add the listeners to the file descriptor lists
   for (ListenerList::listeners_type::const_iterator it = 
	config.getListenerList().getList().begin(); 
	it != config.getListenerList().getList().end(); it++) {
      FD_SET((*it)->getFD(), &inFDSET);
      if ((*it)->getFD() >= maxDescriptors) {
	 maxDescriptors = (*it)->getFD() + 1;
      }
   }
   FD_SET(0, &inFDSET);

   // The main loop!
   for (;;) {
      // Set the time
      setTime();
      
      // Copy the FD sets over to save time
      inFDtemp = inFDSET;
      outFDtemp = outFDSET;

      // Poll select()
      switch (select(maxDescriptors, &inFDtemp, &outFDtemp,
		     SELECT_TYPE_ARG234 NULL, SELECT_TYPE_ARG5 NULL)) {
       case -1: // Select aborted (caused by a signal, usually)
#ifdef KINE_DEBUG_EXTENDED
	 perror("Daemon::run() - select()");
#endif
	 break;
	 
       case 0: // Select timed out, no descriptor change
#ifdef KINE_DEBUG_PSYCHO
	 debug("Doing nothing...");
#endif
	 break;
	 
       default:
#ifdef KINE_DEBUG_PSYCHO
	 debug("Daemon::run() - Select returned with a change!");
#endif
	 
	 if (runlevel >= RUNLEVEL_NORMAL) {
	    // Check for a new connection: Run through the listeners
	    for (ListenerList::listeners_type::const_iterator it = 
		 config.getListenerList().getList().begin(); 
		 it != config.getListenerList().getList().end(); it++) {
	       // Check if there is a new connection we should be aware of
	       if (FD_ISSET((*it)->getFD(), &inFDtemp)) {
		  newConnection(*(*it));
	       }
	    }
	    
	    // Check for activity on connections
//	    for (connection_list_t::iterator it = connections.begin();
//		 it != connections.end(); it++) {
#ifdef DEBUG_EXTENDED
//	       if (!(*it) || !(*it)->socket) {
//		  debug("broken?!");
//		  continue;
//	       }
#endif
	       
	       // Check for input waiting
//	       if (CheckInput((*it)->socket->getFD())) {
//		  (*it)->handleInput();
//	       }
	       
	       // Check for OK to send and something in the queue
//	       if (CheckOutput((*it)->socket->getFD())) {
//		  (*it)->sendQueue();
//	       }
	 }

      }
      
      // Check if we are in shutdown mode
      if (runlevel == RUNLEVEL_SHUTDOWN) {
	 // check queues here..
	 
	 // If we get here the queues must be all dry
#ifdef KINE_DEBUG
	 debug("Output queues exhausted - Breaking main loop");
#endif
	 break;
      }
   }
   
   return Exit::NO_ERROR;
}

