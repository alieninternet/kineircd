/* $Id$
 * Listener class (a listener is a 'connection' with one purpose: listening :)
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

#ifndef _INCLUDE_KINEIRCD_LISTENER_H_
# define _INCLUDE_KINEIRCD_LISTENER_H_ 1

# include <aisutil/socket/socket.h>

namespace Kine {
   class Listener {
    public:
      struct Flags { // <=- Should be namespace?
	 enum type {
	    ALLOW_NETWORKS	= 0x00000001,	//!< IIRC links allowed?
	    ALLOW_SERVERS	= 0x00000002,	//!< Servers allowed?
	    ALLOW_SERVICES	= 0x00000004,	//!< Services allowed?
	    ALLOW_USERS		= 0x00000008	//!< Users allowed?
	 };
      };
      typedef unsigned int flags_type;
      
    private:
      // The socket
      AISutil::Socket& socket;
      
      // The pending acceptance backlog for this socket (for when we listen())
      unsigned short listenBacklog;
      
      // Flags (see above)
      flags_type flags;	
      
      // Are we listening?
      bool listening;
      
      // How many accept()'s have we done since our creation?
      unsigned long int acceptCount;
      
    public:
      // Constructor
      Listener(AISutil::Socket& s, const unsigned short lb, 
	       const flags_type f = 0)
	: socket(s),
          listenBacklog(lb),
          flags(f),
          listening(false),
          acceptCount(0)
	{};
      
      // Destructor
      ~Listener(void)
	{ delete &socket; };
      
      // Make the socket listen
      const bool listen(void);
      
      // Are we listening?
      const bool isListening(void) const
	{ return listening; };
      
      // Return the flags
      const flags_type getFlags(void) const
	{ return flags; };
      
      // Check if a specific flag is set
      const bool isFlagSet(const Flags::type flag) const
	{ return (flags & flag); };
      
      // Return the number of accepts we have done since we started
      const unsigned long int getAcceptCount(void) const
	{ return acceptCount; };
      
      // Return the socket's file descriptor
      const int getFD(void) const
	{ return socket.getFD(); };
      
      // Accept a new connection from the socket
      AISutil::Socket* const accept(void)
	{
	   acceptCount++;
	   return socket.accept();
	};
   };
};
   
#endif // _INCLUDE_KINEIRCD_LISTENER_H_
