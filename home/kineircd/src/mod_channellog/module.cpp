/* $Id$
 * KineIRCd module interface for the channel based logging module
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

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif

#include <kineircd/module.h>
#include <kineircd/daemon.h>

#include "mod_channellog/module.h"
#include "mod_channellog/config.h"
#include "mod_channellog/channellog.h"


namespace {
   // Information about this module
   static const Kine::Module::Info info = {
      // Firstly, we list the version/copyright information for this module
      MOD_CHANNELLOG_NAME_SHORT,
      MOD_CHANNELLOG_NAME_LONG,
      MOD_CHANNELLOG_COPYRIGHT,
      MOD_CHANNELLOG_VERSION_MAJOR, 
      MOD_CHANNELLOG_VERSION_MINOR,
      MOD_CHANNELLOG_VERSION_PATCHLEVEL,
      MOD_CHANNELLOG_VERSION_EXTRA,
      0,

      // Flags to define how we need to be run
      0,

      // Configuration stuff
      &Kine::mod_channellog::Config::definitionTable
   };


   class mod_channellog : public Kine::Module {
    private:
      Kine::mod_channellog::Config config;
      Kine::mod_channellog::ChannelLog* logger;
      
    public:
      // Constructor
      mod_channellog(void)
	: logger(0)
	{};
      
      // Destructor
      ~mod_channellog(void)
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
       * Original 15/10/2002 simonb
       */
      bool start(void) {
	 // Make our logging class
	 logger = new Kine::mod_channellog::ChannelLog(config);

	 // Make sure that worked..
	 if (logger == 0) {
	    return false;
	 }

	 // Register the logger
	 return Kine::daemon().registerLogger(*logger);
      };
   }; // class mod_channellog
}; // namespace {anonymous}


// The initialisation function, called by Kine
KINE_MODULE_INIT { return new mod_channellog(); };
