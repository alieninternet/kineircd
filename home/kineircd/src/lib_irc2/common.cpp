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

#include <ctime>
#include <sstream>
#include <iomanip>
#include <aisutil/string/string.h>
#include <kineircd/config.h>
#include <kineircd/daemon.h>
#include <kineircd/registry.h>

#include "kineircd/irc2/protocol.h"
#include "libkineircd_irc2/language.h"

using namespace Kine::LibIRC2;
using AISutil::String;


// Dodgey macro to grab language stuff
# define GETLANG(n,...) \
   Kine::languages().get(user.getLanguageList(), \
			 Language::tagMap[Language::n].tagID, \
                         ##__VA_ARGS__)


/* sendLUSERS - Send a LUSERS reply to the user
 * Original 13/08/2001 simonb
 */
void Protocol::sendLUSERS(const User& user)
{
   sendNumeric(user, Numerics::RPL_LUSERCLIENT,
	       GETLANG(irc2_RPL_LUSERCLIENT,
		       String::convert(registry().getUsers().size()),
		       String::convert(0),
		       String::convert(0)));
   sendNumeric(user, Numerics::RPL_LUSEROP,
	       0,
	       GETLANG(irc2_RPL_LUSEROP));
   sendNumeric(user, Numerics::RPL_LUSERSTAFF,
	       0,
	       GETLANG(irc2_RPL_LUSERSTAFF));
   sendNumeric(user, Numerics::RPL_LUSERUNKNOWN,
	       0,
	       GETLANG(irc2_RPL_LUSERUNKNOWN));
   sendNumeric(user, Numerics::RPL_LUSERCHANNELS,
	       0,
	       GETLANG(irc2_RPL_LUSERCHANNELS));
   sendNumeric(user, Numerics::RPL_LUSERME,
	       GETLANG(irc2_RPL_LUSERME,
		       String::convert(registry().getLocalUsers().size()),
		       String::convert(0)));
   sendNumeric(user, Numerics::RPL_LOCALUSERS,
	       GETLANG(irc2_RPL_LOCALUSERS,
		       String::convert(registry().getLocalUsers().size()),
		       String::convert(0)));
   sendNumeric(user, Numerics::RPL_GLOBALUSERS,
	       GETLANG(irc2_RPL_GLOBALUSERS,
		       String::convert(registry().getUsers().size()),
		       String::convert(0)));
}


/* sendMOTD - Send our MOTD to the user
 * Original 13/08/2001 simonb
 */
void Protocol::sendMOTD(const User& user, const bool justConnected)
{
   // Send the MOTD header
   sendNumeric(user, Numerics::RPL_MOTDSTART,
	       GETLANG(irc2_RPL_MOTDSTART,
		       Kine::config().getOptionsServerName()));

   // Send this line
   sendNumeric(user, Numerics::RPL_MOTD,
	       "- This is MOTD data");
   
   // Send the MOTD footer
   sendNumeric(user, Numerics::RPL_ENDOFMOTD,
	       GETLANG(irc2_RPL_ENDOFMOTD));
}


/* sendTimeOnServer - Send RPL_TIMEONSERVERIS
 * Original 12/08/2003 simonb
 */
void Protocol::sendTimeOnServer(const User& user)
{
   std::ostringstream data;
   
   // Compile the time data (it's messy, so we compile the fields by hand)
   data << 
     daemon().getTime().seconds << ' ' <<
     daemon().getTime().nanoseconds << ' ';
   
   /* Work out the timezone.. This is a pain in the arse, because the stupid
    * Americans get timezones in C in seconds *WEST* of Greenwich. Sorry to say
    * the Americans, in their infinite wisdom, have forgotten, once again,
    * that the entire WORLD specifies timezones in terms of being *EAST* of
    * Greenwich, as it has always been (it's no coincidence that the world
    * rotates in such a direction for this to be logical). It's funny to note
    * this seems just as stupid as the %D formatting code in strftime().
    * At least you know why we're processing this integer backwards.
    */
   data <<
     ((timezone > 0) ? '-' : '+') << std::setfill('0') <<
     std::setw(2) << ((-timezone) / 3600) <<
     std::setw(2) << (((-timezone) % 3600) / 60) <<
     ' ';
   
   // Yes, no flags.. yet..
   data << '*';
   
   // Send the time
   sendNumeric(user, Numerics::RPL_TIMEONSERVERIS,
	       data.str(),
	       GETLANG(irc2_RPL_TIMEONSERVERIS));
}
