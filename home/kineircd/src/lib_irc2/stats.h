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
 */

#ifndef _SRC_MOD_LIBKINEIRCD_IRC2_STATS_H_
# define _SRC_MOD_LIBKINEIRCD_IRC2_STATS_H_ 1

# include "kineircd/libirc2/protocol.h"
# include "kineircd/client.h"


// Handy define
# define KINE_IRC2_STATS_HANDLER(x) \
     void x(Protocol& protocol, const User& user)


namespace Kine {
   namespace LibIRC2 {
      // Stats request handlers
      namespace Stats {
	 // Handy typedef
	 typedef KINE_IRC2_STATS_HANDLER(handler_type);
	 
	 // List of statistics requests we can provide for
	 struct statsCommand_type {
	    const char* const request;		// Request name (lower-case)
	    const unsigned int minChars;	// Minimum chars needed
	    handler_type* const handler;	// Stats reply function
	 } extern const statsCommands[];
	 
	 // Statistical request handler functions
	 extern handler_type handleLanguages;
	 extern handler_type handleListConnections;
	 extern handler_type handleOperators;
	 extern handler_type handleResources;
	 extern handler_type handleUptime;
      };
   }; // namespace LibIRC2
}; // namespace Kine

#endif // _SRC_MOD_LIBKINEIRCD_IRC2_STATS_H_
   
