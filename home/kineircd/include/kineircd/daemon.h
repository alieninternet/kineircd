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

# include <kineircd/kineircdconf.h>

# ifdef KINE_STL_HAS_SLIST
#  include <slist>
# else
#  include <list>
# endif

# include <string>
# include <set>
# include <map>
# include <ctime>
# include <aisutil/string/string.h>

extern "C" {
# include <sys/time.h>
# include <sys/types.h>
# include <unistd.h>
};

// Forwarded declarations (completed after class)
namespace Kine {
   class Config;
   class Signals;
   class Connection;
   class ProtocolInfo;
};

# include <kineircd/exit.h>
# include <kineircd/listener.h>
# include <kineircd/protocolname.h>
# include <kineircd/logger.h>

namespace Kine {
   // The Daemon class
   class Daemon {
    public:
      enum runlevel_type {
	 RUNLEVEL_INIT,			// Initialising the daemon
	 RUNLEVEL_NORMAL,		// Running normally
         RUNLEVEL_SHUTDOWN		// The daemon is shutting down
      };
      
      typedef std::map <ProtocolName, ProtocolInfo* const> protocols_type;
      
    private:
      runlevel_type runlevel;			// What stage is the daemon in
      
      Config& config;				// Configuration data
      Signals& signals;				// Signal handlers
      
      const long startTime;			// Time the daemon started
      timeval currentTime;			// The time now

      unsigned long long sentBytes;		// Total bytes sent
      unsigned long long receivedBytes;		// Total bytes received
      
      // The list of loggers (this should be hidden)
      typedef std::set <Logger*> loggerList_type;
      loggerList_type loggers;

      // The list of connections (this should be hidden)
# ifdef KINE_STL_HAS_SLIST
      typedef std::slist <Connection*> connections_type;
# else
      typedef std::list <Connection*> connections_type;
# endif
      connections_type connections;

      // The protocol list
      protocols_type protocols;
      
    public: // <=- temporary, pre-poller class code
      // For select()
      fd_set inFDSET, outFDSET;			// Input/Output descriptor sets
    private: // <=- temporary
      int maxDescriptors;			// Highest descriptor to check
      
      // Set the current time
      void setTime(void) 
	{ (void)gettimeofday(&currentTime, NULL); };

      // Process a new connection
      void newConnection(Listener& listener);

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
	{ return (unsigned long)(currentTime.tv_sec - startTime); };

      // Grab the time now
      const timeval& getTime(void) const
	{ return currentTime; };

      // Increase the sent bytes count
      void addSentBytes(const unsigned int bytes)
	{ sentBytes += bytes; };
      
      // Increase the sent bytes count
      void addReceivedBytes(const unsigned int bytes)
	{ receivedBytes += bytes; };

      // Protocol set manipulators, for the registrar and modules to tap into
      bool registerProtocol(const ProtocolName& name, ProtocolInfo& info);
      bool deregisterProtocol(const ProtocolName& name);
      ProtocolInfo* const findProtocol(const ProtocolName::Type::type type,
				       const std::string& name) const;

      // Logger set manipulators
      bool registerLogger(Logger& logger);
      bool deregisterLogger(Logger& logger);
      void log(const std::string& str,
	       const Logger::Mask::type mask = Logger::Mask::Housekeeping);
      
      // Main loop
      Exit::status_type run(void);
   };
};

// Complete forwarded declarations
# include <kineircd/config.h>
# include <kineircd/signals.h>
# include <kineircd/connection.h>
# include <kineircd/protocolinfo.h>

#endif // _INCLUDE_KINEIRCD_DAEMON_H_ 
