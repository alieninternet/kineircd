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

#ifdef KINE_DEBUG_ASSERT
# include <cassert>
#endif
#include <aisutil/string/string.h>
#include <kineircd/config.h>
#include <kineircd/languages.h>

#include "mod_irc2user/isupport.h"
#include "mod_irc2user/commands.h"

using namespace Kine::mod_irc2user;
using AISutil::String;


// The pointer to our single instance
ISupport* ISupport::instance = 0;


// Our "pre-init" table, which holds ISUPPORT information we always know about
const char* const ISupport::preInitInfo[] = {
   "STD=i-d",				// RPL_ISUPPORT standard
   "FNC",				// Forced nickname changes
   "INVEX",				// Channel invite exception lists
   "EXCEPTS",				// Channel ban exception lists
   "PENALTY",				// Commands have different penalties
   "IIRC",				// KineIRCd supports IIRC connections
   "CHARSET=UTF-8",			// We output using UTF-8 encoding
   "CASEMAPPING=rfc1459",
   "PREFIX=(.ohv).@%+",
   "CHANTYPES=&#!+.~",
   "CHIDLEN=5",
#if (defined(HAVE_MOD_IRC2USER_HAVE_CMD_PRIVMSG) || \
     defined(HAVE_MOD_IRC2USER_HAVE_CMD_NOTICE))
   "STATUSMSG=+%@",
#endif
#ifdef KINE_MOD_IRC2USER_HAVE_CMD_MODE
   "CHANMODES=beI,k,l,imnpsta",
#endif
#ifdef KINE_MOD_IRC2USER_HAVE_CMD_WALLCHOPS
   "WALLCHOPS",
#endif
#ifdef KINE_MOD_IRC2USER_HAVE_CMD_USERIP
   "USERIP",
#endif
#ifdef KINE_MOD_IRC2USER_HAVE_CMD_CPRIVMSG
   "CPRIVMSG",
#endif
#ifdef KINE_MOD_IRC2USER_HAVE_CMD_CNOTICE
   "CNOTICE",
#endif
#ifdef KINE_MOD_IRC2USER_HAVE_CMD_KNOCK
   "KNOCK",
#endif
#ifdef KINE_MOD_IRC2USER_HAVE_CMD_WHO
   "WHOX",
#endif
   0
};


/* initInstance - Create the single instance of this class
 * Original 01/04/2003 simonb
 */
void ISupport::initInstance(void)
{
#ifdef KINE_DEBUG_ASSERT
   // Make sure we don't already exist
   assert(instance == 0);
#endif
   
   // Okay, create ourselves then
   instance = new ISupport();
}


/* initInfo - 
 * Original 24/08/2001 simonb
 */
void ISupport::initInfo(void)
{
   // Clear the ISUPPORT vector
   info.clear();
   
   // Do the easy bit - copy the hard-coded stuff into the vector
   for (unsigned int i = 0; preInitInfo[i] != 0; ++i) {
      info.push_back(preInitInfo[i]);
   }
   
   // Tack on information about our configuration
#ifdef KINE_MOD_IRC2USER_HAVE_CMD_NICK
   info.push_back("NICKLEN=" +
		  String::convert((int)config().
				  getLimitsUsersMaxNickNameLength()));
#endif
#ifdef KINE_MOD_IRC2USER_HAVE_CMD_TOPIC
   info.push_back("TOPICLEN=" +
		  String::convert(config().getLimitsChannelsMaxTopicLength()));
#endif
#ifdef KINE_MOD_IRC2USER_HAVE_CMD_KICK
   info.push_back("KICKLEN=" +
		  String::convert(config().getLimitsMaxKickReasonLength()));
#endif
#ifdef KINE_MOD_IRC2USER_HAVE_CMD_JOIN
   info.push_back("CHANNELLEN=" +
		  String::convert(config().getLimitsChannelsMaxNameLength()));
   info.push_back("MAXCHANNELS=" +
		  String::convert(config().getLimitsUsersMaxChannels()));
#endif
#if (defined(HAVE_MOD_IRC2USER_HAVE_CMD_PRIVMSG) || \
     defined(HAVE_MOD_IRC2USER_HAVE_CMD_NOTICE))
   info.push_back("MAXTARGETS=" +
		  String::convert((int)config().getLimitsMaxTargets()));
#endif
#ifdef KINE_MOD_IRC2USER_HAVE_CMD_SILENCE
   info.push_back("SILENCE=" +
		  String::convert(config().getLimitsUsersMaxSilences()));
#endif
#ifdef KINE_MOD_IRC2USER_HAVE_CMD_ACCEPT
   info.push_back("CALLERID=" +
		  String::convert(config().getLimitsUsersMaxAccepts()));
#endif
#ifdef KINE_MOD_IRC2USER_HAVE_CMD_WATCH
   info.push_back("WATCH=" +
		  String::convert(config().getLimitsUsersMaxWatches()));
#endif
#ifdef KINE_MOD_IRC2USER_HAVE_CMD_MODE
   info.push_back("MAXBANS=" +
		  String::convert(config().getLimitsChannelsMaxBans()));
   info.push_back("MODES=" +
		  String::convert((int)0)); // <=- fix this
#endif

   // If there's a network name, then append that too
   if (!config().getNetworkName().empty()) {
      info.push_back("NETWORK=" + config().getNetworkName());
   }

#ifdef KINE_MOD_IRC2USER_HAVE_CMD_LANGUAGE
   // Start assembling the language information
   std::ostringstream languageInfo;
   languageInfo <<
     "LANGUAGE=" << (int)config().getLimitsUsersMaxLanguages();

   for (Languages::languageDataMap_type::const_iterator it = 
	languages().getLanguageDataMap().begin();
	it != languages().getLanguageDataMap().end();
	++it) {
      // Add the comma delimeter
      languageInfo << ',';
      
      // Is this language the default one?
      if (it->second == languages().getDefaultLanguage()) {
	 languageInfo << '*';
      }
      
      // Add on the language code
      languageInfo << it->second->getLanguageCode();
   }

   // Add it to the list
   info.push_back(languageInfo.str());
#endif // KINE_MOD_IRC2USER_HAVE_CMD_LANGUAGE
}
