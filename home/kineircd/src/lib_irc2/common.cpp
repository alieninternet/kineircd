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


/* doLUSERS - Handle the 'LUSERS' command
 * Original 13/08/2001 simonb
 */
void Protocol::doLUSERS(const User& user)
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


/* doMOTD - Handle the 'MOTD' command
 * Original 13/08/2001 simonb
 */
void Protocol::doMOTD(const User& user, const bool justConnected)
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


/* doTIME - Handle the 'TIME' command
 * Original 29/08/2001 simonb
 */
void Protocol::doTIME(const User& user)
{
   // Make up a "human readable" time string
   struct tm currentTime;
   currentTime = *gmtime((const time_t*)&daemon().getTime());
   
   /* Stuff used to make a human readable time string which confirms
    * relatively strictly to what traditional IRC daemons output.
    * Unfortunately it is not internationalised, and too many 
    * scripts/bots rely on it. Hopefully this changes one day.
    */
   static const char* const months[12] = {
      "January", "February", "March", "April", "May", "June", "July",
      "August", "September", "October", "November", "December"
   };
   static const char* const days[7] = {
      "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday",
      "Saturday"
   };
   
   std::ostringstream text;
   text << std::setfill('0') <<
     days[currentTime.tm_wday] << ' ' <<
     months[currentTime.tm_mon] << ' ' <<
     currentTime.tm_mday << ' ' <<
     (currentTime.tm_year + 1900) << " -- " <<
     std::setw(2) << currentTime.tm_hour << ':' <<
     std::setw(2) << currentTime.tm_min << ' ' <<
     ((timezone > 0) ? '-' : '+') <<
     std::setw(2) << ((-timezone) / 3600) << ':' <<
     std::setw(2) << (((-timezone) % 3600) / 60);
   
   // Send the RPL_TIME reply
   sendNumeric(user, LibIRC2::Numerics::RPL_TIME,
	       config().getOptionsServerName(),
	       text.str());
   
   // Also send the RPL_TIMEONSERVERIS
   sendTimeOnServer(user);
}
