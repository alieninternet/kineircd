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

# include "kineircd/socket.h"


namespace Kine {
   class Listener {
    public:
      enum {
	 FLAG_ALLOW_NETWORKS	= 0x00000001,	// IIRC links allowed?
	 FLAG_ALLOW_SERVERS	= 0x00000002,	// Servers allowed?
	 FLAG_ALLOW_SERVICES	= 0x00000004,	// Services allowed?
	 FLAG_ALLOW_USERS	= 0x00000008	// Users allowed?
      };
      typedef unsigned int flags_type;
      
    private:
      Socket* const socket;				// The socket
      bool listening;					// Are we listening?
      flags_type flags;					// Flags (see above)
      
    public:
      // Constructor
      Listener(Socket* const s = 0, flags_type f = 0)
	: socket(s),
          flags(f)
	{};
      
      // Destructor
      ~Listener(void)
	{
	   if (socket != 0) {
	      delete socket;
	   }
	};
      
      // Grab the socket
      const Socket& getSocket(void) const
	{ return *socket; };
      
      // Make the socket listen
      const bool listen(void);
      
      // Are we listening?
      const bool isListening(void) const
	{ return listening; };
      
      // Return the flags
      const flags_type getFlags(void) const
	{ return flags; };
   };
};
   
#endif // _INCLUDE_KINEIRCD_LISTENER_H_
