/* utils.h
 * Utilities
 * 
 * Copyright (c) 2001,2002 AustHex Development Team
 * (See DEV-TEAM file for details)
 *
 * This file is a part of AustHex.
 * 
 * AustHex is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * AustHex is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with AustHex; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __UTILS_H_
# define __UTILS_H_

# include <cstdlib>
# include <netinet/in.h>

# include "str.h"

// Macros
# define octStr(x)	baseXStr(x, 8)		// Convert decimal to octal
# define hexStr(x)	baseXStr(x, 16)		// Convert decimal to hex


// Utility class
class Utils {
 private:
   Utils(void) {};				// Constructor (cannot be run)
   
 public:
   // utils.cc
   static String baseXStr(unsigned long, 
			  unsigned short);	// Convert to another base
   static bool toBool(String const &, bool);	// Convert a string to boolean
   static StringMask 
     fixToIdentityMask(String const &);		// Fix a mask into an id mask

   // sha1.cc
   static String generateSHA1(String const &);	// SHA1 from as string
};


#endif

