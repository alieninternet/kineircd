/* $Id$
 * 'Config' class definition tables and functions
 *
 * Copyright (c) 2002 KineIRCd Development Team
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

#include "kineircd/kineircdconf.h"

#include <cctype>

#include "kineircd/config.h"
#include "kineircd/moduledescriptor.h"
#include "config/defaults.h"
#include "debug.h"

using namespace Kine;


namespace Config {

   // Top definitions
   const Kine::ConfigParser::defTable_type
     Kine::Config::topDefs = {
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
#ifdef KINE_OLD_CONFIG_SUBPARSER
	{
	   "INCLUDEOLDCONFIG",
	     0, // Intentionally null
	     &varHandleOldConfig,
	     0,
	     0
	},
#endif
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
	   "MODULE",
	     (void *)&Config::defModuleList,
	     &varHandleModule,
	     0,
	     &classHandleModule
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
#ifdef KINE_WITH_SSL
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
   const Kine::ConfigParser::defTable_type
     Kine::Config::defClassAdministrator = {
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
   const Kine::ConfigParser::defTable_type
     Kine::Config::defClassArbiters = {
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
   const Kine::ConfigParser::defTable_type
     Kine::Config::defClassArbitersServers = {
	{
	   0,
	     0,
	     0,
	     0,
	     0
	}
   };


   // "LISTEN" class
   const Kine::ConfigParser::defTable_type
     Kine::Config::defClassListen = {
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
   const Kine::ConfigParser::defTable_type
     Kine::Config::defClassLogging = {
	{
	   0,
	     0,
	     0,
	     0,
	     0
	}
   };


   // "NETWORK" class
   const Kine::ConfigParser::defTable_type
     Kine::Config::defClassNetwork = {
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
   const Kine::ConfigParser::defTable_type
     Kine::Config::defClassOperators = {
	{
	   0,
	     0,
	     0,
	     &defClassOperatorsGeneric,
	     0
	}
   };

   // "OPERATORS.*" generic class
   const Kine::ConfigParser::defTable_type
     Kine::Config::defClassOperatorsGeneric = {
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
   const Kine::ConfigParser::defTable_type
     Kine::Config::defClassOperatorsGenericHosts = {
	{
	   0,
	     0,
	     0,
	     0,
	     0
	}
   };


   // "OPTIONS" class
   const Kine::ConfigParser::defTable_type
     Kine::Config::defClassOptions = {
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
   const Kine::ConfigParser::defTable_type
     Kine::Config::defClassOptionsLimits = {
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
   const Kine::ConfigParser::defTable_type
     Kine::Config::defClassOptionsLimitsChannels = {
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
   const Kine::ConfigParser::defTable_type
     Kine::Config::defClassOptionsLimitsUsers = {
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
   const Kine::ConfigParser::defTable_type
     Kine::Config::defClassRedirection = {
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
   const Kine::ConfigParser::defTable_type
     Kine::Config::defClassRedirectionChannels = {
	{
	   0,
	     0,
	     0,
	     0,
	     0
	}
   };


#ifdef KINE_WITH_SSL
   // "SSL" class
   const Kine::ConfigParser::defTable_type
     Kine::Config::defClassSSL = {
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
Kine::Config::Config(const String &f)
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
  
#ifdef KINE_WITH_SSL
  , // <=- here to continue the list properly
    // "SSL" class
    defSSLContext(0)
#endif
{
   defLoggingLog = new Log();  // temporary

#ifdef KINE_WITH_SSL
   // Fire up the SSL component
   SSL_load_error_strings();
   
   if (SSL_library_init()) {
      //
      // SEED PRNG HERE IF THE OPENSSL LIBRARY WONT!!! :( :( :(
      //
      
      /*
       * Fire up the context using SSLv2 and SSLv3 methods.
       * I think this should be configurable, but at this point I don't care!
       * :(
       */
      defSSLContext = SSL_CTX_new(SSLv23_method());
   }
#endif
};


/* ~Config - Configuration class destructor
 * Original 17/06/2002 simonb
 */
Kine::Config::~Config(void)
{
#ifdef KINE_WITH_SSL
   // Clean up the SSL component
   if (defSSLContext != 0) {
      SSL_CTX_free(defSSLContext);
   }
   
   EVP_cleanup();
#endif
}


/* classHandleModule - Get a module's filename and attempt to load & configure
 * Original 21/07/2002 simonb
 */
CONFIG_CLASS_HANDLER(Kine::Config::classHandleModule)
{
#ifdef KINE_DEBUG_ASSERT
   assert(dataVariable != 0);
#endif
   
   // Check if the first value is empty (the filename field)
   if (values.front().empty()) {
      // Get cranky
      errString = "No module filename supplied!";
      return false;
   }

   // Attempt to open the module
   ModuleDescriptor *moduleDesc =
     ((dataClass.*((ModuleList ConfigData::*)dataVariable)).
      loadModule(values.front().trim(), errString));
   
   // Make sure the module was loaded
   if (moduleDesc == 0) {
      return false;
   }
   
   // Does the module have configuration data?
   if (moduleDesc->getModule()->getBasicInfo().configDefinitions != 0) {
      // Parse the module's configuration data
      return 
	ConfigParser::parse(configData, position,
			    (void *)(moduleDesc->getModule()->getBasicInfo().
				     configDefinitions),
			    *moduleDesc->getModule()->getConfigData());
   }
   
   // Just be happy, the module loaded!
   return true;
}


/* varHandleModule - Get a module's filename and attempt to load
 * Original 21/07/2002 simonb
 */
CONFIG_VARIABLE_HANDLER(Kine::Config::varHandleModule)
{
#ifdef KINE_DEBUG_ASSERT
   assert(dataVariable != 0);
#endif
   
   // Check if the first value is empty (the filename field)
   if (values.front().empty()) {
      // Get cranky
      errString = "No module filename supplied!";
      return false;
   }

   // Attempt to open the module
   return ((dataClass.*((ModuleList ConfigData::*)dataVariable)).
     loadModule(values.front().trim(), errString) != 0);
}


/* varHandleNetworkName - Read a network name (front value) and check it
 * Original 13/04/2002 simonb
 */
CONFIG_VARIABLE_HANDLER(Kine::Config::varHandleNetworkName)
{
#ifdef KINE_DEBUG_ASSERT
   assert(dataVariable != 0);
#endif

   // Check if the first value is empty
   if (values.front().empty()) {
      // Clear it (turn it off)
      (dataClass.*((String ConfigData::*)dataVariable)).clear();
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
      
#ifdef KINE_DEBUG_PSYCHO
   debug("Config::varHandleNetworkName() -> '" + 
	 dataClass.*((String ConfigData::*)dataVariable) + '\'');
#endif

   return true;
}
