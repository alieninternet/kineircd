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
   // Set up the current time variable for the first time
   setTime();
   
   // Fire-up the modules we have loaded!
   config.getModuleList().startAll(*this);
   
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


/* run - The main loop
 * Original 11/08/01 simonb
 * Note: Unfortuantely not a very nice looking routine..
 */
#define CheckInput(x)	FD_ISSET(x, &inFDtemp)
#define CheckOutput(x)	FD_ISSET(x, &outFDtemp)
Exit::status_type Kine::Daemon::run(void)
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
#ifdef KINE_DEBUG
	 debug("Doing nothing...");
#endif
	 break;
	 
//       default:
//	 if (runlevel >= RUNLEVEL_NORMAL) {
	    // Check for a new connection
//	    for (listen_list_t::iterator it = listens.begin();
//		 it != listens.end(); it++) {
//	       // Check if there is a new connection we should be aware of
//	       if (CheckInput((*it)->socket->getFD())) {
//		  newConnection(*it);
//	       }
//	    }
	    
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
//	    }

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

