[+ AutoGen5 template h +]
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
 *
 *
[+(dne " * ")+]
 */

[+(make-header-guard "kine")+]

# include <kineircd/entity.h>
# include <kineircd/denizen.h>
# include <kineircd/sender.h>
# include <kineircd/user.h>
# include <kineircd/errors.h>
 
namespace Kine {
   class Client;
   
   //! A server
   class Server : public Denizen, public Sender {
    public:
      //! Server modes type 
      typedef unsigned long modes_type;
      
      // Server modes...
      struct Mode { // <=- should be namespace?
	 enum mode_type {[+ FOR servermodes +]
	    [+ (sprintf "%-25s = 0x%08X%s //!< (+%s)"
	          (get "name")
		  (expt 2 (for-index))		;; calculate the bitmask
		  (if (last-for?) " " ",")	;; no comma on last iteration
		  (get "irc2char"))
	     +][+ ENDFOR servermodes +]
	 };
      };
      
    private:
      const std::string hostname;		// The server's hostname
      modes_type modes;				// Server modes set
      
    protected:
      //! Constructor
      Server(const std::string& _hostname,
	     const AIS::Util::Time& _signonTime)
	: Denizen(_signonTime),
          hostname(_hostname),
	  modes(0)
	{};

    public:
      //! Destructor
      virtual ~Server(void)
	{};


      //! Return the hostname of the server
      const std::string& getHostname(void) const
        { return hostname; };
	
      //! Return the 'name' (a server's hostname is normally its name)
      const std::string& getName(void) const
	{ return getHostname(); };


      //! Return the server modes
      const modes_type getModes(void) const
	{ return modes; };
      
      //! Check if the given mode is set
      const bool isModeSet(const Mode::mode_type mode) const
	{ return (getModes() & mode); };


      //! Is this server hidden from the given user? (this is broken)
      const bool isHiddenFrom(const User& who) const
	{ return isModeSet(Mode::HIDDEN); };
      
      
      //! Request administrative information from this server
      virtual const Error::error_type requestAdminInfo(const Client& who) const
	{ return Error::UNSUPPORTED_BY_ENTITY; };
      
      //! Request the current message of the day from this server
      virtual const Error::error_type requestMOTD(const Client& who) const
	{ return Error::UNSUPPORTED_BY_ENTITY; };
      
      //! Request server/daemon information from this server
      virtual const Error::error_type
	requestDaemonInfo(const Client& who) const
	{ return Error::UNSUPPORTED_BY_ENTITY; };

      //! Request statistics information from this server
      virtual const Error::error_type
	requestStatistics(const Client& who, const std::string& request) const
	{ return Error::UNSUPPORTED_BY_ENTITY; };
      
      //! Request the local time on this server
      virtual const Error::error_type requestTime(const Client& who) const
	{ return Error::UNSUPPORTED_BY_ENTITY; };

      //! Request the daemon's version from this server
      virtual const Error::error_type requestVersion(const Client& who) const
	{ return Error::UNSUPPORTED_BY_ENTITY; };
   }; // class Server
}; // namespace Kine

# include <kineircd/client.h>
 
#endif // [+(. header-guard)+]
