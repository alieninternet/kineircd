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

#ifndef _INCLUDE_KINEIRCD_SENDER_H_
# define _INCLUDE_KINEIRCD_SENDER_H_ 1

namespace Kine {
   class Entity;
   
   /*!
    * \brief Abstract base class for \e senders
    * 
    * A \e sender is an Entity which can send messages to Receiver objects.
    */
   class Sender : virtual public Entity {
    protected:
      //! Constructor
      Sender(void)
	{};
      
    public:
      //! Destructor
      virtual ~Sender(void)
	{};
   }; // class Sender
}; // namespace Kine

# include <kineircd/entity.h>

#endif // _INCLUDE_KINEIRCD_SENDER_H_
