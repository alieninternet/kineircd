/* $Id$
 * The main class
 * 
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

#ifndef _INCLUDE_KINEIRCD_DAEMON_H_
# define _INCLUDE_KINEIRCD_DAEMON_H_ 1

# include "kineircd/kineircdconf.h"

# include <ctime>

// Forwarded declarations (completed after class)
namespace Kine {
   class Config;
   class Signals;
};

# include "kineircd/exit.h"
# include "kineircd/str.h"

namespace Kine {
   // The Daemon class
   class Daemon {
    public:
      enum runlevel_type {
	 RUNLEVEL_INIT,			// Initialising the daemon
	 RUNLEVEL_NORMAL,		// Running normally
         RUNLEVEL_SHUTDOWN		// The daemon is shutting down
      };
      
    private:
      runlevel_type runlevel;			// What stage is the daemon in
      
      Config& config;				// Configuration data
      Signals& signals;				// Signal handlers
      
      const time_t startTime;			// Time the daemon started
      time_t timeNow;				// The time now
      
    public:
      // Constructor
      Daemon(Config& conf, Signals& sigs);

      // Destructor
      ~Daemon(void);

      // Return the run-level or stage the daemon is running in
      runlevel_type const getRunlevel(void) const
	{ return runlevel; };
      
      // Grab the configuration data
      const Config& getConfig(void) const
	{ return config; };
      
      // Grab the signal handler
      Signals& getSignals(void)
	{ return signals; };
      
      // Grab our 'uptime'
      unsigned long getUptime(void) const
	{ return (unsigned long)difftime(timeNow, startTime); };

      // Grab the time now
      time_t const getTime(void) const
	{ return timeNow; };
      
      // Main loop
      Exit::status_type run(void);
   };
};

// Complete forwarded declarations
# include "kineircd/config.h"
# include "kineircd/signals.h"

#endif // _INCLUDE_KINEIRCD_DAEMON_H_ 
