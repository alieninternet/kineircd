/* $Id$
 * Support class for modules
 * 
 * Copyright (c) 2002 Simon Butcher <pickle@alien.net.au>
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

# include <aisutil/config/parser.h>
# include <aisutil/string/string.h>

// Template for the function which initialises the module and returns the info
# define KINE_MODULE_INIT_PROTOTYPE(x) \
   Kine::Module* const x(void)

// The correct name of the symbol which is searched for by dlopen()
# define KINE_MODULE_INIT_SYMBOL_NAME \
   "KineIRCdModuleInit"

// Template for the above, which you should use in your modules (correct name)
# define KINE_MODULE_INIT \
   extern "C" KINE_MODULE_INIT_PROTOTYPE(KineIRCdModuleInit)

namespace Kine {
   // The module class modules inherit themselves from
   class Module {
    public:
      // Flags to determine how this module needs to be configured
      struct Flags { // <=- should be namespace?
	 enum type {
	    NONE = 0,			      //!< No flags set
	    UNIQUE_INSTANCE	= 0x00000001  //!< No multiple instances
	 };
      };
      
      /* This structure defines information about the module itself. Each 
       * module must have one of these present to define parameters about the
       * module so that the server has some idea of how to handle it, what
       * version it is and so on.
       */
      struct Info {
	 /* Name and version information of the module.
	  * Note that the full module version will be seen as follows:
	  * <nameShort>-<versionMajor>.<versionMinor>.<versionPatchLevel>
	  *  <versionExtra>
	  * Eg. FooMod-1.2.3b
	  */
	 const char* const nameShort;
	 const char* const nameLong;
	 const char* const copyright;
	 const unsigned char versionMajor;
	 const unsigned char versionMinor;
	 const unsigned short versionPatchLevel;
	 const char* const versionExtra;

	 /* This is a zero-terminated array of lines to be appended to the
	  * list for /INFO. Lines must be 60 visible characters or shorter.
	  * That also means formatting characters (\002 etc) do not count in
	  * that total :) Do not use \a, \b, \t, \n, \v, \f, or \r, and make
	  * sure characters are in UTF-8. If there is no information you want
	  * listed, simply set this pointer to 0.
	  */
	 const char* const* versionInfo;

	 // Flags, see the list above
	 const long flags;

	 // Configuration info (optional, set to 0 if you don't want it)
	 const AISutil::ConfigParser::defTable_type* const configDefinitions;
	 
	 // This little function checks if the stuff above is valid
	 const bool isOkay(void) const {
	    return ((nameShort != 0) && (nameLong != 0) && (copyright != 0));
	 };
      };

    protected:
      // Constructor
      Module(void)
        {};
      
    public:
      // Destructor
      virtual ~Module(void)
	{};
      
      // Return the information about the module
      virtual const Info& getInfo(void) const = 0;
      
      // Return the configuration data class pointer
      virtual AISutil::ConfigData* const getConfigData(void)
	{ return 0; };
      
      // Start the module
      virtual bool start(void) = 0;

      // Return the name (from the information) in full version format
      const AISutil::String getVersionString(void) const;
      
      // Return a name worthy enough to be used as a key in a map
      const char* const getKeyName(void) const
	{ return getInfo().nameShort; };
   };
};
   
#endif // _INCLUDE_KINEIRCD_MODULE_H_
   
