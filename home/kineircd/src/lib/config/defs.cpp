/* defs.cpp
 * Configuration file class/variable definitions
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

#include "config.h"


namespace Config {

   // Top definitions
   const ConfigParser::defTable_type Config::topDefs = {
	{
	   "ADMINISTRATOR",		&defClassAdministrator,
	     0,				0
	},
	{
	   "ARBITERS",			&defClassArbiters,
	     0,				0
	},
	{
	   "LISTEN",			&defClassListen,
	     0,				0
	},
	{
	   "LOGGING",			&defClassLogging,
	     0,				0
	},
	{
	   "NETWORK",			&defClassNetwork,
	     0,				0
	},
	{
	   "OPERATORS",			&defClassOperators,
	     0,				0
	},
	{
	   "OPTIONS",			&defClassOptions,
	     0,				0
	},
	{
	   "REDIRECTION",		&defClassRedirection,
	     0,				0
	},
#ifdef WITH_SSL
	{
	   "SSL",			&defClassSSL,
	     0,				0
	},
#endif
	{
	   0,				0,
	     0,				0
	}
   };


   // "ADMINISTRATOR" class
   const ConfigParser::defTable_type Config::defClassAdministrator = {
	{
	   "EMAIL",			0,
	     0,				0
	},
	{
	   "LOCATION",			0,
	     0,				0
	},
	{
	   "NAME",			0,
	     0,				0
	},
	{
	   0,				0,
	     0,				0
	}
   };


   // "ARBITERS" class
   const ConfigParser::defTable_type Config::defClassArbiters = {
	{
	   "SERVERS",			&defClassArbitersServers,
	     0,				0
	},
	{
	   0,				0,
	     0,				0
	}
   };

   // "ARBITERS.SERVERS" class
   const ConfigParser::defTable_type Config::defClassArbitersServers = {
	{
	   0,				0,
	     0,				0
	}
   };


   // "LISTEN" class
   const ConfigParser::defTable_type Config::defClassListen = {
	{
	   "ALLOWSERVERS",		0,
	     0,				0
	},
	{
	   "ALLOWSERVICES",		0,
	     0,				0
	},
	{
	   "ALLOWUSERS",		0,
	     0,				0
	},
	{
	   "PORT",			0,
	     0,				0
	},
	{
	   "SECURE",			0,
	     0,				0
	},
	{
	   0,				0,
	     0,				0
	}
   };

   
   // "LOGGING" class
   const ConfigParser::defTable_type Config::defClassLogging = {
	{
	   0,				0,
	     0,				0
	}
   };
   

   // "NETWORK" class
   const ConfigParser::defTable_type Config::defClassNetwork = {
	{
	   0,				0,
	     &defVarNONE,		0
	}
   };


   // "OPERATORS" class
   const ConfigParser::defTable_type Config::defClassOperators = {
	{
	   0,				&defClassOperatorsGeneric,
	     0,				0
	}
   };

   // "OPERATORS.*" generic class
   const ConfigParser::defTable_type Config::defClassOperatorsGeneric = {
	{
	   "HOSTS",			&defClassOperatorsGenericHosts,
	     0,				0
	},
	{
	   "NAME",			0,
	     0,				0
	},
	{
	   "PASSWORD",			0,
	     0,				0
	},
	{
	   "TYPE",			0,
	     0,				0
	},
	{
	   0,				0,
	     0,				0
	}
   };

   // "OPERATORS.*.HOSTS" generic class
   const ConfigParser::defTable_type Config::defClassOperatorsGenericHosts = {
	{
	   0,				0,
	     0,				0
	}
   };


   // "OPTIONS" class
   const ConfigParser::defTable_type Config::defClassOptions = {
	{
	   "AUTOBONESWITCH",		0,
	     0,				0
	},
	{
	   "DESCRIPTION",		0,
	     0,				0
	},
	{
	   "CHGROUP",			0,
	     0,				0
	},
	{
	   "CHROOT",			0,
	     0,				0
	},
	{
	   "CHUSER",			0,
	     0,				0
	},
	{
	   "HIDDEN",			0,
	     0,				0
	},
	{
	   "LANGUAGEDIR",		0,
	     0,				0
	},
	{
	   "MOTDFILE",			0,
	     0,				0
	},
	{
	   "NETWORK",			0,
	     0,				0
	},
	{
	   "SERVERNAME",		0,
	     0,				0
	},
	{
	   0,				0,
	     0,				0
	}
   };


   // "REDIRECTION" class
   const ConfigParser::defTable_type Config::defClassRedirection = {
	{
	   "CHANNELS",			&defClassRedirectionChannels,
	     0,				0
	},
	{
	   0,				0,
	     0,				0
	}
   };

   // "REDIRECTION.CHANNELS" class
   const ConfigParser::defTable_type Config::defClassRedirectionChannels = {
	{
	   0,				0,
	     &defVarNONE,		0
	}
   };


#ifdef WITH_SSL
   // "SSL" class
   const ConfigParser::defTable_type Config::defClassSSL = {
	{
	   "CERTIFICATE",		0,
	     0,				0
	},
	{
	   0,				0,
	     0,				0
	}
   };
#endif

};
