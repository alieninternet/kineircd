[+ AutoGen5 template cpp h +][+

   ;;; Try to be sane.. For this template to work, 'COMPONENT' must be set
   (if
      (string-null?
         (getenv "COMPONENT"))
      (error "COMPONENT was not defined"))
      
 +][+
 
   ;;; We want the SRFI-13 module (string library)
   (use-modules (srfi srfi-13))



   ;;; Work out what namespace to use (either the one given, or make up one)
   (define (namespace)
      (if
         (eq?
            (getenv "NAMESPACE")
	    #f)
         (getenv "COMPONENT")
         (getenv "NAMESPACE")))



   ;;; Determine the path used for includes and such for this component
   (define (componentIncludePath)
      (if
         (eq?
            (getenv "SUBDIR")
	    #f)
         (getenv "COMPONENT")
         (getenv "SUBDIR")))



   ;;; Do we use the tag in the current scope?
   (define (useThisTag?)
      ;; Check if 'components' is the one for us
      (if
         (eq?
	    (string-contains-ci
               (get "components")
               (getenv "COMPONENT"))
	    #f)
	 #f
	 #t))


   ;;; Define our dodgy counting thingy
   (define counter 0)
	 
 +]/* $Id$
 * 
 * Copyright (c) 2003,2004 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2003,2004 KineIRCd Development Team
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
 * For contact details for the KineIRCd Development Team please see the
 * enclosed README file.
 *
[+(dne " * ")+]
 */
[+ CASE (suffix) +][+ == cpp +]

#include "[+(componentIncludePath)+]/language.h"

using namespace Kine;

Languages::tagMap_type [+(namespace)+]::Language::tagMap = {[+ FOR tag +][+ IF
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
[+(make-header-guard "kine")+]

# include <kineircd/languages.h>

namespace Kine {
   namespace [+(namespace)+] {
      namespace Language {
         // Language tag look-up table (for our language map)
         enum {[+ FOR tag +][+ IF (useThisTag?) +][+ IF
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
   }; // namespace [+(namespace)+]
}; // namespace Kine

#endif // [+(. header-guard)+]
[+ ESAC +]
