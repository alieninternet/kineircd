/* $Id$
 * Connection class
 * 
 * Copyright (c) 2001,2002 Simon Butcher <pickle@alien.net.au>
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

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif
#include "kineircd/kineircdconf.h"

#include "kineircd/connection.h"
#include "lib/debug.h"

using namespace Kine;
using AISutil::String;


/* Connection - Initialise new connection
 * Original 12/08/2001 simonb
 */
Connection::Connection(AISutil::Socket& s)
: socket(s),
  protocol(0),
  sentBytes(0),
  receivedBytes(0),
  connectedTime(daemon().getTime().tv_sec),
  lastSpoke(connectedTime),
  connected(socket.isOkay())
{
#ifdef KINE_DEBUG_PSYCHO
   std::ostringstream out;
   out << "Connection::Connection() Loading new connection on fd " <<
    socket.getFD() << " @ " << this << " (" << connectedTime << ')';
   debug(out.str());
#endif
}


/* ~Connection - Shutdown connection
 * Original 12/08/2001 simonb
 */
Connection::~Connection(void)
{ 
   delete &socket;
   delete protocol;
}


/* handleInput - Handle data input on connection
 * Original 12/08/2001 simonb
 */
bool Connection::handleInput(void)
{
#ifdef KINE_DEBUG_ASSERT
   assert(protocol != 0);
#endif

   // Make sure we are connected..
   if (!connected) {
      return false;
   }
   
   // Read a line from the socket buffer (this should change)
   std::stringstream line;
   if (!socket.read(line)) {
#ifdef KINE_DEBUG_PSYCHO
      debug("Connection::handleInput() - Read failed on fd " + 
	    String::convert(socket.getFD()));
#endif
      return false;
   }
   
   // Make sure we have a whole line - we only want to deal with full lines
   if (!line.str().empty()) {
      // Increment our input counters
      receivedBytes += line.str().length();
      daemon().addReceivedBytes(line.str().length());

      // Throw the data at the protocol routines
      protocol->handleInput(line);
   }
   
   // If there is something wanting to be outputted, tell the poller
   if (protocol->moreOutput()) {
      // this is temporary, until the poller code comes into play
      FD_SET(socket.getFD(), &daemon().outFDSET);
   }
   
   // All is well!
   return true;
}


/* sendOutput - Send a chunk from the output queue
 * Original 29/09/2002 simonb
 */
void Connection::sendOutput(void)
{
   // Dump a chunk
   socket.write(protocol->withdrawOutput(socket.getReadBlockSize()));
   
   // If there is nothing else left in the queue, stop output-okay checks
   if (!protocol->moreOutput()) {
      // this is temporary, until the poller code comes into play
      FD_CLR(socket.getFD(), &daemon().outFDSET);
   }
}


/* goodbye - Close the connection
 * Original 12/08/2001 simonb
 */
void Connection::goodbye(void)
{
#ifdef KINE_DEBUG_EXTENDED
   debug("Connection::goodbye() - Shutting down fd " +
	 String::convert(socket.getFD()));
#endif
   
   // This is temporary..
   connected = false;
//   socket.close();
}
