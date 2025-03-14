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

#include "kineircd/network.h"
#include "kineircd/config.h"
#include "lib/debug.h"

using namespace Kine;


/* checkValidity - Check to see if the name is a valid network name
 * Original 11/11/2003 pickle
 */
const Error::error_type Network::Name::checkValidity(void) const
{
   // Check the length..
//   if (length() > ??) {
//      return Error::NAME_TOO_LONG;
//   }
   
   // Currently the network name rules are pretty flimsy..
   if (find_first_of(L"\0\007\012\015 ,./@\\:", 1) != npos) {
      return Error::NAME_HAS_BAD_CHARS;
   }
   
   // I guess the name is okay then!
   return Error::NO_ERROR;
}
