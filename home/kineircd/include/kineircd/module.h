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

# include "kineircd/str.h"

// Template for the function which initialises the module and returns the info
# define KINE_MODULE_INIT_PROTOTYPE(x) \
   const Kine::Module *x(void)

// The correct name of the symbol which is searched for by dlopen()
# define KINE_MODULE_INIT_SYMBOL_NAME \
   "KineIRCdModuleInit"

// Template for the above, which you should use in your modules (correct name)
# define KINE_MODULE_INIT \
   extern "C" KINE_MODULE_INIT_PROTOTYPE(KineIRCdModuleInit)

// Template for the function which triggers a module to start
# define KINE_MODULE_START(x) \
   bool x(void)

// Template for the function which triggers a module to stop
# define KINE_MODULE_STOP(x) \
   void x(void)

namespace Kine {
   class Module {
    public:
      // The functions all modules are required to have - start and stop
      typedef KINE_MODULE_START(startFunction_type);
      typedef KINE_MODULE_STOP(stopFunction_type);
      
      // Type of the module, so we know how to install it correctly
      enum type_type {
	 TYPE_PROTOCOL,			// Protocol module (pmod)
	 TYPE_SERVICE,			// Service module (smod)
	 TYPE_EXTENSION			// Core extension module (emod)
      } const type;
      
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
	 const char *nameShort;
	 const char *nameLong;
	 const char *copyright;
	 const unsigned char versionMajor;
	 const unsigned char versionMinor;
	 const unsigned short versionPatchLevel;
	 const char *versionExtra;

	 // Flags to determine how this module needs to be configured
	 enum flags_type {
	    FLAG_NONE = 0,			      // No flags set
            FLAG_UNIQUE_INSTANCE	= 0x00000001  // No multiple instances
	 };
	 const int flags;
	 
	 // The two required functions - start and stop
	 const startFunction_type *startFunction;
	 const stopFunction_type *stopFunction;
      } const &basicInfo;

    protected:
      // Constructor
      Module(const type_type t, const basicInfo_type &mi)
	: type(t),
          basicInfo(mi)
        {};
      
    public:
      // Destructor
      virtual ~Module(void)
	{};

      // Return the basic information about the module
      const basicInfo_type &getBasicInfo(void) const
	{ return basicInfo; };
      
      // Return the name (from the basic information) in full version format
      String getVersionString(void) const;
   };
};
   
#endif // _INCLUDE_KINEIRCD_MODULE_H_
   
