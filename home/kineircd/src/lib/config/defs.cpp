/* defs.cpp
 * Configuration file class/variable definitions
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
	     0,				&defVarNONE
	},
	{
	   "LOCATION",			0,
	     0,				&defVarNONE
	},
	{
	   "NAME",			0,
	     0,				&defVarNONE
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
	     0,				&defVarNONE
	}
   };

   
   // "LISTEN" class
   const ConfigParser::defTable_type Config::defClassListen = {
	{
	   "ALLOWSERVERS",		0,
	     0,				&defVarNONE
	},
	{
	   "ALLOWSERVICES",		0,
	     0,				&defVarNONE
	},
	{
	   "ALLOWUSERS",		0,
	     0,				&defVarNONE
	},
	{
	   "PORT",			0,
	     0,				&defVarNONE
	},
	{
	   "SECURE",			0,
	     0,				&defVarNONE
	},
	{ 
	   0,				0,
	     0,				0
	}
   };

   
   // "NETWORK" class
   const ConfigParser::defTable_type Config::defClassNetwork = {
	{ 
	   0,				0,
	     &defVarNONE,		0 // should be a class but shhhh!!
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
	     0,				&defVarNONE
	},
	{ 
	   "PASSWORD",			0,
	     0,				&defVarNONE
	},
	{ 
	   "TYPE",			0,
	     0,				&defVarNONE
	},
	{ 
	   0,				0,
	     0,				&defVarNONE
	}
   };

   // "OPERATORS.*.HOSTS" generic class
   const ConfigParser::defTable_type Config::defClassOperatorsGenericHosts = {
	{ 
	   0,				0,
	     0,				&defVarNONE
	}
   };

   
   // "OPTIONS" class
   const ConfigParser::defTable_type Config::defClassOptions = {
	{
	   "AUTOBONESWITCH",		0,
	     0,				&defVarNONE
	},
	{
	   "DESCRIPTION",		0,
	     0,				&defVarNONE
	},
	{
	   "HIDDEN",			0,
	     0,				&defVarNONE
	},
	{
	   "LANGUAGEDIR",		0,
	     0,				&defVarNONE
	},
	{
	   "MOTDFILE",			0,
	     0,				&defVarNONE
	},
	{
	   "NETWORK",			0,
	     0,				&defVarNONE
	},
	{
	   "SERVERNAME",		0,
	     0,				&defVarNONE
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
	     0,				&defVarNONE
	}
   };

   // "REDIRECTION.CHANNELS" class
   const ConfigParser::defTable_type Config::defClassRedirectionChannels = {
	{ 
	   0,				0,
	     0,				&defVarNONE
	}
   };

   
#ifdef WITH_SSL
   // "SSL" class
   const ConfigParser::defTable_type Config::defClassSSL = {
	{
	   "CERTIFICATE",		0,
	     0,				&defVarNONE
	},
	{ 
	   0,				0,
	     0,				0 
	}
   };
#endif
   
};
