[+ AutoGen5 template h +]
/* $Id$
 * Error returns from functions
 *
 * Copyright (c) 2001,2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2001,2003 Alien Internet Services
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

[+(make-header-guard "kine")+]

namespace Kine {
   namespace Error {
      class Events {
       public:
	 struct Mask { // <=- should be namespace
	    enum mask_type {[+FOR mask ","+]
	       //! [+comment+]
	       [+
   (sprintf "%-25s = 0x%08X"
      (get "name")
      (expt 2
         (for-index)))
 +][+ENDFOR+]
	    };
	    
	    typedef unsigned int lazy_type;
	 };
      }; // class Events
   }; // namespace Error
}; // namespace Kine

#endif [+(. header-guard)+]
