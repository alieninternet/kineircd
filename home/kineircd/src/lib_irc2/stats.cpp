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
#include <cmath>
#include <aisutil/time.h>
#include <aisutil/string/string.h>
#include <kineircd/daemon.h>
#include <kineircd/languages.h>

#include "libkineircd_irc2/stats.h"
#include "libkineircd_irc2/lang.h"

using namespace Kine::LibIRC2;
using AISutil::String;
using AISutil::Time;


// List of statistics requests we can handle
const Stats::statsCommand_type Stats::statsCommands[] = {
     { "languages",		2,	handleLanguages },
     { "listconnections",	1,	handleListConnections },
     { "operators",		1,	handleOperators },
     { "uptime",		1,	handleUptime },
     { 0, 0, 0 }
};


/* handleLanguages - List the languages and a bit of info about them
 * Original 19/05/2003 simonb
 */
KINE_IRC2_STATS_HANDLER(Stats::handleLanguages)
{
   // Run through the known languages list
   for (Languages::languageDataMap_type::const_iterator it =
	languages().getLanguageDataMap().begin();
	it != languages().getLanguageDataMap().end();
	++it) {
      // Work out the completeness percentile of this language
      std::ostringstream percentComplete;
      percentComplete <<
	(int)rintf(((float)it->second->getTagCount() /
		    languages().getHighestTagID()) * 100) <<
	'%';
      
      // Output the data we have on this language
      protocol.sendNumeric(user, Numerics::RPL_NONE,
			   it->second->getLanguageCode(),
			   (it->second->getMaintainer().empty() ?
			    "*" : it->second->getMaintainer()),
			   it->second->getFileRevision(),
			   it->second->getTagCount(),
			   percentComplete.str(),
			   (it->second->getLanguageNote().empty() ?
			    it->second->getLanguageName() :
			    (it->second->getLanguageName() + " (" +
			     it->second->getLanguageNote() + ")")));
   }
}


/* handleListConnections - List connections (RFC1459)
 * Original 26/08/2001 simonb
 */
KINE_IRC2_STATS_HANDLER(Stats::handleListConnections)
{
   // Run through the connection list
   for (Daemon::connections_type::const_iterator it =
	daemon().getConnections().begin();
	it != daemon().getConnections().end();
	++it) {
      protocol.sendNumeric(user, Numerics::RPL_STATSLINKINFO,
			   ((((*it)->getProtocol() != 0) &&
			     ((*it)->getProtocol()->getIdentifyingName() !=
			      0)) ?
			    *(*it)->getProtocol()->getIdentifyingName() :
			    "!"),
			   0, // <=- sendq..
			   (((*it)->getProtocol() != 0) ?
			    (*it)->getProtocol()->getSentMessageCount() :
			    0),
			   ((*it)->getSentBytes() / 1024),
			   (((*it)->getProtocol() != 0) ?
			    (*it)->getProtocol()->getReceivedMessageCount() :
			    0),
			   ((*it)->getReceivedBytes() / 1024),
			   (daemon().getTime() -
			    (*it)->getConnectedTime()).seconds,
			   (false /* is secured */ ? "SSL" : "*"),
			   (((*it)->getProtocol() != 0) ?
			    (*it)->getProtocol()->getProtocolName() :
			    "*"));
   }
}


/* handleOperators - List the local and global operator information (RFC1459)
 * Original ? simonb
 */
KINE_IRC2_STATS_HANDLER(Stats::handleOperators)
{
   protocol.sendNumeric(user, Numerics::RPL_STATSOLINE,
			(true /* is global */ ? 'O' : 'o'),
			"user@host.mask",
			"*", // <=- use for flags, but RFC says '*'
			"nickname",
			"This is a description field");
}


/* handleUptime - Server uptime information (RFC1459)
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
   
   // Send the uptime
   protocol.sendNumeric(user, Numerics::RPL_STATSUPTIME,
			GETLANG(irc2_RPL_STATSUPTIME,
				String::convert(uptime.seconds / 86400),
				timestr.str()));
}
