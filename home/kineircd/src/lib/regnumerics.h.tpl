[+ AutoGen5 template h +]
/* $Id$
 * Tiny list of numerics potentially used during the registration of a client
 * 
 * Copyright (c) 2002 Alien Internet Services
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

#ifndef _SRC_LIB_REGNUMERICS_H_
# define _SRC_LIB_REGNUMERICS_H_ 1

namespace Kine {
   namespace RegistrationNumerics {
      enum numeric_type {[+FOR irc2numerics +][+IF (get "registration")+]
      [+IF (or (get "obsolete") (get "conflict"))+]//[+ELSE+]  [+ENDIF+] [+name+] = [+numeric+],[+ENDIF+][+ENDFOR irc2numerics+]
         _LAST_
      };
   };
};

#endif // _SRC_LIB_REGNUMERICS_H_

