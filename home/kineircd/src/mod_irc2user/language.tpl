[+ AutoGen5 template cpp h +][+
   ;;; Do we use the tag in the current scope?
   (define (useThisTag?)
      ;; Check if 'component' is the one for us
      (string-ci=?
         (get "component")
         "mod_irc2user"))
	    
   ;;; Define our dodgy counting thingy
   (define counter 0)
+]/* $Id$
 * 
 * Copyright (c) 2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2003 KineIRCd Development Team
 * (See DEV-TEAM file for details)
 *
 * This file is a part of KineIRCd.
 * 
 * Exordium is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * Exordium is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Exordium; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * For contact details for the KineIRCd Development Team please see the
 * enclosed README file.
 *
[+(dne " * ")+]
 */
[+ CASE (suffix) +][+ == cpp +]

#include "mod_irc2user/language.h"

using namespace Kine;

Languages::tagMap_type mod_irc2user::Language::tagMap = {[+ FOR tags +][+ IF
   (useThisTag?) 
 +]
   { "[+
   (string-upcase
      (sprintf "%s%s"
         (get "tagPrefix")
	 (get "name")))
 +]", 0 },[+ ENDIF +][+ ENDFOR +]
   { 0, 0 }
}; // tagMap
[+ == h +]
#ifndef _SRC_MOD_IRC2USER_LANGUAGE_H_
# define _SRC_MOD_IRC2USER_LANGUAGE_H_ 1

# include <kineircd/languages.h>

namespace Kine {
   namespace mod_irc2user {
      namespace Language {
         // Language tag look-up table (for our language map)
         enum {[+ FOR tags +][+ IF (useThisTag?) +][+ IF
   ;; Determine if we should put a comma here..
   (and
      (not
         (first-for?))
      (> counter 0))
 +],[+ ENDIF +]
	    [+name+] = [+
   ;; Output the counter's value..
   (. counter)
 +][+
   ;; counter++
   (set!
      counter
      (+ counter 1))
 +][+ IF 
   (not
      (last-for?))
 +][+ ENDIF +][+ ENDIF +][+ ENDFOR +]
	 };
	 
	 // The language map
	 extern Languages::tagMap_type tagMap;
      }; // namespace Language
   }; // namespace mod_irc2user
}; // namespace Kine


// Macro to make the code neater (hopefully)
# define GETLANG(n,...) \
   Kine::langs().get(origin.getLanguage(), \
                     Language::tagMap[Language::n].tagID, \
                     ##__VA_ARGS__)


#endif // _SRC_MOD_IRC2USER_LANGUAGE_H_
[+ ESAC +]
