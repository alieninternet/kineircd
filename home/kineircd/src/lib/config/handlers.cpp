/* class.cpp
 * 'Config' class definition tables and functions
 *
 * Copyright (c) 2002 AustHex Development Team
 * (See DEV-TEAM file for details)
 *
 * This file is a part of AustHex.
 *
 * AustHex is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * AustHex is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with AustHex; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "autoconf.h"

#include <cctype>

#include "config.h"
#include "configdefault.h"
#include "handlerhelp.h"
#include "debug.h"


namespace Config {

   // Top definitions
   const ConfigParser::defTable_type Config::topDefs = {
	{
	   "ADMINISTRATOR",
	     0,
	     0,
	     &defClassAdministrator,
	     0
	},
	{
	   "ARBITERS",
	     0,
	     0,
	     &defClassArbiters,
	     0
	},
	{
	   "LISTEN",
	     0,
	     0,
	     &defClassListen,
	     0
	},
	{
	   "LOGGING",
	     0,
	     0,
	     &defClassLogging,
	     0
	},
	{
	   "NETWORK",
	     0,
	     0,
	     &defClassNetwork,
	     0
	},
	{
	   "OPERATORS",
	     0,
	     0,
	     &defClassOperators,
	     0
	},
	{
	   "OPTIONS",
	     0,
	     0,
	     &defClassOptions,
	     0
	},
	{
	   "REDIRECTION",
	     0,
	     0,
	     &defClassRedirection,
	     0
	},
#ifdef WITH_SSL
	{
	   "SSL",
	     0,
	     0,
	     &defClassSSL,
	     0
	},
#endif
	{
	   0,
	     0,
	     0,
	     0,
	     0
	}
   };


   // "ADMINISTRATOR" class
   const ConfigParser::defTable_type Config::defClassAdministrator = {
	{
	   "EMAIL",
	     (void *)&Config::defAdminEmail,
	     &varHandleString,
	     0,
	     0
	},
	{
	   "LOCATION",
	     (void *)&Config::defAdminLocation,
	     &varHandleString,
	     0,
	     0
	},
	{
	   "NAME",
	     (void *)&Config::defAdminName,
	     &varHandleString,
	     0,
	     0
	},
	{
	   0,
	     0,
	     0,
	     0,
	     0
	}
   };


   // "ARBITERS" class
   const ConfigParser::defTable_type Config::defClassArbiters = {
	{
	   "SERVERS",
	     0,
	     0,
	     &defClassArbitersServers,
	     0
	},
	{
	   0,
	     0,
	     0,
	     0,
	     0
	}
   };

   // "ARBITERS.SERVERS" class
   const ConfigParser::defTable_type Config::defClassArbitersServers = {
	{
	   0,
	     0,
	     0,
	     0,
	     0
	}
   };


   // "LISTEN" class
   const ConfigParser::defTable_type Config::defClassListen = {
	{
	   "ALLOWSERVERS",
	     0,
	     0,
	     0,
	     0
	},
	{
	   "ALLOWSERVICES",
	     0,
	     0,
	     0,
	     0
	},
	{
	   "ALLOWUSERS",
	     0,
	     0,
	     0,
	     0
	},
	{
	   "PORT",
	     0,
	     0,
	     0,
	     0
	},
	{
	   "SECURE",
	     0,
	     0,
	     0,
	     0
	},
	{
	   0,
	     0,
	     0,
	     0,
	     0
	}
   };


   // "LOGGING" class
   const ConfigParser::defTable_type Config::defClassLogging = {
	{
	   0,
	     0,
	     0,
	     0,
	     0
	}
   };


   // "NETWORK" class
   const ConfigParser::defTable_type Config::defClassNetwork = {
	{
	   "NAME",
	     (void *)&Config::defNetworkName,
	     &varHandleNetworkName,
	     0,
	     0
	},
	{
	   0,
	     0,
	     0,
	     0,
	     0
	}
   };


   // "OPERATORS" class
   const ConfigParser::defTable_type Config::defClassOperators = {
	{
	   0,
	     0,
	     0,
	     &defClassOperatorsGeneric,
	     0
	}
   };

   // "OPERATORS.*" generic class
   const ConfigParser::defTable_type Config::defClassOperatorsGeneric = {
	{
	   "HOSTS",
	     0,
	     0,
	     &defClassOperatorsGenericHosts,
	     0
	},
	{
	   "NAME",
	     0,
	     0,
	     0,
	     0
	},
	{
	   "PASSWORD",
	     0,
	     0,
	     0,
	     0
	},
	{
	   "TYPE",
	     0,
	     0,
	     0,
	     0
	},
	{
	   0,
	     0,
	     0,
	     0,
	     0
	}
   };

   // "OPERATORS.*.HOSTS" generic class
   const ConfigParser::defTable_type Config::defClassOperatorsGenericHosts = {
	{
	   0,
	     0,
	     0,
	     0,
	     0
	}
   };


   // "OPTIONS" class
   const ConfigParser::defTable_type Config::defClassOptions = {
	{
	   "DESCRIPTION",
	     (void *)&Config::defOptionsDescription,
	     &varHandleString,
	     0,
	     0
	},
	{
	   "CHGROUP",
	     0,
	     0,
	     0,
	     0
	},
	{
	   "CHROOT",
	     0,
	     0,
	     0,
	     0
	},
	{
	   "CHUSER",
	     0,
	     0,
	     0,
	     0
	},
	{
	   "HIDDEN",
	     (void *)&Config::defOptionsHidden,
	     &varHandleBoolean,
	     0,
	     0
	},
	{
	   "LANGUAGEDIR",
	     0,
	     0,
	     0,
	     0
	},
	{
	   "LIMITS",
	     0,
	     0,
	     &defClassOptionsLimits,
	     0
	},
	{
	   "MOTDFILE",
	     (void *)&Config::defOptionsMOTD,
	     0,
	     0,
	     0
	},
	{
	   "SERVERNAME",
	     (void *)&Config::defOptionsServerName,
	     &varHandleHostName,
	     0,
	     0
	},
	{
	   0,
	     0,
	     0,
	     0,
	     0
	}
   };

   // "OPTIONS.LIMITS" class
   const ConfigParser::defTable_type Config::defClassOptionsLimits = {
	{
	   "CHANNELS",
	     0,
	     0,
	     &defClassOptionsLimitsChannels,
	     0
	},
	{
	   "MAXKICKREASONLENGTH",
	     (void *)&Config::defOptionsLimitsMaxKickReasonLength,
	     &varHandleUnsignedShortNoZero,
	     0,
	     0
	},
	{
	   "MAXTARGETS",
	     (void *)&Config::defOptionsLimitsMaxTargets,
	     &varHandleUnsignedCharNoZero,
	     0,
	     0
	},
	{
	   "MINBROADCASTLENGTH",
	     (void *)&Config::defOptionsLimitsMinBroadcastLength,
	     &varHandleUnsignedShort,
	     0,
	     0
	},
	{
	   "USERS",
	     0,
	     0,
	     &defClassOptionsLimitsUsers,
	     0
	},
	{
	   0,
	     0,
	     0,
	     0,
	     0
	}
   };

   // "OPTIONS.LIMITS.CHANNELS" class
   const ConfigParser::defTable_type Config::defClassOptionsLimitsChannels = {
	{
	   "MAXBANS",
	     (void *)&Config::defOptionsLimitsChannelsMaxBans,
	     &varHandleUnsignedLongNoZero,
	     0,
	     0
	},
	{
	   "MAXBANEXCEPTIONS",
	     (void *)&Config::defOptionsLimitsChannelsMaxBanExceptions,
	     &varHandleUnsignedLongNoZero,
	     0,
	     0
	},
	{
	   "MAXINVITEEXCEPTIONS",
	     (void *)&Config::defOptionsLimitsChannelsMaxInviteExceptions,
	     &varHandleUnsignedLongNoZero,
	     0,
	     0
	},
	{
	   "MAXNAMELENGTH",
	     (void *)&Config::defOptionsLimitsChannelsMaxNameLength,
	     &varHandleUnsignedShortNoZero,
	     0,
	     0
	},
	{
	   "MAXTOPICLENGTH",
	     (void *)&Config::defOptionsLimitsChannelsMaxTopicLength,
	     &varHandleUnsignedShortNoZero,
	     0,
	     0
	},
	{
	   0,
	     0,
	     0,
	     0,
	     0
	}
   };

   // "OPTIONS.LIMITS.USERS" class
   const ConfigParser::defTable_type Config::defClassOptionsLimitsUsers = {
	{
	   "MAXACCEPTS",
	     (void *)&Config::defOptionsLimitsUsersMaxAccepts,
	     &varHandleUnsignedLongNoZero,
	     0,
	     0
	},
	{
	   "MAXCHANNELS",
	     (void *)&Config::defOptionsLimitsUsersMaxChannels,
	     &varHandleUnsignedLongNoZero,
	     0,
	     0
	},
	{
	   "MAXLANGUAGES",
	     (void *)&Config::defOptionsLimitsUsersMaxLanguages,
	     &varHandleUnsignedCharNoZero,
	     0,
	     0
	},
	{
	   "MAXNICKNAMELENGTH",
	     (void *)&Config::defOptionsLimitsUsersMaxNickNameLength,
	     &varHandleUnsignedCharNoZero,
	     0,
	     0
	},
	{
	   "MAXREALNAMELENGTH",
	     (void *)&Config::defOptionsLimitsUsersMaxRealNameLength,
	     &varHandleUnsignedCharNoZero,
	     0,
	     0
	},
	{
	   "MAXSILENCES",
	     (void *)&Config::defOptionsLimitsUsersMaxSilences,
	     &varHandleUnsignedLongNoZero,
	     0,
	     0
	},
	{
	   "MAXWATCHES",
	     (void *)&Config::defOptionsLimitsUsersMaxWatches,
	     &varHandleUnsignedLongNoZero,
	     0,
	     0
	},
	{
	   0,
	     0,
	     0,
	     0,
	     0
	}
   };


   // "REDIRECTION" class
   const ConfigParser::defTable_type Config::defClassRedirection = {
	{
	   "CHANNELS",
	     0,
	     0,
	     &defClassRedirectionChannels,
	     0
	},
	{
	   0,
	     0,
	     0,
	     0,
	     0
	}
   };

   // "REDIRECTION.CHANNELS" class
   const ConfigParser::defTable_type Config::defClassRedirectionChannels = {
	{
	   0,
	     0,
	     0,
	     0,
	     0
	}
   };


#ifdef WITH_SSL
   // "SSL" class
   const ConfigParser::defTable_type Config::defClassSSL = {
	{
	   "CERTIFICATE",
	     0,
	     0,
	     0,
	     0
	},
	{
	   0,
	     0,
	     0,
	     0,
	     0
	}
   };
#endif
};


/* Config - Configuration class constructor (mainly to load defaults if any)
 * Original 11/04/2002 simonb
 */
Config::Config(const String &f)
  : parser(*this, &Config::topDefs, f),

    // "ADMIN" class
    defAdminEmail(DEFAULT_ADMIN_EMAIL),
    defAdminLocation(DEFAULT_ADMIN_LOCATION),
    defAdminName(DEFAULT_ADMIN_NAME),

    // "LOGGING" class
    defLoggingLog(0),

    // "NETWORK" class
    defNetworkName(DEFAULT_NETWORK_NAME),

    // "OPTIONS" class
    defOptionsDescription(DEFAULT_OPTIONS_DESCRIPTION),
    defOptionsHidden(DEFAULT_OPTIONS_HIDDEN),
    defOptionsServerName(DEFAULT_OPTIONS_SERVER_NAME),

    // "OPTIONS.LIMITS" class
    defOptionsLimitsMaxKickReasonLength(DEFAULT_OPTIONS_LIMITS_MAX_KICK_REASON_LENGTH),
    defOptionsLimitsMaxTargets(DEFAULT_OPTIONS_LIMITS_MAX_TARGETS),
    defOptionsLimitsMinBroadcastLength(DEFAULT_OPTIONS_LIMITS_MIN_BROADCAST_LENGTH),

    // "OPTIONS.LIMITS.CHANNELS" class
    defOptionsLimitsChannelsMaxBans(DEFAULT_OPTIONS_LIMITS_CHANNELS_MAX_BANS),
    defOptionsLimitsChannelsMaxBanExceptions(DEFAULT_OPTIONS_LIMITS_CHANNELS_MAX_BAN_EXCEPTIONS),
    defOptionsLimitsChannelsMaxInviteExceptions(DEFAULT_OPTIONS_LIMITS_CHANNELS_MAX_INVITE_EXCEPTIONS),
    defOptionsLimitsChannelsMaxNameLength(DEFAULT_OPTIONS_LIMITS_CHANNELS_MAX_NAME_LENGTH),
    defOptionsLimitsChannelsMaxTopicLength(DEFAULT_OPTIONS_LIMITS_CHANNELS_MAX_TOPIC_LENGTH),

    // "OPTIONS.LIMITS.USERS" class
    defOptionsLimitsUsersMaxAccepts(DEFAULT_OPTIONS_LIMITS_USERS_MAX_ACCEPTS),
    defOptionsLimitsUsersMaxChannels(DEFAULT_OPTIONS_LIMITS_USERS_MAX_CHANNELS),
    defOptionsLimitsUsersMaxLanguages(DEFAULT_OPTIONS_LIMITS_USERS_MAX_LANGUAGES),
    defOptionsLimitsUsersMaxNickNameLength(DEFAULT_OPTIONS_LIMITS_USERS_MAX_NICK_NAME_LENGTH),
    defOptionsLimitsUsersMaxRealNameLength(DEFAULT_OPTIONS_LIMITS_USERS_MAX_REAL_NAME_LENGTH),
    defOptionsLimitsUsersMaxSilences(DEFAULT_OPTIONS_LIMITS_USERS_MAX_SILENCES),
    defOptionsLimitsUsersMaxWatches(DEFAULT_OPTIONS_LIMITS_USERS_MAX_WATCHES)
{
   defLoggingLog = new Log();  // temporary
};


/* varHandleNetworkName - Read a network name (front value) and check it
 * Original 13/04/2002 simonb
 */
CONFIG_VARIABLE_HANDLER(Config::varHandleNetworkName)
{
#ifdef DEBUG_ASSERT
   assert(dataVariable != 0);
#endif

   // Check if the first value is empty
   if (values.front().empty()) {
      // Clear it (turn it off)
      dataClass.*((String ConfigData::*)dataVariable) = "";
      return true;
   }
   
   const char *name = values.front().c_str();
   
   for (; *name != '\0'; name++) {
      /* Check the network name - basically only look for nasties which will
       * break the ISUPPORT numeric for IRC2 clients *sigh*
       */
      if ((*name == ',') ||
	  (*name == '=') ||
	  isspace(*name) ||
	  iscntrl(*name)) {
	 // Construct an error string and be unhappy about this revelation
	 if (isprint(*name)) {
	    errString = String("Unfriendly character found ('") + *name + 
	      "\')";
	 } else {
	    errString = "Unfriendly character found (#" +
	      String::convert((int)*name) + ')';
	 }
	 return false;
      }
   }
   
   // Set it happily
   dataClass.*((String ConfigData::*)dataVariable) = values.front();
      
#ifdef DEBUG_PSYCHO
   debug("Config::varHandleNetworkName() -> '" + 
	 dataClass.*((String ConfigData::*)dataVariable) + '\'');
#endif

   return true;
}
