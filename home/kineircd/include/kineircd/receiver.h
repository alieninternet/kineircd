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
   class Entity;
   
   //! Abstract base class for objects which can receive messages
   class Receiver {
    protected:
      //! Constructor
      Receiver(void)
	{};
      
    public:
      //! Destructor
      virtual ~Receiver(void)
	{};
      
      //! Send a message from a generic entity
      virtual const Error::error_type sendMessage(Entity& from,
						  const std::string& message)
	{ return Error::UNSUPPORTED_BY_ENTITY; };

   
      //! Send a notice from a generic entity
      virtual const Error::error_type sendNotice(Entity& from,
						 const std::string& message)
	{ return Error::UNSUPPORTED_BY_ENTITY; };
   }; // class Receiver
}; // namespace Kine

# include <kineircd/entity.h>

#endif // _INCLUDE_KINEIRCD_RECEIVER_H_
