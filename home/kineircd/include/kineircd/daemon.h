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
# include <sys/types.h>
# include <unistd.h>
# include <aisutil/string/string.h>
# include <aisutil/time.h>

# include <kineircd/listener.h>
# include <kineircd/logger.h>
# include <kineircd/connection.h>
# include <kineircd/protocolinfo.h>

namespace Kine {
   //! The Daemon class
   class Daemon {
    public:
      enum runlevel_type {
	 RUNLEVEL_INIT,			//!< Initialising the daemon
	 RUNLEVEL_NORMAL,		//!< Running normally
         RUNLEVEL_SHUTDOWN		//!< The daemon is shutting down
      };
      
    private:
      runlevel_type runlevel;			//!< What stage is the daemon in
      
      const AISutil::Time startTime;		//!< Time the daemon started
      AISutil::Time currentTime;		//!< The time now

      unsigned long long sentBytes;		//!< Total bytes sent
      unsigned long long receivedBytes;		//!< Total bytes received
      
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
      typedef std::map <const char* const, ProtocolInfo* const>	protocols_type;
      protocols_type protocols;
      
    public: // <=- temporary, for crappy pre-poller class code :(
      // For select()
      fd_set inFDSET, outFDSET;			//!< Input/Output descriptor sets
    private: // <=- temporary
      int maxDescriptors;			//!< Highest descriptor to check
      
      //! Set the current time
      void setTime(void) 
	{ currentTime.setTime(); };

      //! Process a new connection
      void newConnection(Listener& listener);

      //! Our single instance :)
      static Daemon* instance;
      
      //! Constructor
      Daemon(void);
      
    public:
      //! Destructor
      ~Daemon(void);

      //! Create the single instance of this class
      static void initInstance(void);
      
      //! Get the single instance of this class (this class 'always' exists)
      static Daemon& getInstance(void)
	{ return *instance; };
      
      //! Return the run-level or stage the daemon is running in
      runlevel_type const getRunlevel(void) const
	{ return runlevel; };
      
      //! Grab our 'uptime'
      const AISutil::Time getUptime(void) const
	{ return currentTime - startTime; };

      //! Grab the time now
      const AISutil::Time& getTime(void) const
	{ return currentTime; };

      //! Increase the sent bytes count
      void addSentBytes(const unsigned int bytes)
	{ sentBytes += bytes; };
      
      //! Increase the sent bytes count
      void addReceivedBytes(const unsigned int bytes)
	{ receivedBytes += bytes; };

      // Protocol set manipulators, for the registrar and modules to tap into
      bool registerProtocol(ProtocolInfo& info);
      void deregisterProtocol(const ProtocolInfo& info);
      ProtocolInfo* const findProtocol(const ProtocolInfo::Type::type type,
				       const std::string& name) const;

      // Logger set manipulators
      bool registerLogger(Logger& logger);
      bool deregisterLogger(Logger& logger);
      void log(const std::string& str,
	       const Logger::Mask::type mask = Logger::Mask::Housekeeping);
      
      //! Main loop
      bool run(void);
   };

   
   //! Return the current instance of the Daemon:: class
   inline static Daemon& daemon(void)
     { return Daemon::getInstance(); };
};

#endif // _INCLUDE_KINEIRCD_DAEMON_H_ 
