/* $Id$
 * Support class for modules
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

#ifndef _INCLUDE_KINEIRCD_MODULE_H_
# define _INCLUDE_KINEIRCD_MODULE_H_ 1

// Forwarded declarations (completed after class)
namespace Kine {
   class Daemon;
};

# include "kineircd/configparser.h"
# include "kineircd/str.h"

// Template for the function which initialises the module and returns the info
# define KINE_MODULE_INIT_PROTOTYPE(x) \
   const Kine::Module* x(void)

// The correct name of the symbol which is searched for by dlopen()
# define KINE_MODULE_INIT_SYMBOL_NAME \
   "KineIRCdModuleInit"

// Template for the above, which you should use in your modules (correct name)
# define KINE_MODULE_INIT \
   extern "C" KINE_MODULE_INIT_PROTOTYPE(KineIRCdModuleInit)

// Template for the function which triggers a module to start
# define KINE_MODULE_START(x) \
   bool x(Kine::Daemon& daemon)

// Template for the function which triggers a module to stop
# define KINE_MODULE_STOP(x) \
   void x(void)

namespace Kine {
   class Module {
    public:
      // The functions all modules are required to have - start and stop
      typedef KINE_MODULE_START(startFunction_type);
      typedef KINE_MODULE_STOP(stopFunction_type);
      
      /* This structure defines information about the module itself. Each 
       * module must have one of these present to define parameters about the 
       * module so that the server has some idea of how to handle it, what
       * version it is and so on. This is only the basic information necessary
       * for the module, and hense every module needs this information.
       */
      struct basicInfo_type {
	 /* Name and version information of the module.
	  * Note that the full module version will be seen as follows:
	  * <nameShort>-<versionMajor>.<versionMinor>.<versionPatchLevel>
	  *  <versionExtra>
	  * Eg. FooMod-1.2.3b
	  */
	 const char* nameShort;
	 const char* nameLong;
	 const char* copyright;
	 const unsigned char versionMajor;
	 const unsigned char versionMinor;
	 const unsigned short versionPatchLevel;
	 const char* versionExtra;

	 // Flags to determine how this module needs to be configured
	 enum flags_type {
	    FLAG_NONE = 0,			      // No flags set
            FLAG_UNIQUE_INSTANCE	= 0x00000001  // No multiple instances
	 };
	 const int flags;
	 
	 // Configuration info (optional, set to 0 if you don't want it)
	 const ConfigParser::defTable_type* configDefinitions;
	 
	 // The two required functions - start and stop
	 const startFunction_type* const startFunction;
	 const stopFunction_type* const stopFunction;
	 
	 // Padding, for future use (ignore this, or fill it with 0's)
	 char _padding[28];
	 
	 // This little function checks if the stuff above is valid
	 const bool isOkay(void) const {
	    return ((nameShort != 0) && (nameLong != 0) && (copyright != 0) &&
		    (startFunction != 0) && (stopFunction != 0));
	 };
      };

    private:
      // Basic information about this module
      const basicInfo_type& basicInfo;

      // The configuration data class (optional)
      ConfigData* const configData;

    public:
      // Constructor
      Module(const basicInfo_type& bi, ConfigData* const cd = 0)
	: basicInfo(bi),
          configData(cd)
        {};
      
      // Destructor
      ~Module(void)
	{};

      // Return the basic information about the module
      const basicInfo_type& getBasicInfo(void) const
	{ return basicInfo; };

      // Return the configuration data class pointer
      ConfigData* const getConfigData(void) const
	{ return configData; };
      
      // Return the name (from the basic information) in full version format
      String getVersionString(void) const;
      
      // Return a name worthy enough to be used as a key in a map
      const char* getKeyName(void) const
	{ return basicInfo.nameShort; };
   };
};
   
// Complete forwarded declarations
# include "kineircd/daemon.h"

#endif // _INCLUDE_KINEIRCD_MODULE_H_
   
