[+ AutoGen5 template h +]
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

#ifndef _INCLUDE_KINEIRCD_IRC2_NUMERICS_H_
# define _INCLUDE_KINEIRCD_IRC2_NUMERICS_H_ 1

namespace Kine {
   namespace LibIRC2 {
      //! List of IRC-2 related numerics, mainly used between client <-> server
      namespace Numerics {
         enum numeric_type {[+FOR irc2numerics+][+IF
 (not
    (or
       (exist? "obsolete")
       (exist? "conflict")
       (exist? "repeated")))
 +]
	    [+
   (sprintf "%-30s = %3s, %s"
      (get "name")
      (get "numeric")
      (cond
         ((exist? "information")
	    (sprintf "// %s"
	       (get "information")))
         ((exist? "origin")
	    (sprintf "// From %s"
	       (get "origin")))
	 ((exist? "contact")
	    (sprintf "// %s"
	       (get "contact")))
	 (else
	    "")))
 +][+ENDIF+][+ENDFOR+]
            _HIGHEST_
         };
      };
   };
};

#endif // _INCLUDE_KINEIRCD_IRC2_NUMERICS_H_

