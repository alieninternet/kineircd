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
    private:
      Socket* socket;
      bool listening;
      
    public:
      // Constructor   
      Listener(Socket* s = 0)
	: socket(s)
	  {};
      
      // Destructor
      ~Listener(void)
	{ delete socket; };
      
      // Grab the socket
      const Socket* const getSocket(void) const
	{ return socket; };
      
      // Make the socket listen
      const bool listen(void)
	{ return (listening = (socket->bind() && socket->listen())); };
      
      // Are we listening?
      const bool isListening(void) const
	{ return listening; };
   };
};
   
#endif // _INCLUDE_KINEIRCD_LISTENER_H_

