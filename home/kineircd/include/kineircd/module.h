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

// Template for the information structure YOU SHOULD USE in your module
# define KINE_MODULE_INFORMATION \
   static const struct Kine::Module::modInfo_type moduleInfo

// Template for the function which triggers a module to start
# define KINE_MODULE_START(x) \
   bool x(void)

// Template for the above which YOU SHOULD USE in your modules
# define KINE_MODULE_START_FUNCTION \
   static KINE_MODULE_START(moduleStart)

// Template for the function which triggers a module to start
# define KINE_MODULE_STOP(x) \
   void x(void)

// Template for the above which YOU SHOULD USE in your modules
# define KINE_MODULE_STOP_FUNCTION \
   static KINE_MODULE_STOP(moduleStop)

// Template for the optional function which hands modules a time-slice
# define KINE_MODULE_TIMESLICE(x) \
   int x(int)

// Template for the above which YOU SHOULD USE in your modules
# define KINE_MODULE_TIMESLICE_FUNCTION \
   static KINE_MODULE_TIMESLICE(moduleTimeSlice)


namespace Kine {
   class Module {
    public:
      /* This structure defines information about the module itself. Each 
       * module must have one of these present to define parameters about the 
       * module so that the server has some idea of how to handle it, what 
       * version it is and so on.
       */
      struct modInfo_type {
	 // Type of the module, so we know how to install it correctly
	 enum type_type {
	    PROTOCOL,			// Protocol module (pmod)
	    SERVICE,			// Service module (smod)
	    EXTENSION			// Core extension module (emod)
	 } const type;
	 
	 /* Name and version information of the module.
	  * Note that the full module version will be seen as follows:
	  * <name>-<versionMajor>.<versionMinor>.<versionPatchLevel>
	  *  <versionExtra>
	  * Eg. FooMod-1.2.3b
	  */
	 const char *name;
	 const char *copyright;
	 const unsigned char versionMajor;
	 const unsigned char versionMinor;
	 const unsigned short versionPatchLevel;
	 const char *versionExtra;

	 // Flags to determine how this module needs to be configured
	 enum flags_type {
	    FLAG_NEEDS_ROOT_START = 0x00000001,	// Needs root access to start
	    FLAG_NEEDS_ROOT_RUN = 0x00000002	// Needs root to run
	 } const flags;
	 
	 // Check that the details are okay
	 const bool isOkay(void) const
	   {
	      return (((type == PROTOCOL) || (type == SERVICE) || 
		       (type == EXTENSION)) &&
		      (name != 0) && (copyright != 0));
	   };
      } const &moduleInfo;
      
      // Constructor
      Module(const modInfo_type &mi)
	: moduleInfo(mi)
        {};
      
      // Destructor
      ~Module(void)
	{};
   };
};
   
#endif // _INCLUDE_KINEIRCD_MODULE_H_
   
