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

#include "mod_irc2user/user.h"
#include "mod_irc2user/language.h"

using namespace Kine::mod_irc2user;


// Alternative GETLANG() macro (since the getLanguageList() call is local)
# define GETLANG(n,...) \
   Kine::languages().get(getLanguageList(), \
			 Language::tagMap[Language::n].tagID, \
                         ##__VA_ARGS__)



/* doEventAwayToggle - Called when we are going away/returning away
 * Original 16/04/2003 simonb
 */
void User::doEventAwayToggle(void)
{
   // Have we gone away or are we returning?
   if (isAway()) {
      // Tell the user they are now known to be away
      protocol.sendNumeric(LibIRC2::Numerics::RPL_NOWAWAY,
			   GETLANG(irc2_RPL_NOWAWAY));
      return;
   }
   
   // Tell the user they are now back, as far as we know
   protocol.sendNumeric(LibIRC2::Numerics::RPL_UNAWAY,
			GETLANG(irc2_RPL_UNAWAY));
}
