/* $Id$
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

# include "mod_irc2user/access.h"

using namespace Kine::mod_irc2user;


/* checkOPER - Check if the user is an IRC operator (global or local)
 * Original 19/04/2002 simonb
 */
IRC2USER_ACCESS_FUNCTION(Access::checkOPER)
{
   return false; // noop for now..
}


/* checkGLOBALOPER - Check if the user is a global IRC operator
 * Original 19/04/2002 simonb
 */
IRC2USER_ACCESS_FUNCTION(Access::checkGLOBALOPER)
{
   return false; // noop for now..
}
