/* $Id$
 * 
 * Copyright (c) 2001,2002,2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2001,2002,2003 KineIRCd Development Team
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

#include <kineircd/module.h>
#include <kineircd/daemon.h>

#include "mod_filelog/module.h"
#include "mod_filelog/config.h"
#include "mod_filelog/filelog.h"


namespace {
   // Information about this module
   static const Kine::Module::Info info = {
      // Firstly, we list the version/copyright information for this module
      MOD_FILELOG_NAME_SHORT,
      MOD_FILELOG_NAME_LONG,
      MOD_FILELOG_VERSION_MAJOR, 
      MOD_FILELOG_VERSION_MINOR,
      0,

      // Flags to define how we need to be run
      0,

      // Configuration stuff
      &Kine::mod_filelog::Config::definitionTable
   };


   class mod_filelog : public Kine::Module {
    private:
      Kine::mod_filelog::Config config;
      Kine::mod_filelog::FileLog* logger;
      
    public:
      // Constructor
      mod_filelog(void)
	: logger(0)
	{};
      
      // Destructor
      ~mod_filelog(void)
	{
	   // If the logger has been created, deregister/delete it
	   if (logger != 0) {
	      (void)Kine::daemon().deregisterLogger(*logger);
	      delete logger;
	   }
	};
      
      // Return the information
      const Kine::Module::Info& getInfo(void) const
	{ return info; };

      // Return the configuration data class
      AIS::Util::ConfigData* const getConfigData(void)
	{ return &config; };
      
      /* moduleStart - Fire up the module
       * Original 15/10/2002 simonb
       */
      bool start(void) {
	 // Make our logging class
	 logger = new Kine::mod_filelog::FileLog(config);

	 // Make sure that worked..
	 if (logger == 0) {
	    return false;
	 }

	 // Register the logger
	 return Kine::daemon().registerLogger(*logger);
      };
   }; // class mod_filelog
}; // namespace {anonymous}


// The initialisation function, called by Kine
KINE_MODULE_INIT { return new mod_filelog(); };
