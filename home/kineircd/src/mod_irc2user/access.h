/* $Id$
 * Access control tests
 * 
 * Copyright (c) 2001,2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2001,2003 KineIRCd Development Team
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

#ifndef _SRC_MOD_IRC2USER_ACCESS_H_
# define _SRC_MOD_IRC2USER_ACCESS_H_ 1

# include "mod_irc2user/user.h"

# define IRC2USER_ACCESS_FUNCTION(x) \
     bool x(const Kine::mod_irc2user::User& user)

namespace Kine {
   namespace mod_irc2user {
      namespace Access {
	 // The typedef for an access function.. (returns true for 'allowed')
	 typedef IRC2USER_ACCESS_FUNCTION(function_type);
	 
	 // Check if the user is an IRC operator (global or local)
	 static IRC2USER_ACCESS_FUNCTION(checkOPER)
	   { return false; }; // noop for now..
	 
	 // Check if the user is a global IRC operator
	 static IRC2USER_ACCESS_FUNCTION(checkGLOBALOPER)
	   { return false; }; // noop for now..
      }; // namespace Access
   }; // namespace mod_irc2user
}; // namespace Kine
   
#endif // _SRC_MOD_IRC2USER_ACCESS_H_
   
