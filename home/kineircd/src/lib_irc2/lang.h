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

#ifndef _SRC_MOD_LIBKINEIRCD_IRC2_LANG_H_
# define _SRC_MOD_LIBKINEIRCD_IRC2_LANG_H_ 1

# include "lib_irc2/language.h"


// Dodgey macros to grab language stuff
# define GETLANG_BY_ID(id,...) \
   Kine::languages().get(user.getLanguageList(), \
                         id, \
                         ##__VA_ARGS__)

# define GETLANG(n,...) \
   GETLANG_BY_ID(Language::tagMap[Language::n].tagID, \
                 ##__VA_ARGS__)


// The prefix used for RPL_WHOISSTAFF status name lookups
# define WHOISSTAFF_TAG_PREFIX "irc2_RPL_WHOISSTAFF_"


#endif // _SRC_MOD_LIBKINEIRCD_IRC2_LANG_H_
