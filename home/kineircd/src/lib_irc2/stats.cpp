/* $Id$
 * 
 * Copyright (c) 2001,2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2001,2003 KineIRCd Development Team
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

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif

#include <sstream>
#include <iomanip>
#include <aisutil/time.h>
#include <aisutil/string/string.h>
#include <kineircd/daemon.h>

#include "libkineircd_irc2/stats.h"
#include "libkineircd_irc2/lang.h"

using namespace Kine::LibIRC2;
using AISutil::String;
using AISutil::Time;


// List of statistics requests we can handle
const Stats::statsCommand_type Stats::statsCommands[] = {
     { "uptime",	1,	handleUptime },
     { 0, 0, 0 }
};


/* handleUptime - Server uptime information
 * Original 26/08/2001 simonb
 */
KINE_IRC2_STATS_HANDLER(Stats::handleUptime)
{
   // Grab (and remember) the daemon's uptime
   Time uptime = daemon().getUptime();
   
   // Formulate the time string appropriately (excluding days)
   std::ostringstream timestr;
   timestr <<
     std::setfill('0') << ((uptime.seconds % 86400) / 3600) << ':' <<
     std::setw(2) << ((uptime.seconds % 3600) / 60) << ':' <<
     std::setw(2) << (uptime.seconds % 60);
   
   // Send the up-time
   protocol.sendNumeric(user, Numerics::RPL_STATSUPTIME,
			GETLANG(irc2_RPL_STATSUPTIME,
				String::convert(uptime.seconds / 86400),
				timestr.str()));
}
