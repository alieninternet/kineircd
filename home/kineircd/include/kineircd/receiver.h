/* $Id$
 *
 * Copyright (c) 2002,2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2002,2003 KineIRCd Development Team
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

#ifndef _INCLUDE_KINEIRCD_RECEIVER_H_
# define _INCLUDE_KINEIRCD_RECEIVER_H_ 1

# include <kineircd/errors.h>

namespace Kine {
   class Sender;
   
   //! Abstract base class for objects which can receive messages
   class Receiver {
    public:
      //! The 'directivity' of a recipient (acuteness of target specification)
      union Directivity {
	 struct {
	    bool address:1;		//!< The IP address was specified
	    bool hostname:1;		//!< The hostname was specified
	    bool name:1;		//!< The unique name was specified
	    bool network:1;		//!< The network was specified
	    bool server:1;		//!< The server was specified
	    bool username:1;		//!< The username was specified
	 };
	 
	 unsigned char conflux;
	 
	 //! Default constructor (sets us up into the 'unknown' state)
	 Directivity(void)
	   : conflux(0)
	   {};
      };
      
    protected:
      //! Constructor
      Receiver(void)
	{};
      
    public:
      //! Destructor
      virtual ~Receiver(void)
	{};
      
      //! Send a message to this recipient
      virtual const Error::error_type
	sendMessage(Sender& from,
		    const std::string& message,
		    const Directivity directivity = Directivity())
	{ return Error::UNSUPPORTED_BY_ENTITY; };

   
      //! Send a notice to this recipient
      virtual const Error::error_type
	sendNotice(Sender& from,
		   const std::string& message,
		   const Directivity directivity = Directivity())
	{ return Error::UNSUPPORTED_BY_ENTITY; };
   }; // class Receiver
}; // namespace Kine

# include <kineircd/sender.h>

#endif // _INCLUDE_KINEIRCD_RECEIVER_H_
