[+ AutoGen5 template h +]
/* $Id$
 * A server
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

#ifndef _INCLUDE_KINEIRCD_SERVER_H_
# define _INCLUDE_KINEIRCD_SERVER_H_ 1

# include <kineircd/entity.h>
# include <kineircd/sender.h>

namespace Kine {
   class Server : public Entity, public Sender {
    public:
      //! Server modes type 
      typedef unsigned long modes_type;
      
      // Server modes...
      namespace Mode {
	 enum mode_type {[+ FOR servermodes +]
	    [+ (sprintf "%-25s = 0x%08X%s //!< (+%s)"
	          (get "name")
		  (expt 2 (for-index))		;; calculate the bitmask
		  (if (last-for?) " " ",")	;; no comma on last iteration
		  (get "irc2char"))
	     +][+ ENDFOR servermodes +]
	 };
      };
      
    private:
      modes_type modes;				//!< Server modes bit mask
      
    public:
      //! Constructor
      Server(void)
	{};
      
      //! Destructor
      virtual ~Server(void)
	{};
      
      //! Return the server modes
      const modes getModes(void) const
	{ return modes; };
      
      //! Check if the given mode is set
      const bool isModeSet(const Mode::mode_type mode) const
	{ return (modes & mode); };
   }; // class Server
}; // namespace Kine

#endif // _INCLUDE_KINEIRCD_SERVER_H_
