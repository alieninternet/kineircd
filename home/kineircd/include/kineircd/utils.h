/* $Id$
 * Utilities
 * 
 * Copyright (c) 2001,2002 Simon Butcher <pickle@alien.net.au>
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

#ifndef _INCLUDE_KINEIRCD_UTILS_H_
# define _INCLUDE_KINEIRCD_UTILS_H_ 1

# include <kineircd/kineircdconf.h>

# include <string>
# include <cstdlib>
# include <aisutil/string/string.h>
# include <aisutil/string/mask.h>

namespace Kine {
   class Utils {
    public:
      // Type of a "base"
      typedef unsigned char base_type;
      
    private:
      Utils(void) {};				// Constructor (cannot be run)
      
    public:
      // Convert to another base
      static AISutil::String baseXStr(unsigned long, const base_type);
      
      // Convert a string to boolean
      static bool toBool(const AISutil::String &, const bool);
      
      // Fix a mask into an id mask
      static AISutil::StringMask fixToIdentityMask(const AISutil::String &);
      
      // sha1.cpp
      class SHA1 {
       public:
	 // A digest. SHA1 returns 160 bits, this makes it easier to manage.
	 union digest_type {
	    unsigned char u_char[20];
	    signed char s_char[20];
	    unsigned long u_long[5];
	    
	    // Boolean equals operator
	    bool operator==(const digest_type &d) const {
	       return ((u_long[0] == d.u_long[0]) &&
		       (u_long[1] == d.u_long[1]) &&
		       (u_long[2] == d.u_long[2]) &&
		       (u_long[3] == d.u_long[3]) &&
		       (u_long[4] == d.u_long[4]) &&
		       (u_long[5] == d.u_long[5]));
	    };
	 };
	 
	 // An empty digest
	 static const digest_type nullDigest;
	 
	 // SHA1 digest from a string
	 static digest_type generate(const std::string &);
	 
	 // Convert an SHA1 digest output to particular base
	 static std::string digestToStr(const digest_type &, 
					const base_type,
					const std::string::size_type);
      };
   };
};
   
#endif // _INCLUDE_KINEIRCD_UTILS_H_
   
