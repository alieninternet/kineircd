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

#ifndef _INCLUDE_KINEIRCD_SERVICE_H_
# define _INCLUDE_KINEIRCD_SERVICE_H_ 1

# include <aisutil/string/mask.h>
# include <kineircd/client.h>

namespace Kine {
   //! A service (refinement of a client)
   class Service : public Client {
    private: 
      const AISutil::StringMask scopeMask;	//!< Service server scope mask
      
    protected:
      //! Constructor
      Service(const AISutil::Time& _signonTime)
	: Client(_signonTime),
          scopeMask("*")
	{};
      
    public:
      //! Destructor
      virtual ~Service(void)
	{};
      
      //! Return the server scope mask
      const AISutil::StringMask& getScopeMask(void) const
	{ return scopeMask; };
      
      //! Return the service 'type' bitmask
      const long getServiceType(void) const
	{ return 0; /* replace this! */ };
   }; // class Service
}; // namespace Kine

#endif // _INCLUDE_KINEIRCD_SERVICE_H_
