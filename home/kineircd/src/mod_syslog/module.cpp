/* $Id$
 * KineIRCd module interface for the syslog logging module
 * 
 * Copyright (c) 2001,2002 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2001,2002 KineIRCd Development Team
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

#include "mod_syslog/module.h"
#include "mod_syslog/syslog.h"


namespace {
   // Information about this module
   static const Kine::Module::Info info = {
      // Firstly, we list the version/copyright information for this module
      MOD_SYSLOG_NAME_SHORT,
      MOD_SYSLOG_NAME_LONG,
      MOD_SYSLOG_COPYRIGHT,
      MOD_SYSLOG_VERSION_MAJOR, 
      MOD_SYSLOG_VERSION_MINOR,
      MOD_SYSLOG_VERSION_PATCHLEVEL,
      MOD_SYSLOG_VERSION_EXTRA,
      0,

      // Flags to define how we need to be run
      Kine::Module::Flags::UNIQUE_INSTANCE,

      // Configuration stuff
      &Kine::mod_syslog::Config::definitionTable
   };


   class mod_syslog : public Kine::Module {
    private:
      Kine::mod_syslog::Config config;
      Kine::mod_syslog::Syslog* logger;
      
    public:
      // Constructor
      mod_syslog(void)
	: logger(0)
	{};
      
      // Destructor
      ~mod_syslog(void)
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
      AISutil::ConfigData* const getConfigData(void)
	{ return &config; };
      
      /* moduleStart - Fire up the module
       * Original 01/11/2002 simonb
       */
      bool start(void) {
	 // Make our logging class
	 logger = new Kine::mod_syslog::Syslog(config);
	 
	 // Make sure that worked..
	 if (logger == 0) {
	    return false;
	 }
	 
	 // Register the logger
	 return Kine::daemon().registerLogger(*logger);
      };
   }; // class mod_syslog
}; // namespace {anonymous}


// The initialisation function, called by Kine
KINE_MODULE_INIT { return new mod_syslog(); };
