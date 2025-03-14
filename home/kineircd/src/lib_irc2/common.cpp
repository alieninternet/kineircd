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
#include <cstring>
#ifdef KINE_DEBUG_ASSERT
# include <cassert>
#endif
#include <algorithm>
#ifdef HAVE_UTMPX_H
# include <utmpx.h>
#else
# ifdef HAVE_UTMP_H
#  include <utmp.h>
# endif
#endif
#include <aisutil/string/string.h>
#include <aisutil/string/tokens.h>
#include <kineircd/config.h>
#include <kineircd/myserver.h>
#include <kineircd/mynetwork.h>
#include <kineircd/daemon.h>
#include <kineircd/version.h>
#include <kineircd/localuser.h>

#include "kineircd/libirc2/protocol.h"
#include "lib_irc2/lang.h"
#include "lib_irc2/stats.h"

using namespace Kine::LibIRC2;
using AIS::Util::String;
using AIS::Util::StringTokens;


/* sendTimeOnServer - Send RPL_TIMEONSERVERIS
 * Original 12/08/2001 simonb
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
   data << replacementCharacter;
   
   // Send the time
   sendNumeric(user, Numerics::RPL_TIMEONSERVERIS,
	       data.str(),
	       GETLANG(irc2_RPL_TIMEONSERVERIS));
}


/* doADMIN
 * Original 27/08/2001 simonb
 */
void Protocol::doADMIN(const User& user)
{
   // Do we have administrative any information?
   if (Kine::config().getAdministratorName().empty() &&
       Kine::config().getAdministratorLocation().empty() &&
       Kine::config().getAdministratorContact().empty()) {
      sendNumeric(user, LibIRC2::Numerics::ERR_NOADMININFO,
		  delocaliseStr(Kine::myServer().getName()),
		  GETLANG(irc2_ERR_NOADMININFO));
      return;
   }
   
   // Send the admin header
   sendNumeric(user, LibIRC2::Numerics::RPL_ADMINME,
	       delocaliseStr(Kine::myServer().getName()),
	       GETLANG(irc2_RPL_ADMINME,
		       Kine::myServer().getName()));
   
   // Send the administrator's name
   if (!Kine::config().getAdministratorName().empty()) {
      sendNumeric(user, LibIRC2::Numerics::RPL_ADMINLOC1,
		  Kine::config().getAdministratorName());
   }
   
   // Send the administrator's location
   if (!Kine::config().getAdministratorLocation().empty()) {
      sendNumeric(user, LibIRC2::Numerics::RPL_ADMINLOC2,
		  Kine::config().getAdministratorLocation());
   }
   
   // Send the administrator's contact address
   if (!Kine::config().getAdministratorContact().empty()) {
      sendNumeric(user, LibIRC2::Numerics::RPL_ADMINEMAIL,
		  Kine::config().getAdministratorContact());
   }
}


/* doINFO
 * Original 14/08/2001 simonb
 */
void Protocol::doINFO(const User& user)
{
   // Send info lines...
   for (Daemon::info_type::const_iterator it = daemon().getInfo().begin();
	it != daemon().getInfo().end();
	++it) {
      sendNumeric(user, Numerics::RPL_INFO,
		  delocaliseStr(*it));
   }

   // End of the list..
   sendNumeric(user, Numerics::RPL_ENDOFINFO,
	       GETLANG(irc2_RPL_ENDOFINFO));
}


/* doLUSERS
 * Original 13/08/2001 simonb
 */
void Protocol::doLUSERS(const User& user)
{
   sendNumeric(user, Numerics::RPL_LUSERCLIENT,
	       GETLANG(irc2_RPL_LUSERCLIENT,
		       Languages::toWideStr(String::convert(myNetwork().getUserCount())),
		       Languages::toWideStr(String::convert(myNetwork().getServiceCount())),
		       Languages::toWideStr(String::convert(myNetwork().getServerCount()))));
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
	       myNetwork().getChannelCount(),
	       GETLANG(irc2_RPL_LUSERCHANNELS));
   sendNumeric(user, Numerics::RPL_LUSERME,
	       GETLANG(irc2_RPL_LUSERME,
		       Languages::toWideStr(String::convert(myServer().getClientCount())),
		       Languages::toWideStr(String::convert(myServer().getServerCount()))));
   sendNumeric(user, Numerics::RPL_LOCALUSERS,
	       GETLANG(irc2_RPL_LOCALUSERS,
		       Languages::toWideStr(String::convert(myServer().getUserCount().getValue())),
		       Languages::toWideStr(String::convert(myServer().getUserCount().getPeak()))));
   sendNumeric(user, Numerics::RPL_GLOBALUSERS,
	       GETLANG(irc2_RPL_GLOBALUSERS,
		       Languages::toWideStr(String::convert(myNetwork().getUserCount())),
		       Languages::toWideStr(String::convert(myNetwork().getUserCount().getPeak()))));
}


/* doMOTD
 * Original 13/08/2001 simonb
 */
void Protocol::doMOTD(const User& user, const bool justConnected)
{
   // Send the MOTD header
   sendNumeric(user, Numerics::RPL_MOTDSTART,
	       GETLANG(irc2_RPL_MOTDSTART,
		       Kine::myServer().getName()));

   // Send this line
   sendNumeric(user, Numerics::RPL_MOTD,
	       "- This is MOTD data");
   
   // Send the MOTD footer
   sendNumeric(user, Numerics::RPL_ENDOFMOTD,
	       GETLANG(irc2_RPL_ENDOFMOTD));
}


/* handleSTATS
 * Original 14/08/2001 simonb
 */
void Protocol::doSTATS(const User& user, const std::string& request)
{
   // Fix up the request for easy searching (note the down-cast)
   std::string req(static_cast<const String&>(request).toLower());
   
   // Run through the list of stats commands
   for (unsigned int i = 0; Stats::statsCommands[i].request != 0; ++i) {
      // Check for a match (only the require chars are needed)
      if (!strncmp(req.data(),
		   Stats::statsCommands[i].request,
		   std::max(req.length(),
			    Stats::statsCommands[i].minChars))) {
#ifdef KINE_DEBUG_ASSERT
	 // The handler should not be null!
	 assert(Stats::statsCommands[i].handler != 0);
#endif
	 
	 // Run the function and break out of the loop
	 Stats::statsCommands[i].handler(*this, user);
	 break;
      }
   }
   
   // Send the footer
   sendNumeric(user, Numerics::RPL_ENDOFSTATS,
	       request,
	       GETLANG(irc2_RPL_ENDOFSTATS));
}


/* handleSUMMON
 * Original 10/04/2003 simonb
 */
void Protocol::doSUMMON(const User& user, const std::string& username)
{
   // We only reply to conform to specs..
   sendNumeric(user, LibIRC2::Numerics::ERR_SUMMONDISABLED,
	       GETLANG(irc2_ERR_SUMMONDISABLED));
}


/* doTIME
 * Original 29/08/2001 simonb
 */
void Protocol::doTIME(const User& user)
{
   // Firstly, send the RPL_TIMEONSERVERIS thing
   sendTimeOnServer(user);
   
   // Make up the "human readable" time string for the old RPL_TIME..
   struct tm currentTime;
   currentTime = *localtime((const time_t*)&daemon().getTime());
   
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
	       delocaliseStr(myServer().getName()),
	       text.str());
}


/* handleUSERS
 * Original 10/04/2003 simonb
 */
void Protocol::doUSERS(const User& user)
{
#if defined(HAVE_GETUTENT) || defined(HAVE_GETUTXENT)
   if (!config().getOptionsEnableUsersCommand()) {
#endif
      // Tell the user that users has been disabled (to conform to specs
      sendNumeric(user, LibIRC2::Numerics::ERR_USERSDISABLED,
		  GETLANG(irc2_ERR_USERSDISABLED));
#if defined(HAVE_GETUTENT) || defined(HAVE_GETUTXENT)
      return;
   }
   
   // Open the UTMP file, and reset it to the top
   setutxent();
   
   // This is the users structure
   const utmpx* utmpEntry;
 
   // Send the header
   sendNumeric(user, LibIRC2::Numerics::RPL_USERSSTART,
	       GETLANG(irc2_RPL_USERSSTART));

   // We'll use this to know if we've told the user about any users
   bool haveUsers = false;
   
   // Iterate over the UTMP file
   while ((utmpEntry = getutxent()) != 0) {
      // If this is a logged in user, then we can do something with it
      if (utmpEntry->ut_type == USER_PROCESS) {
	 // Assemble the user information
	 std::ostringstream output;
	 output << std::setiosflags(std::ios::left) <<
	   std::setw(8) << utmpEntry->ut_user << ' ' <<
	   std::setw(9) << utmpEntry->ut_line << ' ' <<
	   utmpEntry->ut_host;
	 
	 // Send the information about this user
	 sendNumeric(user, LibIRC2::Numerics::RPL_USERS,
		     output.str());
	 
	 // Tell ourself that we've outputted a user record
	 haveUsers = true;
      }
   }

   /* If we didn't have any useful user information, tell the user there are
    * no users logged in...
    */
   if (!haveUsers) {
      sendNumeric(user, LibIRC2::Numerics::RPL_NOUSERS,
		  GETLANG(irc2_RPL_NOUSERS));
   }
   
   // Send the footer
   sendNumeric(user, LibIRC2::Numerics::RPL_ENDOFUSERS,
	       GETLANG(irc2_RPL_ENDOFUSERS));
   
   // Close the UTMP file
   endutxent();
#endif
}


/* doVERSION
 * Original 24/08/2001 simonb
 */
void Protocol::doVERSION(const User& user)
{
   // Send the RPL_VERSION reply
   sendNumeric(user, LibIRC2::Numerics::RPL_VERSION,
	       Version::version,
	       delocaliseStr(myServer().getName()),
	       Version::versionChars);
}


/* doWHOIS
 * Original 23/08/2001 simonb
 * Note: Still incomplete
 */
void Protocol::doWHOIS(const User& user, const std::string& targets)
{
   // Tokenise the targets list
   StringTokens st(targets);
   
   while (st.hasMoreTokens()) {
      // Find out who to look for
      std::string target = st.nextToken(',');

      // Find this target.. First look to see if it is a user
      User* const foundUser = myNetwork().findUser(localiseStr(target));
      
      // If we did not find a user, look for this as a service
      Service* const foundService = 
	((foundUser == 0) ? myNetwork().findService(localiseStr(target)) : 0);
      
      // Whatever it is, it's a client, and we want to keep the code simple :)
      Client* const foundClient =
	((foundUser != 0) ? 
	 static_cast<Client* const>(foundUser) : 
	 static_cast<Client* const>(foundService));
      
      // Make sure we found that client
      if (foundClient != 0) {
	 // Is the host hidden?
	 const bool hideHost =
	   ((foundService == 0) ?
	    foundUser->hideHostFrom(user) :
	    (user.isStaff() || user.isOperator()));
	 
	 // Check if this user is allowed to see the real hostname of this user
	 if (hideHost) {
	    // Send the user details with the virtual hostname
	    sendNumeric(user, LibIRC2::Numerics::RPL_WHOISUSER,
			delocaliseStr(foundClient->getNickname()),
			delocaliseStr(foundClient->getUsername()),
			((foundUser != 0) ?
			 delocaliseStr(foundUser->getVirtualHostname()) :
			 replacementParameter), // <=- simply mask the host
			replacementCharacter,
			delocaliseStr(foundClient->getDescription()));
	 } else {
	    // Send the user entry as normal
	    sendNumeric(user, LibIRC2::Numerics::RPL_WHOISUSER,
			delocaliseStr(foundClient->getNickname()),
			delocaliseStr(foundClient->getUsername()),
			delocaliseStr(foundClient->getHostname()),
			replacementCharacter,
			delocaliseStr(foundClient->getDescription()));
	    
	    // If this is a user, also send the virtual host details
	    if (foundUser != 0) {
	       sendNumeric(user, LibIRC2::Numerics::RPL_WHOIS_HIDDEN,
			   delocaliseStr(foundUser->getNickname()),
			   delocaliseStr(foundUser->getVirtualHostname()),
			   GETLANG(irc2_RPL_WHOIS_HIDDEN));
	    }
	 }

	 /* If the client is a normal user (or the caller is an operator),
	  * potentially compile a list of channels the user is on. Also check
	  * if the user is even on any channels!
	  */
	 if (((foundService == 0) || user.isStaff()) &&
	     (false /*!foundService->getChannelList().empty()*/)) {
	    sendNumeric(user, LibIRC2::Numerics::RPL_WHOISCHANNELS,
			delocaliseStr(foundClient->getNickname()),
			"#foo +!bah12345 @&baz %.qux");
	 }
	 
	 // If the server the client is on is not hidden, send that too
	 if ((!foundClient->getServer().isHiddenFrom(user) &&
	      (foundService == 0)) ||
	     user.isStaff()) {
	    sendNumeric(user, LibIRC2::Numerics::RPL_WHOISSERVER,
			delocaliseStr(foundClient->getNickname()),
			delocaliseStr(foundClient->getServer().getName()),
			delocaliseStr(foundClient->getServer().getDescription()));
	 }
	 
	 // If this is a user, we have lots more we can say
	 if (foundUser != 0) {
	    // If this user has language info, we should send it too
	    if (!foundUser->getLanguageList().empty()) {
	       std::ostringstream langs;
	    
	       // Assemble the language information for this user
	       for (Languages::languageDataList_type::const_iterator it =
		    foundUser->getLanguageList().begin();
		    it != foundUser->getLanguageList().end();
		    ++it) {
		  // Can we output this langauge?
		  if (*it != 0) {
		     // Do we need to output a delimeter here?
		     if (!langs.str().empty()) {
			langs << ',';
		     }
		     
		     // Output the language code
		     langs << (*it)->getLanguageCode();
		  }
	       }
	       
	       // Send the language info
	       sendNumeric(user, LibIRC2::Numerics::RPL_WHOISLANGUAGE,
			   delocaliseStr(foundUser->getNickname()),
			   langs.str(),
			   GETLANG(irc2_RPL_WHOISLANGUAGE));
	    }
	 
	    // If the user is marked away, send their away information
	    if (foundUser->isAway()) {
	       sendNumeric(user, LibIRC2::Numerics::RPL_AWAY,
			   delocaliseStr(foundUser->getNickname()),
			   (daemon().getTime() -
			    foundUser->getAwaySince()).seconds,
			   delocaliseStr(foundUser->getAwayMessage()));
	    }
	 
	    // If the user is an IRC operator, tell the world about it
	    if (foundUser->isGlobalOperator()) {
	       sendNumeric(user, LibIRC2::Numerics::RPL_WHOISOPERATOR,
			   delocaliseStr(foundUser->getNickname()),
			   GETLANG(irc2_RPL_WHOISOPERATOR));
	    } else if (foundUser->isLocalOperator()) {
	       sendNumeric(user, LibIRC2::Numerics::RPL_WHOISOPERATOR,
			   delocaliseStr(foundUser->getNickname()),
			   GETLANG(irc2_RPL_WHOISOPERATOR_LOCAL));
	    }
	 
	    /* If this user is a network staff member (i.e. network services),
	     * then we should also add this to the list
	     */
	    if (foundUser->isStaff()) {
	       /* Try to locate an appropriate tag ID which describes this
		* user's job..
		*/
	       std::string text;
	       Languages::tagID_type tagID;
	       if ((tagID = 
		    languages().getTagID(WHOISSTAFF_TAG_PREFIX +
					 foundUser->getStaffStatus())) !=
		   Languages::unknownTagID) {
		  /* Output the data at the tag we found (and hope there is
		   * data for the user's selected language(s)!!)
		   */
		  text = GETLANG_BY_ID(tagID);
	       } else {
		  /* We cannot work out what kind of staff member, or we do not
		   * have the appropriate language data to say what kind of
		   * staff member this person is, so we'll just say they're a 
		   * staff member and leave it at that.
		   */
		  text = GETLANG(irc2_RPL_WHOISSTAFF);
	       }
	       
	       // Send it, with whatever language text we could find..
	       sendNumeric(user, LibIRC2::Numerics::RPL_WHOISSTAFF,
			   delocaliseStr(foundUser->getNickname()),
			   text);
	    }
	    
	    // See if the user is locally connected
	    const LocalUser* const foundLocalUser =
	      dynamic_cast<const LocalUser* const>(foundUser);
	    if (foundLocalUser != 0) {
	       // Output the idle time, and the time this user connected
	       sendNumeric(user, LibIRC2::Numerics::RPL_WHOISIDLE,
			   delocaliseStr(foundUser->getNickname()),
			   (daemon().getTime() - 
			    foundLocalUser->getLastAwake()).seconds,
			   foundUser->getSignonTime().seconds,
			   GETLANG(irc2_RPL_WHOISIDLE));
	    }
	 }
	 
	 // If the client is connected via a secure connection, say what type
	 if (false) {
	    sendNumeric(user, LibIRC2::Numerics::RPL_WHOISSECURE,
			delocaliseStr(foundClient->getNickname()),
			"SSL",
			GETLANG(irc2_RPL_WHOISSECURE));
	 }
	 
	 // Continue to the next target
	 continue;
      }

      // If we got here, the client was unknown
      sendNumeric(user, LibIRC2::Numerics::ERR_NOSUCHNICK,
		  target,
		  GETLANG(irc2_ERR_NOSUCHNICK));
   }

   // Send the footer
   sendNumeric(user, LibIRC2::Numerics::RPL_ENDOFWHOIS,
	       targets,
	       GETLANG(irc2_RPL_ENDOFWHOIS));
}


/* doWHOWAS
 * Original 09/10/2001 simonb
 * Note: Totally imcomplete transfer ;)
 */
void Protocol::doWHOWAS(const User& user, const std::string& targets,
			int count)
{
   sendNumeric(user, LibIRC2::Numerics::RPL_WHOWASUSER,
	       "nickname",
	       "username",
	       "hostname",
	       replacementCharacter,
	       "real name field");
   sendNumeric(user, LibIRC2::Numerics::RPL_WHOISSERVER,
	       "nickname",
	       "servername",
	       "12345678"); // signoff time
   sendNumeric(user, LibIRC2::Numerics::RPL_WHOWASDETAILS,
	       "nickname",
	       "1",
	       "something");
   
   sendNumeric(user, LibIRC2::Numerics::ERR_WASNOSUCHNICK,
	       "nickname",
	       GETLANG(irc2_ERR_WASNOSUCHNICK));
   
   // Send end of whowas list
   sendNumeric(user, LibIRC2::Numerics::RPL_ENDOFWHOWAS,
	       targets,
	       GETLANG(irc2_RPL_ENDOFWHOWAS));
}
