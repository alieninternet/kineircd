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

#ifndef _INCLUDE_KINEIRCD_SERVICE_H_
# define _INCLUDE_KINEIRCD_SERVICE_H_ 1

# include <aisutil/string/mask.h>
# include <kineircd/user.h>
# include <kineircd/client.h>
# include <kineircd/errors.h>

namespace Kine {
   /*!
    * \brief A service
    * 
    * A service is a special type of Client, designed for trusted, automated
    * clients. Unlike ordinary \e bots or \e drones, which normally connect
    * to the network as a User, services are allowed on the network by the
    * network's administrators specifically.
    * 
    * Being automated, services only ever accept commands, or \e queries. They
    * then reply to the requesting Client, normally with a \e notice (using 
    * Kine::Client::sendNotice()) to avoid endless loop problems.
    * 
    * Services might connect using a restricted Protocol, similar to that used
    * for User connections, but in %Kine it is common for the service to be
    * provided by a loaded Module.
    */
   class Service : public Client {
    private: 
      const ClientName nickname;		//!< Service's nickname
      const AIS::Util::StringMask scopeMask;	//!< Service server scope mask
      
    protected:
      /*!
       * \brief Constructor
       * 
       * \param _nickname The service's \a nickname
       * \param _hostname The host the service is connected from
       * \param _signonTime The time the service connected to the network
       */
      explicit Service(const ClientName& _nickname,
		       const std::string& _hostname,
		       const AIS::Util::Time& _signonTime)
	: Client(_hostname, _signonTime),
          nickname(_nickname),
          scopeMask("*")
	{};
      
    public:
      //! Destructor
      virtual ~Service(void)
	{};

      
      // Return the user's nickname
      const ClientName& getNickname(void) const
	{ return nickname; };

      
      // Return the user name (default to the service nickname)
      virtual const std::string& getUsername(void) const
	{ return getName(); };
      
      
      //! Return the server scope mask
      const AIS::Util::StringMask& getScopeMask(void) const
	{ return scopeMask; };
      
      //! Return the service 'type' bitmask
      const long getServiceType(void) const
	{ return 0; /* replace this! */ };
      
      
      /*!
       * \brief Send a query to this service from a client
       *
       * This is similar to Kine::Receiver::sendMessage(), except only
       * clients can send queries to services.
       * 
       * \copydoc Kine::Receiver::sendMessage()
       */
      virtual const Error::error_type
	sendQuery(Client& sender,
		  const std::string& message,
		  const Receiver::Directivity directivity =
		  Receiver::Directivity())
	{ return Error::UNSUPPORTED_BY_ENTITY; };
   }; // class Service
}; // namespace Kine

#endif // _INCLUDE_KINEIRCD_SERVICE_H_
