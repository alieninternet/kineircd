/* $Id$
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
 */

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif
#include "kineircd/kineircdconf.h"

#include <cctype>

#include "kineircd/config.h"
#include "kineircd/moduledescriptor.h"
#include "lib/language/config.h"
#include "lib/listener/config.h"
#include "lib/debug.h"


using AIS::Util::String;
using namespace Kine;


/* classHandleModule - Get a module's filename and attempt to load & configure
 * Original 21/07/2002 simonb
 */
LIBAISUTIL_CONFIG_CLASS_HANDLER(Kine::Config::classHandleModule)
{
#ifdef KINE_DEBUG_ASSERT
   assert(dataVariable != 0);
#endif
   
   // Check if the first value is empty (the filename field)
   if (values.empty() || values.front().empty()) {
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
   if ((moduleDesc->getModule().getInfo().configDefinitions != 0) &&
       (moduleDesc->getModule().getConfigData() != 0)) {
      // Parse the module's configuration data
      return 
	AIS::Util::ConfigParser::parse(configData, position,
				       ((void *)
					(moduleDesc->getModule().getInfo().
					 configDefinitions)),
				       *moduleDesc->getModule().getConfigData());
   } else {
      errString = "Module requires no additional configuration";
      return false;
   }
   
   // Just be happy, the module loaded!
   return true;
}


/* varHandleModule - Get a module's filename and attempt to load
 * Original 21/07/2002 simonb
 */
LIBAISUTIL_CONFIG_VARIABLE_HANDLER(Kine::Config::varHandleModule)
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
LIBAISUTIL_CONFIG_VARIABLE_HANDLER(Kine::Config::varHandleNetworkName)
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
   
   for (; *name != '\0'; ++name) {
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
      
#ifdef KINE_DEBUG
   debug("Config::varHandleNetworkName() -> '" + 
	 dataClass.*((String ConfigData::*)dataVariable) + '\'');
#endif

   return true;
}


// Our instance... (Well, I had to hide.. uhh.. I mean put it somewhere :)
Kine::Config* Kine::Config::instance = 0;


/* configure - Load/parse configuration using the filename previously set
 * Original 17/06/2002 simonb
 */
bool Kine::Config::configure(void)
{
   // Tell anyone who is interested that we are now configuring..
   configuring = true;
   
   // The config parser instance we will use
   AIS::Util::ConfigParser parser(*this, &topDefs, configFile);
   
   // Configure this thing :)
   bool happy = parser.configure();
   
   // We are no longer configuring!
   configuring = false;
   
   // Return whatever the parser had to say..
   return happy;
}
