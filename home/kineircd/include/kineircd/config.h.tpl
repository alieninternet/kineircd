[+ AutoGen5 template h +]
/* $Id$
 * Configuration file stuff
 *
 * Copyright (c) 2002,2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2002,2003 KineIRCd Development Team 
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
 *
 *
[+(dne " * ")+]
 */
[+DEFINE output-variable-name+][+IF .variable+][+variable+][+ELSE+][+(getPrefix)+][+name+][+ENDIF+][+ENDDEF+]
#ifndef _INCLUDE_KINEIRCD_CONFIG_H_
# define _INCLUDE_KINEIRCD_CONFIG_H_ 1

# include <kineircd/kineircdconf.h>
 
# include <aisutil/config/data.h>
# include <aisutil/string/string.h>
 
# include <kineircd/listenerlist.h>
# include <kineircd/modulelist.h>
 
# ifdef KINE_WITH_SSL
extern "C" {
#  include <openssl/ssl.h>
}
# endif

namespace Kine {
   // Config class
   class Config : public AISutil::ConfigData {
    public:
      /* Operator passwords are stored as a SHA1 message digest, however I
       * don't know anyone who can type in a 160-bit sequence perfectly on
       * their keyboard without considerable effort :) These definitions define
       * how the password is stored in the configuration files. DO NOT CHANGE
       * THIS!!! People will be furious with you, if you do. Yes: Very angry.
       * This has been designed so that each 160-bit digest hash is exactly
       * 25 characters long (not too long IMHO). This is done by using
       * "base-85", which generates a maximum of 5 characters per unsigned
       * long. This length per unsigned long is why we set the pas to 5
       * characters.
       */
      static const unsigned char operPasswordStrBase = 85;
      static const AISutil::String::size_type operPasswordStrBaseLongPad = 5;
      
    private:
      //! The config file to use..
      std::string configFile;
      
      //! Are we configuring?
      bool configuring;
[+DEFINE output-variables+][+FOR definition+][+
 IF (or
       (exist? ".definition")
       (shadowTable?))+][+IF (exist? ".condition")+]
#ifdef [+condition+][+ENDIF+][+(pushPrefix (get "name"))+]

      // '[+(getPrefix)+]' table...
      static const AISutil::ConfigParser::defTable_type defClass[+(getPrefix)+];[+IF (not (shadowTable?))+][+output-variables+][+ELSE+][+
 IF (not (and 
            ;; more checks than this??
            (exist? ".hasVariable")))+]
[+ENDIF+][+ENDIF+][+(popPrefix)+][+IF (exist? ".condition")+]
#endif[+ENDIF+][+ENDIF+][+IF (exist? ".hasVariable")+][+IF (exist? ".varType")+][+IF (exist? ".condition")+]
#ifdef [+condition+][+ENDIF+]
      [+varType+] def[+output-variable-name+];[+IF (exist? ".condition")+]
#endif[+ENDIF+][+ENDIF+][+ENDIF+][+IF (and
                                         (exist? "defaultDefinition.varType")
					 (exist? "defaultDefinition.variable"))+]
      [+defaultDefinition.varType+] def[+defaultdefinition.variable+];[+ENDIF+][+ENDFOR+][+ENDDEF+]
      // Our top-level definition table, defining compiled in top level classes
      static const AISutil::ConfigParser::defTable_type topDefs;[+output-variables+]
	
      // Additional handlers specific to data in this class
      static LIBAISUTIL_CONFIG_CLASS_HANDLER(classHandleModule);
      static LIBAISUTIL_CONFIG_VARIABLE_HANDLER(varHandleModule);
      static LIBAISUTIL_CONFIG_VARIABLE_HANDLER(varHandleNetworkName);
# ifdef KINE_OLD_CONFIG_SUBPARSER
      static LIBAISUTIL_CONFIG_VARIABLE_HANDLER(varHandleOldConfig);
# endif
      
      // Our single instance
      static Config* instance;
      
      //! Constructor
      Config(void);
      
    public:
      //! Destructor
      ~Config(void);
      
      //! Get the single instance of this class
      static Config& getInstance(void)
	{
	   if (instance == 0) {
	      instance = new Config();
	   }
	   return *instance;
	};
      
      //! Grab the configuration file name
      const std::string& getConfigFile(void) const
	{ return configFile; };

      //! Are we in the middle of configuration?
      const bool isConfiguring(void) const
        { return configuring; };

      //! Load/parse the config (from the parser routines)
      bool configure(void);
      bool configure(const std::string& file)
	{
	   configFile = file;
	   return configure();
	};
[+DEFINE output-methods+][+FOR definition+][+IF (exist? ".definition")+][+IF (exist? ".condition")+]
#ifdef [+condition+][+ENDIF+][+(pushPrefix (get "name"))+][+output-methods+][+(popPrefix)+][+IF (exist? ".condition")+]
#endif[+ENDIF+][+ENDIF+][+IF (exist? ".hasVariable")+][+IF (exist? ".varType")+][+IF (exist? ".condition")+]
#ifdef [+condition+][+ENDIF+]
      [+IF .varTypeProtected+][+varTypeProtected+][+ELSE+]const [+varType+]&[+ENDIF+] get[+output-variable-name+](void)[+IF (not (exist? "varPublicModify"))+] const[+ENDIF+]
         { return def[+output-variable-name+]; };[+IF (exist? ".condition")+]
#endif[+ENDIF+][+ENDIF+][+ENDIF+][+IF (and
                                         (exist? "defaultDefinition.varType")
					 (exist? "defaultDefinition.variable"))+]
      [+IF defaultDefinition.varTypeProtected+][+defaultDefinition.varTypeProtected+][+ELSE+]const [+defaultDefinition.varType+]&[+ENDIF+] get[+defaultDefinition.variable+](void)[+IF (not (exist? "defaultDefinition.varPublicModify"))+] const[+ENDIF+]
         { return def[+defaultDefinition.variable+]; };[+ENDIF+][+ENDFOR+][+ENDDEF+][+output-methods+]
   }; // class Config


   //! Return the current instance of the Config:: class
   inline static Config& config(void)
     { return Config::getInstance(); };
}; // namespace Kine
   
#endif // _INCLUDE_KINEIRCD_CONFIG_H_

