/* $Id$
 * 
 * Copyright (c) 2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2003 KineIRCd Development Team
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

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif
#include "kineircd/kineircdconf.h"

#include <cctype>

#include "kineircd/server.h"
#include "kineircd/config.h"
#include "lib/debug.h"

using namespace Kine;


/* checkValidity - Check to see if the name is a valid server hostname
 * Original 11/11/2003 pickle
 */
const Error::error_type Server::Name::checkValidity(void) const
{
   /* First off, if the hostname is invalid by 1034 standards, break!
    * 
    */
   if (length() > 255) { // RFC 1034, Section 3.1
      return Error::NAME_TOO_LONG;
   }
   
   unsigned int dotCount = 0;
   size_type labelLength = 0;
   
   // Scan the string..
   for (size_type i = 0; i != length(); ++i) {
      // Is this character a dot?
      if ((*this)[i] == L'.') {
	 // If the label length is empty, that's not good..
	 if (labelLength == 0) {
	    return Error::SERVER_NAME_HAS_DOT_DOT;
	 }
	 
	 // If the previous char is a hyphen, that's also bad..
	 if ((*this)[i - 1] == L'-') {
	    // See RFC 1034, Section 3.5
	    return Error::NAME_HAS_BAD_CHARS;
	 }
	 
	 // Increase the dot counter, reset the label length, and continue
	 ++dotCount;
	 labelLength = 0;
	 continue;
      }
      
      // If the label is too long, break
      if (++labelLength > 63) { // RFC 1034, Section 3.1
	 // This is probably an inappropriate error return..
	 return Error::NAME_TOO_LONG;
      }

      // Is this the first char of the label?
      if (labelLength == 1) {
	 // Letters are allowed for the first char (RFC 1034, Section 3.5)
	 if (std::isalpha((*this)[i])) {
	    continue;
	 }
      } else {      
	 // Alpha-numeric chars are okay (as per RFC 1034, Section 3.5)
	 if (std::isalnum((*this)[i])) {
	    continue;
	 }
	 
	 // Hyphens are also okay here
	 if ((*this)[i] == L'-') {
	    continue;
	 }
      }
      
      // If we got here, the char must be invalid..
      return Error::NAME_HAS_BAD_CHARS;
   }
   
   // Make sure there is at least one dot!
   if (dotCount == 0) {
      return Error::SERVER_NAME_HAS_NO_DOT;
   }
   
   // The hostname must be okay!!
   return Error::NO_ERROR;
}
   
   
/* addUser - Add the given user
 * Original 08/04/2003
 */
const Error::error_type Server::addUser(User& user)
{
#ifdef KINE_DEBUG
# warning "ICK!"
#endif
   // For now, fob this off to the registry - ick ick ICK ICK.
//   return registry().addUser(user);

   // All is well!
   return Error::NO_ERROR;
}
