/* $Id$
 * The protocol base class
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

#ifndef _INCLUDE_KINEIRCD_PROTOCOL_H_
# define _INCLUDE_KINEIRCD_PROTOCOL_H_ 1

# include <iostream>
# include <string>
# include <aisutil/socket/socket.h>

# include <kineircd/connection.h>


namespace Kine {
   class Protocol {
    protected:
      Connection& connection;			// Back up !
      
    public:
      // Constructor
      Protocol(Connection& c)
	: connection(c)
        {};
      
      // Destructor
      virtual ~Protocol(void)
	{};
      
      // Handle incoming data
      virtual void handleInput(std::stringstream& data) = 0;
      
      // Remove up to the amount of octets given from the output queue
      virtual std::string 
	withdrawOutput(AISutil::Socket::blockSize_type amount) = 0;
      
      // Return true should there be anything in the output queue to send
      virtual bool moreOutput(void) const = 0;
   };
};
   
#endif // _INCLUDE_KINEIRCD_PROTOCOL_H_
   
