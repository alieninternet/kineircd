[+ AutoGen5 template h +]
/* $Id$
 * Server modes
 * 
 * Copyright (c) 2002 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2002 KineIRCd Development Team 
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

#ifndef _INCLUDE_KINEIRCD_SERVERMODES_H_
# define _INCLUDE_KINEIRCD_SERVERMODES_H_ 1

namespace Kine {
   namespace Modes {
      namespace ServerModes {
         // The masks
         enum type {[+ FOR servermodes +]
	    [+ (sprintf "%-25s = 0x%08X%s //!< (+%s)"
	          (get "name")
		  (expt 2 (for-index))		; calculate the bitmask
		  (if (last-for?) " " ",")	; no comma on last iteration
		  (get "irc2char"))
             +][+ ENDFOR servermodes +]
         };
      }; // namespace ServerModes
   }; // namespace Modes
}; // namespace Kine
   
#endif // _INCLUDE_KINEIRCD_LOGGER_H_
