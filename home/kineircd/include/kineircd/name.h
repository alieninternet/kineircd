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

#ifndef _INCLUDE_KINEIRCD_NAME_H_
# define _INCLUDE_KINEIRCD_NAME_H_ 1

# include <aisutil/string/string.h>

namespace Kine {
   //! A name, conforming to IRC rules (a specially modified form of a string)
   class Name : public AISutil::String {
    public:
      // Constructor
      Name(void)
	{};
      
      // Constructor (a magic one)
      template <class Ta> Name(const Ta& a)
	: AISutil::String(a)
        {};
      
      // Destructor
      ~Name(void)
	{};
      
      //! Lower-case the nickname based on RFC-1459 rules
      const AISutil::String IRCtoLower(void) const;
   }; // class Name
}; // namespace Kine

#endif // _INCLUDE_KINEIRCD_NAME_H_
