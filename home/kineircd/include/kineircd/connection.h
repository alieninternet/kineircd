/* $Id$
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

#ifndef _INCLUDE_KINEIRCD_CONNECTION_H_
# define _INCLUDE_KINEIRCD_CONNECTION_H_ 1

# include <aisutil/socket/socket.h>
# include <aisutil/time.h>
# include <kineircd/protocol/base.h>

namespace Kine {
   //! Connection class
   class Connection {
    public:
      typedef unsigned long long byteCount_type;
      
    private:
      AIS::Util::Socket& socket;		//!< The connected socket
      Protocol::Base* protocol;			//!< The protocol we are running
      
      byteCount_type sentBytes;			//!< Number of bytes sent
      byteCount_type receivedBytes;		//!< Number of bytes received
      
      const AIS::Util::Time connectedTime;	//!< Time connection was established
      AIS::Util::Time lastSpoke;		//!< Time the connection last spoke

      bool connected;				//!< Are we connected?
      
    public:
      //! Class constructor
      explicit Connection(AIS::Util::Socket& s);
      
      //! Shutdown connection
      ~Connection(void);

      //! Return the socket
      const AIS::Util::Socket& getSocket(void) const
	{ return socket; };

      
      //! Return the protocol in use (read-only access)
      const Protocol::Base* const getProtocol(void) const
	{ return protocol; }; 
      
      //! Replace the protocol with something new
      void setProtocol(Protocol::Base& p)
	{ protocol = &p; };

      
      //! Return the number of bytes sent
      const byteCount_type getSentBytes(void) const
	{ return sentBytes; };
      
      //! Return the number of bytes sent
      const byteCount_type getReceivedBytes(void) const
	{ return receivedBytes; };
      
      //! Return the connection time
      const AIS::Util::Time& getConnectedTime(void) const 
	{ return connectedTime; };
      
      //! Return the last time this connection 'spoke'
      const AIS::Util::Time& getLastSpoke(void) const 
	{ return lastSpoke; };

      
      //! Handle input on connection
      bool handleInput(void);

      //! Send a chunk from the output queue
      void sendOutput(void);
      
      
      //! Close the connection
      void goodbye(void);
   }; // class Connection
}; // namespace Kine

#endif // _INCLUDE_KINEIRCD_CONNECTION_H_
