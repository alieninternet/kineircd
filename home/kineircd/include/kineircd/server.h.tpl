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
# include <kineircd/errors.h>

namespace Kine {
   class Client;
   class User;
//   class Service;
   
   //! A server
   class Server : public Denizen, public Sender {
    public:
      //! A server's hostname, conforming to IRC rules
      class Name : public Kine::Name {
       public:
	 //! Constructor
	 Name(void)
	   {};
	 
	 //! Constructor (a magic one)
	 template <class T>
	   Name(const T& string)
	     : Kine::Name(string)
	     {};
	 
	 //! Destructor
	 ~Name(void)
	   {};
	 
	 // Check if the name is valid, according to server host naming rules
	 const Error::error_type checkValidity(void) const;
      }; // class Name

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
      const Name hostname;			// The server's hostname
      modes_type modes;				// Server modes set

      counter_type userCount;			//! Number of users held
      counter_type serviceCount;		//! Number of services held
      counter_type serverCount;			//! Number of servers held
      
    protected:
      //! Constructor
      explicit Server(const Name& _hostname,
		      const AIS::Util::Time& _signonTime)
	: Entity(_signonTime),
          hostname(_hostname),
	  modes(0)
	{};

    public:
      //! Destructor
      virtual ~Server(void)
	{};


      //! Return the name of the server
      const Name& getServerName(void) const
	{ return hostname; };
      
      // Return the hostname of the server
      const std::wstring& getHostname(void) const
        { return getServerName(); };
	
      // Return the 'name' (a server's hostname is normally its name)
      const Kine::Name& getName(void) const
	{ return getServerName(); };


      //! Return the server modes
      const modes_type getModes(void) const
	{ return modes; };
      
      //! Check if the given mode is set
      const bool isModeSet(const Mode::mode_type mode) const
	{ return (getModes() & mode); };


      // Is this server hidden from the given user? (this is broken)
      const bool isHiddenFrom(const User& user) const
	{ return isModeSet(Mode::HIDDEN); };
      

      //@{
      /*!
       * \brief Return the number of clients connected to this server
       * 
       */
      const counter_type getClientCount(void) const
	{ return (getUserCount() + getServiceCount()); };

      /*!
       * \brief Return the number of users connected to this server
       * 
       */
      const counter_type& getUserCount(void) const
	{ return userCount; };
      
      /*!
       * \brief Return the number of services connected to this server
       * 
       */
      const counter_type& getServiceCount(void) const
	{ return serviceCount; };
      
      /*!
       * \brief Return the number of servers connected locally
       * 
       */
      const counter_type& getServerCount(void) const
	{ return serverCount; };
      //@}


      //@{
      //! Request administrative information from this server
      virtual const Error::error_type requestAdminInfo(const Client& who)
	{ return Error::UNSUPPORTED_BY_ENTITY; };
      
      //! Request the current message of the day from this server
      virtual const Error::error_type requestMOTD(const Client& who)
	{ return Error::UNSUPPORTED_BY_ENTITY; };
      
      //! Request server/daemon information from this server
      virtual const Error::error_type requestDaemonInfo(const Client& who)
	{ return Error::UNSUPPORTED_BY_ENTITY; };

      //! Request statistics information from this server
      virtual const Error::error_type
	requestStatistics(const Client& who, const std::string& request)
	{ return Error::UNSUPPORTED_BY_ENTITY; };
      
      //! Request the local time on this server
      virtual const Error::error_type requestTime(const Client& who)
	{ return Error::UNSUPPORTED_BY_ENTITY; };

      //! Request the daemon's version from this server
      virtual const Error::error_type requestVersion(const Client& who)
	{ return Error::UNSUPPORTED_BY_ENTITY; };
      //@}
      

      //@{
      /*!
       * \brief Register a User to the network
       * 
       * This will register the given \p user to the network \e via this
       * server. Each User connects to a specific server on the network,
       * and must be registered as being connected to that particular server
       * for verification purposes.
       *
       * \return The status of the operation
       * \retval Error::NO_ERROR
       *    The \p user has been registered to the network, as being
       *    \e logically connected to this server
       * \retval Error::ENTITY_EXISTS
       *    This \p user is already registered. If the given \p user is
       *    being reregistered, the most likely explaination is that there is
       *    a flaw in your code. This means that the given \p user is
       *    identical to one which is already registered -- not just by name,
       *    but the instance of the User is the same one as that which is
       *    already registered.
       * \retval Error::NAME_HAS_BAD_CHARS
       *    The \p user has a name which contains characters which are
       *    considered invalid. It's best to check the name closer using
       *    the Client::Name routines
       * \retval Error::NAME_IS_IN_USE
       *    The \p user given is using a name which is already in use on the
       *    network, and has not been registered
       * \retval Error::NAME_TOO_LONG
       *    The \p user given has a name which is considered to be too long
       * \retval Error::NICKNAME_BEGINS_WITH_DIGIT
       *    The \p user has a nickname which begins with a digit, and is
       *    considered to be invalid. Only arabic digits (\c 0 to \c 9) cause
       *    this.
       * \retval Error::PERMISSION_DENIED
       *    The \p user is not allowed to be registered to this server. The
       *    server may have a special property, such as not being able to
       *    carry as clients. User registration to such a server would
       *    obviously be illegal, under such terms.
       * \retval Error::UNSUPPORTED_BY_ENTITY
       *    This server cannot accept User entities.
       */
      const Error::error_type addUser(User& user);
      
      /*!
       * \brief Register a Service to the network
       * 
       * This will register the given \p service to the network \e via this
       * server. See addUser() for more information.
       * 
       * \return The status of the operation
       * \retval Error::NO_ERROR
       *    The \p service has been registered to the network, as being
       *    \e logically connected to this server
       * \retval Error::ENTITY_EXISTS
       *    This \p service is already registered. If the given \p service is
       *    being reregistered, the most likely explaination is that there is
       *    a flaw in your code.
       * \retval Error::NAME_HAS_BAD_CHARS
       *    The \p service has a name which contains characters which are
       *    considered invalid. It's best to check the name closer using
       *    the Client::Name routines
       * \retval Error::NAME_IS_IN_USE
       *    The \p service given is using a name which is already in use on
       *    the network within the given scope. If the \p service you're
       *    trying to register has a scope identical than the one which
       *    already exists, you should try to tighten or widen the scope up
       *    to avoid the conflict. See Service::getScopeMask() and related
       *    functions for more information on this.
       * \retval Error::NAME_TOO_LONG
       *    The \p service given has a name which is considered to be too long
       * \retval Error::NICKNAME_BEGINS_WITH_DIGIT
       *    The \p service has a nickname which begins with a digit, and is
       *    considered to be invalid. Only arabic digits (\c 0 to \c 9) cause
       *    this.
       * \retval Error::PERMISSION_DENIED
       *    The \p service is not allowed to be registered to this server.
       * \retval Error::UNSUPPORTED_BY_ENTITY
       *    This server cannot carry Service entities.
       */
//      const Error::error_type addService(Service& service);
      //@}
      
      
      // Decamp
      void decamp(void) {};
   }; // class Server
}; // namespace Kine

# include <kineircd/user.h>
//# include <kineircd/service.h>
 
#endif // [+(. header-guard)+]
