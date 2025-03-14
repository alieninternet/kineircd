[+ AutoGen5 template cpp +]
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
 *
 *
[+(dne " * ")+]
 */

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif

#include "mod_irc2user/commands.h"
#include "mod_irc2user/language.h"

using namespace Kine::mod_irc2user;


// Pre-initialisation command table, commands we always support
const Commands::preInitCommand_type Commands::preInitCommands[] = {[+FOR commands+]
#ifdef KINE_MOD_IRC2USER_HAVE_CMD_[+
   (string-upcase
      (get "command"))
 +]
     {
        "[+
   (string-upcase
      (get "command"))
 +]",
	  &Protocol::handle[+command+],
	  [+IF (exist? "access")+]Access::[+
   (string-upcase
      (get "access"))
 +][+ELSE+]Access::ANYBODY[+ENDIF+],
	  [+IF (exist? "disabled")+]false[+ELSE+]true[+ENDIF+], [+defaultPenalty+], [+IF
   (exist? "minimumParams")
 +][+minimumParams+][+ELSE+]0[+ENDIF+],
	  [+IF
   (exist? "noParams")
 +]0[+ELSE+]&Language::tagMap[Language::irc2user_HELP_CMD_[+command+]_USAGE].tagID[+ENDIF+],
	  &Language::tagMap[Language::irc2user_HELP_CMD_[+command+]].tagID,
	  Flags::NONE[+IF (exist? "changeIdleTime")+] |
	    Flags::CHANGE_IDLE_TIME[+ENDIF+]
     },
#endif // KINE_MOD_IRC2USER_HAVE_CMD_[+
   (string-upcase
      (get "command"))
 +][+ENDFOR+]
     { 0, 0, Access::ANYBODY, false, 0, 0, 0, 0, Flags::NONE }
};
