[+ AutoGen5 template cpp h +][+

   ;;; schemeToCName - Convert a scheme subr name to a C procedure name
   (define (schemeName->CName name)
      (string-append
         "scm_"
         (string-substitute name
	    ;; These rules have been taken from the guile manual
            '("?"	"!"	"->"	"<="	">="	"<"	">"	"@")
	    '("_p"	"_x"	"_to_"	"_leq"	"_geq"	"_less"	"_gr"	"at"))))

+]/* $Id$
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
[+CASE (suffix)+][+== cpp+]

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif

#include "mod_irc2user/schemesubs.h"

// Make sure we are sane
#ifndef KINE_MOD_IRC2USER_SCHEME_ENABLED
# error "Cannot compile scheme support without libguile!"
#endif

using namespace Kine::mod_irc2user::Scheme;

const Subs::subrInfo_type Subs::subrInfoTable[] = {[+FOR subs+]
#ifdef KINE_MOD_IRC2USER_HAVE_[+
   (string-upcase
      (schemeName->CName
         (get "name")))
 +]
     {
	"[+name+]",
	  [+requiredParams+],
	  [+optionalParams+],
	  [+restParams+],
	  (SCM (*)())Subs::[+
   (schemeName->CName
      (get "name"))
 +]
     },
#endif[+ENDFOR+]
     { 0, 0, 0, 0 }
};
[+== h+]
[+(make-header-guard "kine")+]

# include <libguile.h>
 
namespace Kine {
   namespace mod_irc2user {
      namespace Scheme {
	 // Scheme subrs
	 namespace Subs {
	    // An array of scheme subrs and required information
	    struct subrInfo_type {
	       const char* const name;
	       const int requiredParams;
	       const int optionalParams;
	       const int restParams;
	       SCM (*function)();
	    } extern const subrInfoTable[];
	 
	    // Scheme subr functions[+FOR subs+]
	    extern SCM [+
   (schemeName->CName
      (get "name"))
 +]([+
   ;; Work out how many parameters the function requires, and output SCM's as
   ;; is appropriate
   (let outputParams ((counter (+
                                  (string->number
				     (get "requiredParams"))
                                  (string->number
				     (get "optionalParams"))
				  (if (and
				         (exist? "restParams")
					 (not
					    (=
					       (string->number
					          (get "restParams"))
					       0)))
				     1
				     0))))
      (if (positive? 
             (- counter 1))
         ;; Output us, plus the next iteration
         (string-append
            (outputParams 
               (- counter 1))
	       ", SCM")
	 "SCM"))
 +]);[+ENDFOR+]
	 }; // namespace Subs
      }; // namespace Scheme
   }; // namespace mod_irc2user
}; // namespace Kine


// Define which scheme subrs need to be compiled in[+FOR subs+]
# define KINE_MOD_IRC2USER_HAVE_[+
   (string-upcase
      (schemeName->CName
         (get "name")))
 +] 1[+ENDFOR+]


#endif // [+(. header-guard)+]
[+ESAC+]
