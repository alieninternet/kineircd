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

# include <aisutil/socket/socket.h>

namespace Kine {
   class Protocol;
};

# include "kineircd/daemon.h"

namespace Kine {
   class Connection {
    private:
      Daemon& daemon;			// Call-back to the daemon
      AISutil::Socket& socket;		// The connected socket
      Protocol* protocol;		// The protocol we are running
      
      unsigned long long sentBytes;	// Number of bytes sent
      unsigned long long receivedBytes;	// Number of bytes received
      
      const time_t connectedTime;	// Time connection was established
      time_t lastSpoke;			// Time the connection last spoke

      bool connected;			// Are we connected?
      
      // Fix the lastSpoke variable to 'now'
      void touch(void)
	{ lastSpoke = daemon.getTime().tv_sec; };
      
    public:
      // Class constructor
      Connection(Daemon& d, AISutil::Socket& s);
      
      // Shutdown connection
      ~Connection(void);

      // Return the daemon
      Daemon& getDaemon(void)
	{ return daemon; };
      
      // Return the socket
      const AISutil::Socket& getSocket(void) const
	{ return socket; };

      // Replace the protocol with something new
      void setProtocol(Protocol& p)
	{ protocol = &p; };
      
      // Return the connection time
      const time_t getConnectedTime(void) const 
	{ return connectedTime; };
      
      // Return the last time this connection 'spoke'
      time_t getLastSpoke(void) const 
	{ return lastSpoke; };

      // Handle input on connection
      bool handleInput(void);

      // Send a chunk from the output queue
      void sendOutput(void);
      
      // Close the connection
      void goodbye(void);
   };
};

// Complete forwarded declarations
# include "kineircd/protocol.h"

#endif // _INCLUDE_KINEIRCD_CONNECTION_H_ 

