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

#ifndef _INCLUDE_KINEIRCD_CONNECTION_H_
# define _INCLUDE_KINEIRCD_CONNECTION_H_ 1

# include "kineircd/kineircdconf.h"

# include <queue>

namespace Kine {
   class Connection;
};

# include "kineircd/user.h"
# include "kineircd/handler.h"
# include "kineircd/socket.h"

// Connection status flags
//# define CONFLAG_CONNECTED	0x0001	// Connected
//# define CONFLAG_PING_SENT	0x0002	// Sent a ping, waiting for a pong

namespace Kine {
   
   class Connection {
    private:
      Socket *socket;			// The connected socket

      std::queue <String> outQueue;	// Output queue
      
      unsigned long long sentBytes;	// Number of bytes sent
      unsigned long sentMessages;	// Number of messages sent (lines)
      
      unsigned long long receivedBytes;	// Number of bytes received
      unsigned long receivedMessages;	// Number of messages sent (lines)
      
      short status;			// Connection status flags
      
      Handler *handler;			// Data handler
      
      bool secure;			// Is this connection secured?
      
      time_t const connectedTime;	// Time connection was established
      time_t lastSpoke;			// Time the connection last spoke
      
      String *name;			// The name of this connection
      
    public:
      Connection(Socket *, bool);	// Class constructor
      ~Connection(void);		// Shutdown connection
      
      // Is this connection secure?
      bool isSecure(void) const {
	 return secure;
      };
      
      // Return the connection time
      time_t getConnectedTime(void) const {
	 return connectedTime;
      };
      
      // Return the last time this connection 'spoke'
      time_t getLastSpoke(void) const {
	 return lastSpoke;
      };
      
      // Update the 'lastSpoke' variable (this should be inlined0)
      void touch(void);
      
      void handleInput(void);		// Handle input on connection
      
      void sendRaw(String const &);	// Send a line to the connection raw
      
      void killQueue(void);		// Kill the queue
      void sendQueue(void);		// Send an output queue line
      
      void goodbye(String const & = "");// Close down socket
      void kill(void);			// Kill the connection (sharp)
      
      friend class Daemon;
      friend class statsFunctions;
      friend class registerHandler;
   };
   
};
   
#endif // _INCLUDE_KINEIRCD_CONNECTION_H_ 

