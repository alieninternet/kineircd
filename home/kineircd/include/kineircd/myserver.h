/* $Id$
 * 
 * Copyright (c) 2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2003 KineIRCd Development Team
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

#ifndef _INCLUDE_KINEIRCD_MYSERVER_H_
# define _INCLUDE_KINEIRCD_MYSERVER_H_ 1

# include <kineircd/server.h>
# include <kineircd/config.h>

namespace Kine {
   /*!
    * \brief Our server instance
    * 
    * This is a special instance of a Server, which is the specific server
    * this daemon represents.
    *
    * Unlike an ordinary Server, though, this also provides access to special
    * functions, such as extended information or sending \e wallops messages.
    * 
    * You should reference it through Kine::myServer().
    */
   class MyServer : public Server {
    private:
      //! Our instance
      static MyServer* instance;
      
      //! Constructor
      MyServer(void);
      
    public:
      //! Destructor
      ~MyServer(void)
	{};
      
      // Create the single instance of this class
      static void initInstance(void);
      
      // Return out instance
      static MyServer& getInstance(void)
	{ return *instance; };
      
      
      // Return the server's description
      const std::string& getDescription(void) const
	{ return config().getServerDescription(); };
      
      // Return the server we're "connected to" (technically, that's us)
      Server& getServer(void) const
	{ return const_cast<MyServer&>(*this); };


      //@{
      /*!
       * \brief Rehash the server (from a module)
       * 
       * This will \e rehash the server -- reloading the configuration
       * file(s), the language data file(s), and so on. While most variables
       * which are configurable can be changed with a rehash, there are some
       * which unfortunately will require at least a restart() to be
       * effective.
       * 
       * Depending on the configuration of the server, a rehash may also
       * have the effect of erasing all \e whowas information, locally set
       * bans, and so on and so forth.
       * 
       * \return The status of the operation
       * \retval Kine::Error::NO_ERROR
       *    The server has finished rehashing successfully, or at the very
       *    least it's started rehashing.
       */
      const Error::error_type rehash(void)
	{ return Error::UNKNOWN_ERROR; };

      /*!
       * \brief Rehash the server (requested from a user)
       * 
       * \copydoc rehash(void)
       * 
       * If the rehash is requested by a user, you should use this, as it
       * will check the permissions of the user appropriately to see if they
       * have the appropriate access to rehash the server.
       * 
       * \param user The user requestion the rehash operation
       * \retval Kine::Error::PERMISSION_DENIED
       *    The given \p user does not have enough access to rehash the
       *    server. This may be because the \p user is not an <em>IRC
       *    Operator</em>, or maybe the \p user is one and simply has not
       *    been given the rights to rehash the server.
       * \retval Kine::Error::UNREGISTERED_ENTITY
       *    The given \p user is not registered on the network, and therefore
       *    cannot perform operations
       */
      const Error::error_type rehash(const User& user)
	{ return Error::UNKNOWN_ERROR; };
      //@}
      

      //@{
      /*!
       * \brief Restart the server (from a module)
       * 
       * This will restart the server, shutting it down and loading a
       * completely new daemon process, with the same command line
       * parameters, if any, that were originally given.
       * 
       * \warning If the server is incorrectly configured, the server may
       *    not be able to restart itself! If this is the case, this
       *    operation will act like shutdown() in that it will require
       *    <b>manual intervention</b> in order to restore the IRC service.
       * \param reason The reason for restarting the server. Since the
       *    server will most likely drop all connections during the
       *    restart process, a reason should be given for the operation.
       * \return The status of the operation
       * \retval Kine::Error::NO_ERROR
       *    The server will now restart, or is just about to
       * \retval Kine::Error::TEXT_TOO_SHORT
       *    The given reason is too short to be considered a reasonable
       *    explaination as to why the server should be restarted
       */
      const Error::error_type restart(const std::string& reason)
	{ return Error::UNKNOWN_ERROR; };

      /*!
       * \brief Restart the server (requested by a user)
       * 
       * \copydoc restart(const std::string& reason)
       * 
       * If the restart is being requested by a User, you should use this
       * method, since the permissions of the \p user will be checked
       * properly to conform with this server's policy.
       * 
       * \param user The user requesting the server to restart itself
       * \retval Kine::Error::PERMISSION_DENIED
       *    The given \p user is not an <em>IRC Operator</em>, or at least
       *    has not been given the permission (maybe in the configuration
       *    file) to restart the server
       * \retval Kine::Error::UNREGISTERED_ENTITY
       *    The given \p user is not registered on this network, and cannot
       *    request operations
       */
      const Error::error_type restart(const User& user,
				      const std::string& reason)
	{ return Error::UNKNOWN_ERROR; };
      //@}


      //@{
      /*!
       * \brief Shutdown the server (from a module) 
       * 
       * This will start the server shut down process, which ultimately
       * results in the entire daemon terminating. Unlike restart(), this
       * will not restart the server after the shut down is complete.
       * 
       * \warning For obvious reasons, this operation will cause the daemon
       *    to shut down. It should not be used lightly, since <b>manual
       *    intervention</b> will be required if the server is to be
       *    restarted. 
       * \param reason The reason for the server to shut down
       * \return The status of the operation
       * \retval Kine::Error::NO_ERROR
       *    The server has started to shut down
       * \retval Kine::Error::TEXT_TOO_SHORT
       *    The given \p reason is too short, and cannot be accepted.
       *    Depending on how the server is configured by its administrator,
       *    a minimum message length may be required to stop people from
       *    shutting the server down with useless reasons.
       */
      const Error::error_type shutdown(const std::string& reason)
	{ return Error::UNKNOWN_ERROR; };

      /*!
       * \brief Shut down the server (requested by a user)
       *  
       * \copydoc shutdown(const std::string& reason)
       * 
       * You need to use this form when a User is requesting the server to
       * shutdown in order to evaluate the correct permissions. If you
       * need to shut the server down from a module, you may use
       * form shutdown(const std::string& reason)
       * 
       * \param user The user who is trying to shut down the server
       * \retval Kine::Error::UNREGISTERED_ENTITY
       *    The given \p user is not registered, and cannot request a
       *    shut down!
       * \retval Kine::Error::PERMISSION_DENIED
       *    The given \p user is not an <em>IRC Operator</em>, or if they are,
       *    their permissions do not allow them to shut the server down.
       */
      const Error::error_type shutdown(const User& user,
				       const std::string& reason)
	{ return Error::UNKNOWN_ERROR; };
      //@}
   }; // class MyServer


   /*!
    * \brief Return the MyServer instance
    *
    * Since MyServer is a singleton, this is how you should reference it in
    * your code.
    *
    * \return The current instance of MyServer
    * \relates Kine::MyServer
    */
   inline static MyServer& myServer(void)
     { return MyServer::getInstance(); };
};

#endif // _INCLUDE_KINEIRCD_MYSERVER_H_
