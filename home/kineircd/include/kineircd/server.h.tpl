[+ AutoGen5 template h +]
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

[+(make-header-guard "kine")+]

# include <kineircd/entity.h>
# include <kineircd/sender.h>

namespace Kine {
   //! A server
   class Server : public Entity, public Sender {
    public:
      //! Server modes type 
      typedef unsigned long modes_type;
      
      // Server modes...
      struct Mode { // <=- should be namespace?
	 enum mode_type {[+ FOR servermodes +]
	    [+ (sprintf "%-25s = 0x%08X%s //!< (+%s)"
	          (get "name")
		  (expt 2 (for-index))		;; calculate the bitmask
		  (if (last-for?) " " ",")	;; no comma on last iteration
		  (get "irc2char"))
	     +][+ ENDFOR servermodes +]
	 };
      };
      
    protected:
      //! Constructor
      Server(const AISutil::Time& _signonTime)
	: Entity(_signonTime)
	{};

    public:
      //! Destructor
      virtual ~Server(void)
	{};
      
      //! Return the 'name' (a server's hostname is normally its name)
      virtual const std::string& getName(void) const
	{ return getHostname(); };
	
      //! Return the server modes
      virtual const modes_type getModes(void) const = 0;
      
      //! Check if the given mode is set
      const bool isModeSet(const Mode::mode_type mode) const
	{ return (getModes() & mode); };
   }; // class Server
}; // namespace Kine

#endif // [+(. header-guard)+]
