/* $Id$
 * The main class (IRC daemon) constructors and destructors
 * 
 * Copyright (c) 2001,2002,2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2001,2002,2003 KineIRCd Development Team
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

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif
#include "kineircd/kineircdconf.h"

#include <cstdio>
#include <cerrno>
#include <cstring>
#include <ctime>
#include <cstdlib>
#include <algorithm>

extern "C" {
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#ifdef KINE_HAVE_OPENSSL
# include <openssl/ssl.h>
#endif
}

#include "kineircd/daemon.h"
#include "kineircd/config.h"
#include "kineircd/version.h"
#include "libkineircd/debug.h"

using namespace Kine;
using AISutil::String;


// Our instance...
Daemon* Daemon::instance = 0;


/* Daemon - Init the server
 * Original 11/08/2001 simonb
 */
Daemon::Daemon(void)
  : runlevel(RUNLEVEL_INIT),
    startTime(time(NULL)),
    sentBytes(0),
    receivedBytes(0)
{
#ifdef KINE_DEBUG_ASSERT
   // Check the STL stuff are really empty
   assert(connections.empty());
#endif
   
   // Set up the current time variable for the first time
   setTime();

   // Seed the random thingy for rand() - this is kinda dodgey
   srand(getTime().tv_sec);
   
   // We are ready to go, go into normal running runlevel
   runlevel = RUNLEVEL_NORMAL;
}


/* ~Daemon - Deinit the server
 * Original 11/08/2001 simonb
 */
Daemon::~Daemon(void)
{
#ifdef KINE_DEBUG
   debug("Daemon::~Daemon() - Shutting down server");
#endif

   // Run through any connections that might be lingering
   while (!connections.empty()) {
      delete connections.front();
      connections.erase(connections.begin());
   }
}


/* initInstance - Create the single instance of this class
 * Original 28/10/2002 simonb
 * Note: This is separated from the getInstance() since doing the if for every
 *       reference call is damned wasteful, don't you think? :)
 */
void Daemon::initInstance(void)
{
#ifdef KINE_DEBUG_ASSERT
   // We should only ever be called to init the first instance..
   assert(instance == 0);
#else
   // Well, since we are not debugging I suppose we should quietly ignore this
   if (instance != 0) {
# ifdef KINE_DEBUG
      debug("Daemon::initInstance() - Called when an instance already exists");
# endif
      return;
   }
#endif
   
   // Create the new instance, then!
   instance = new Daemon();
   
#ifdef KINE_DEBUG_PSYCHO
   std::ostringstream debugOut;
   debugOut << "Daemon::initInstance() - Created new instance @ " << instance;
   debug(debugOut.str());
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
   AISutil::Socket* const newSocket = listener.accept();
   
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

   // We should be adding the socket to the poller here..
   FD_SET(newSocket->getFD(), &inFDSET);
   if (newSocket->getFD() >= maxDescriptors) {
      maxDescriptors = newSocket->getFD() + 1;
   }
   
   // Find a registrar (This should be configurable per listener, SIMON??)
   ProtocolInfo* protocolInfo = findProtocol(ProtocolInfo::Type::PRIMARY,
					     "IRC/2");
   
   // Create this new connection
   Connection* newConnection = new Connection(*newSocket);

   // Create a new instance of the primary protocol for this connection
   newConnection->setProtocol(*protocolInfo->createProtocol(*newConnection,
							    listener));

   // Add the socket to the connections list
   connections.push_front(newConnection);
}


/* registerProtocol - Register a new protocol on the protocol list
 * Original 02/10/2002 simonb
 */
bool Daemon::registerProtocol(ProtocolInfo& info)
{
#ifdef KINE_DEBUG_PSYCHO
   std::ostringstream output;
   output << "Daemon::registerProtocol() - Attemping to add protocol '" <<
     info.description.name << "' of type #" << info.description.type;
   debug(output.str());
#endif
   
   // Look for the protocol first - see if it is not already added
   if (protocols.find(info.description.key) != protocols.end()) {
#ifdef KINE_DEBUG_EXTENDED
      debug("Daemon::registerProtocol() - Unable to register '" +
	    String(info.description.name) + '\'');
#endif
      return false;
   }

   // Okay, add the protocol then
   (void)protocols.insert(protocols_type::value_type(info.description.key,
						     &info));
   
   return true;
}


/* deregisterProtocol - Remove a protocol from the protocol list
 * Original 02/10/2002 simonb
 * Note: This needs to scan the connections for any which are using this
 *       protocol too.. Kinda important :)
 */
void Daemon::deregisterProtocol(const ProtocolInfo& info)
{
#ifdef KINE_DEBUG_PSYCHO
   debug("Daemon::deregisterProtocol() - Attempting to remove protocol '" +
	 String(info.description.name) + '\'');
#endif

   // Something here, Simon? :)
}


/* findProtocol - Find a protocol matching the given criteria
 * Original 04/10/2002 simonb
 * Note: This needs optimisation :)
 */
ProtocolInfo* const Daemon::findProtocol(const ProtocolInfo::Type::type type,
					 const std::string& name) const
{
#ifdef KINE_DEBUG_PSYCHO
   debug("Daemon::findProtocol() - Looking for protocol '" +
	 name + "' of type type #" + String::convert(type));
#endif

   // Find the protocol, maybe (this is ugly IMHO)
   protocols_type::const_iterator proto =
     protocols.find(ProtocolInfo::Description(type, name.c_str()).key);

   // Check if we found the protocol, more often we will have
   if (proto != protocols.end()) {
      return proto->second;
   }
   
   // We must not have found it, return a null pointer
   return 0;
};


/* registerLogger - Add a logger to the logger list
 * Original 17/10/2002 simonb
 */
bool Daemon::registerLogger(Logger& logger)
{
#ifdef KINE_DEBUG_PSYCHO
   std::ostringstream output;
   output << "Daemon::registerLogger() - Attemping to add logger @ " << 
     &logger;
   debug(output.str());
#endif
   
   // Confirm the logger is not already registered
   if (loggers.find(&logger) != loggers.end()) {
#ifdef KINE_DEBUG_EXTENDED
      debug("Daemon::registerLogger() - Logger already registered");
#endif
      return false;
   }

   // Add the logger
   (void)loggers.insert(&logger);
   
   return true;
}


/* deregisterProtocol - Remove a logger from the logger list
 * Original 17/10/2002 simonb
 */
bool Daemon::deregisterLogger(Logger& logger)
{
   return (loggers.erase(&logger) > 0);
}


// Helper for the log() function below
struct logLine {
   const std::string& str;
   const Logger::Mask::type mask;
   
   // Constructor
   logLine(const std::string& s, const Logger::Mask::type m)
     : str(s),
       mask(m)
     {};
   
   // Pass the info to the given logger
   void operator()(Logger* logger) const
     { logger->log(str, mask); };
};


/* log - Send a log message to all the loggers in our loggers list
 * Original 09/04/2002 simonb
 */
void Daemon::log(const std::string& str, const Logger::Mask::type mask)
{
#ifdef KINE_DEBUG_EXTENDED
   debug("Log::sendLoggers(\"" + str + "\", " + String::convert(mask) + ");");
#endif
   
   // Only iterate through the list if it is not empty, else what is the point?
   if (!loggers.empty()) {
      (void)std::for_each(loggers.begin(), loggers.end(), logLine(str, mask));
   }
}


/* run - The main loop
 * Original 11/08/2001 simonb
 * Note: Unfortuantely not a very nice looking routine.. Much of this code is
 *       temporary and hense thrown together. Further down the road I will
 *       code a dedicated set of poller routines (including support for other
 *       methods such as poll(), /dev/epoll etc.). At this stage it's to get 
 *       the show on the road sooner rather than later)
 */
bool Daemon::run(void)
{
   /* Make sure the init was all happy, else there isn't much point us
    * going beyond this point really
    */
   if (runlevel != RUNLEVEL_NORMAL) {
      return false;
   }
   
   // Fire-up the modules we have loaded!
   config().getModuleList().startAll();

   // Start listening on all listeners..
   config().getListenerList().startAll();

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
	config().getListenerList().getList().begin(); 
	it != config().getListenerList().getList().end(); it++) {
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
	 if (runlevel >= RUNLEVEL_NORMAL) {
	    // Check for a new connection: Run through the listeners
	    for (ListenerList::listeners_type::const_iterator it = 
		 config().getListenerList().getList().begin(); 
		 it != config().getListenerList().getList().end(); it++) {
	       // Check if there is a new connection we should be aware of
	       if (FD_ISSET((*it)->getFD(), &inFDtemp)) {
		  newConnection(*(*it));
	       }
	    }
	    
	    // Check for activity on connections
	    for (connections_type::iterator it = connections.begin();
		 it != connections.end(); it++) {
#ifdef KINE_DEBUG_ASSERT
	       // Make sure the connection is sane. It should always be!
	       assert(*it != 0);
#endif

	       // Check for input waiting
	       if (FD_ISSET((*it)->getSocket().getFD(), &inFDtemp)) {
		  // Tell the connection it has stuff waiting to read
		  if (!(*it)->handleInput()) {
#ifdef KINE_DEBUG_PSYCHO
		     debug("Destroying connection on fd " + 
			   String::convert((*it)->getSocket().getFD()));
#endif
		     // The operation did not work, so we will delete this
		     FD_CLR((*it)->getSocket().getFD(), &inFDSET);
		     FD_CLR((*it)->getSocket().getFD(), &outFDSET);
		     delete *it;
		     connections.erase(it);
//		     continue;
		     break;
		  }
	       }
	       
	       // Check for OK to send and something in the queue
	       if (FD_ISSET((*it)->getSocket().getFD(), &outFDtemp)) {
		  (*it)->sendOutput();
	       }
	    }
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
   
   return true;
}
