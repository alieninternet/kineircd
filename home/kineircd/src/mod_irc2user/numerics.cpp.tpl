[+ AutoGen5 template cpp +]
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
 *
 *
[+(dne " * ")+]
 */

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif

#include "mod_irc2user/scheme.h"

// Make sure we are sane
#ifndef KINE_MOD_IRC2USER_SCHEME_ENABLED
# error "Cannot compile scheme support without libguile!"
#endif

using namespace Kine::mod_irc2user;


// Name to numeric map so that schemers can use names instead of numbers
const Scheme::numericName_type Scheme::numericNames[] = {[+FOR irc2numerics+][+IF
   (not
      (exist? "repeated"))
 +]
     { [+
   ;; Neat formatting stuff ;)
   (sprintf "%-40s %3s"
      (sprintf "\"%s\","
         (get "name"))
      (get "numeric"))
 +] },[+ENDIF+][+ENDFOR+]
     { 0, 0 }
};
