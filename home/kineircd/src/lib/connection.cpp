/* $Id$
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

#include "kineircd/connection.h"
#include "debug.h"

using namespace Kine;


/* Connection - Initialise new connection
 * Original 12/08/2001 simonb
 */
Connection::Connection(Daemon& d, Socket& s)
: daemon(d),
  socket(s),
  sentBytes(0),
  sentMessages(0),
  receivedBytes(0),
  receivedMessages(0),
  connectedTime(d.getTime().tv_sec),
  lastSpoke(connectedTime)
{
#ifdef KINE_DEBUG_PSYCHO
   debug("Connection::Connection() Loading new connection on fd " +
	 String::convert(socket.getFD()) + " @ " + 
	 String::convert(connectedTime));
#endif
}


/* handleInput - Handle data input on connection
 * Original 12/08/2001 simonb
 */
bool Connection::handleInput(void)
{
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
      receivedMessages++;
      receivedBytes += line.str().length();
      daemon.addReceivedBytes(line.str().length());
      debug("Read: " + line.str());
//      // Handle the data (finally!)
//      handler->parseLine(line);
   }
   
   // All is well!
   return true;
}
