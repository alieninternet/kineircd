/* connection.cpp
 * Connection class
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

#include <cerrno>
#include <cstring>

#include "kineircd/connection.h"
#include "kineircd/register.h"
#include "kineircd/debug.h"
#include "kineircd/daemon.h"
#include "kineircd/utils.h"


/* Connection - Initialise new connection
 * Original 12/08/01 simonb
 */
Connection::Connection(Socket *sock, bool sec)
: socket(sock),
  sentBytes(0),
  sentMessages(0),
  receivedBytes(0),
  receivedMessages(0),
  status(CONFLAG_CONNECTED),
  handler(new registerHandler(this)),
  secure(sec),
  connectedTime(Daemon::getTime()),
  lastSpoke(Daemon::getTime()),
  name(0)
{
#ifdef DEBUG_EXTENDED
   debug(String::printf("[%d] Loading new connection",
			socket->getFD()));
#endif
   
   // Clear the queue, anything that is in it is just garble at this point
   killQueue();

   // Add ourselves to the input descriptor set
   Daemon::addInputFD(socket->getFD());
   
   // Increase connection counters
   Daemon::numConns++;
   Daemon::numUnknown++;
   if (Daemon::numConns > Daemon::numConnsPeak) {
      Daemon::numConnsPeak = Daemon::numConns;
   }
}


/* ~Connection - Shut down connection
 * Original 12/08/01 simonb
 */
Connection::~Connection(void)
{
#ifdef DEBUG_EXTENDED
   debug(String::printf("[%d] Deleting connection",
			socket->getFD()));
#endif

   /* Kill the output handler, this should be first just in case it depends
    * on anything we will delete after its destructor is called
    */
   delete handler;
   
   // Kill the output queue, just as a safety measure
   killQueue();
   
   // Shut down the socket (will also close it if necessary)
   delete socket;
   
   // Reduce the connection count
   Daemon::numConns--;
}


// this needs to be inlined - connection.h?!
void Connection::touch(void)
{
   lastSpoke = Daemon::getTime();
};


/* handleInput - Handle data input on connection
 * Original 12/08/01 simonb
 */
void Connection::handleInput(void)
{
   // Catch a dead connection here
   if (socket->isConnected()) {
      // Read a line from the socket buffer (this should change!!!)
      String line = socket->io->read();

      // Make sure we have a whole line - we only want to deal with full lines
      if (line.length()) {
	 // Increment our input counters
	 receivedMessages++;
	 receivedBytes += line.length();
	 Daemon::receivedBytes += line.length();
      
	 // Handle the data (finally!)
	 handler->parseLine(line);
      }
      
      return;
   }
   
   /* Dead connection, the socket has been closed;
    * Uncheck the connected flag and remove us from any file descriptor sets
    * we may be on..
    */
#ifdef DEBUG_EXTENDED
   debug(String::printf("[%d] handleInput(): Dead connection",
			socket->getFD()));
#endif
   Daemon::delOutputFD(socket->getFD());
}


/* sendRaw - [Various forms] Write a raw line to the connection output queue
 * Original 12/08/01 simonb
 */
void Connection::sendRaw(String const &line)
{
   if (status & CONFLAG_CONNECTED) {
      if (outQueue.empty()) {
	 Daemon::addOutputFD(socket->getFD());
      }

      outQueue.push(line);
   }
}


/* killQueue - Wipe the queue clear
 * Original 17/08/01 simonb
 */
void Connection::killQueue(void)
{
   while (!outQueue.empty()) {
      outQueue.pop();
   }
}


/* sendQueue - Send a line from the queue
 * Original 12/08/01 simonb
 * Note: This is shit. We should send as much as we can to generate bigger
 *       tcp packets, this is just an utter disgrace but will do for now
 *       (time of writing, not even the user parser has been completed)
 */
void Connection::sendQueue(void)
{
   // Make sure we are connected (checking the socket not the status flag)
   if (!socket->isConnected()) {
#ifdef DEBUG_EXTENDED
      debug(String::printf("[%d] sendQueue() Called on a disconnected "
			   "connection!",
			socket->getFD()));
#endif
      Daemon::delOutputFD(socket->getFD());
      return;
   }
   
#ifdef DEBUG_EXTENDED
   // Sanity check, should never happen normally
   if (outQueue.empty()) {
      debug(String::printf("[%d] sendQueue() Called while nothing to send - "
			   "very wrong!",
			socket->getFD()));
      return;
   }
#endif

   String outdata;
   
   // Loop until the queue is either empty or we have enough to send
   for (;;) {
      outdata = outdata + outQueue.front();
      outQueue.pop();

      if (outQueue.empty() || 
	  ((outdata.length() + outQueue.front().length()) >
	   MAX_OUTQUEUE_DUMP_SIZE)) {
	 break;
      }
   }
   
   /* Write the data, if this fails kill the output queue and mark this
    * connection as no longer connected for the garbage collector to clean up
    */
   if (!socket->io->write(outdata)) {
      killQueue();
      goodbye(String::printf("Write error: %s",
			     strerror(errno)));
   }

   // Increment our output counters
   sentBytes += outdata.length();
   Daemon::sentBytes += outdata.length();
   
   // Do we need to turn off output checking for select()?
   if (outQueue.empty()) {
      Daemon::delOutputFD(socket->getFD());
   }
}


/* goodbye - [Various forms] Say goodbye to a socket and mark it 'disconnected'
 * Original 12/08/01 simonb
 */
void Connection::goodbye(String const &reason)
{
   // Check that we are marked connected
   if (status & CONFLAG_CONNECTED) {
#ifdef DEBUG_EXTENDED
      debug(String::printf("[%d] goodbye()", socket->getFD()));
#endif

      // Inform the handler it is going bye bye
      handler->goodbye(reason);
      
      // Mark this connection as dead for the garbage collector
      status &= ~CONFLAG_CONNECTED;
      Daemon::delInputFD(socket->getFD());
      
      // Mask this as a dead connection with the name (for stats l)
      name = 0;
   }
#ifdef DEBUG_EXTENDED
   else {
      debug(String::printf("[%d] goodbye(): Called while already marked "
			   "disconnected -- wrong!",
			   socket->getFD()));
   }
#endif
}


/* kill - Kill the connection - nasty :)
 * Original 22/09/01 simonb
 */
void Connection::kill(void)
{
   // Check that we are marked connected
   if (status & CONFLAG_CONNECTED) {
#ifdef DEBUG_EXTENDED
      debug(String::printf("[%d] kill()",
			   socket->getFD()));
#endif

      // Mark this connection as dead for the garbage collector
      status &= ~CONFLAG_CONNECTED;
      Daemon::delInputFD(socket->getFD());

      // Mask this as a dead connection with the name (for stats l)
      name = 0;
   }
#ifdef DEBUG_EXTENDED
   else {
      debug(String::printf("[%d] kill(): Called while already marked "
			   "disconnected -- wrong!",
			   socket->getFD()));
   }
#endif
}

