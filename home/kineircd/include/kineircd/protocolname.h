/* $Id$
 * The protocol information class
 * 
 * Copyright (c) 2001,2002 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2001,2002 KineIRCd Development Team
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
 */

#ifndef _INCLUDE_KINEIRCD_PROTOCOLNAME_H_
# define _INCLUDE_KINEIRCD_PROTOCOLNAME_H_ 1

namespace Kine {
   struct ProtocolName {
      /* The name of the protocol. This takes on several roles.. If the
       * protocol specified through sane means, this can be the key to
       * resolving any ambiguity, and probably save a lot of time. This
       * string must be in capital letters.
       */
      const char* const name;
      
      struct Type { // <=- should be namespace?
	 enum type {
	    UNKNOWN = 0,		// Unset, or unknown
	    REGISTRAR_EXTENSION,	// Something that extends the registrar
	    STANDALONE_EXTENSION,	// Something that has its own registrar
	    CLIENT,			// A server<->user protocol
	    NETWORK,			// A network<->network protocol
	    SERVER,			// A server<->server protocol
	    SERVICE			// A server<->service protocol
	 };
      };
      
      // The type of protocol this is (see the enumeration above)
      const Type::type type;

      // Constructor
      ProtocolName(const char* const n, Type::type t)
	: name(n),
          type(t)
	{};
      
      // Equal-to operator
      bool operator==(const ProtocolName& a) const
	{ return ((a.type == type) && (strcmp(a.name, name) == 0)); };
      
      // Less-than operator
      bool operator<(const ProtocolName& a) const
	{ 
	   return ((a.type == type) ?
		   (strcmp(a.name, name) < 0) : (a.type < type));
	};
   };
}; // namespace Kine
   
#endif // _INCLUDE_KINEIRCD_PROTOCOLNAME_H_
   
