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

#ifndef _INCLUDE_KINEIRCD_RECEIVER_H_
# define _INCLUDE_KINEIRCD_RECEIVER_H_ 1

# include <string>
# include <kineircd/errors.h>

namespace Kine {
   class Entity;
   class Sender;
   
   /*!
    * \brief Abstract base class for \e receivers
    * 
    * \e Receivers a type of Entity which can receive messages from Sender
    * objects. Message types primarily include ordinary messages (or
    * <em>PRIVMSG</em>'s) and notices.
    */
   class Receiver : virtual public Entity {
    public:
      //! The 'directivity' of a recipient (acuteness of target specification)
      union Directivity {
	 struct {
	    bool address:1;		//!< The IP address was specified
	    bool hostname:1;		//!< The hostname was specified
	    bool name:1;		//!< The unique name was specified
	    bool network:1;		//!< The network was specified
	    bool server:1;		//!< The server was specified
	    bool username:1;		//!< The username was specified
	 };
	 
	 unsigned char conflux;
	 
	 //! Default constructor (sets us up into the 'unknown' state)
	 Directivity(void)
	   : conflux(0)
	   {};
      };
      
    protected:
      //! Constructor
      Receiver(void)
	{};
      
    public:
      //! Destructor
      virtual ~Receiver(void)
	{};

      
      /*!
       * \brief Send a message to this recipient
       * 
       * \param sender The sender of the message
       * \param message The message itself (or the \e payload)
       * \param directivity The focus or Directivity of the specified
       *    receiver
       * \return The status of the operation
       * \retval Kine::Error::NO_ERROR
       *    The message was sent successfully. Note that this <b>does not</b>
       *    guarantee that the message will be received.
       * \retval Kine::Error::PERMISSION_DENIED
       *    The \p sender is not allowed to send to this receiver
       * \retval Kine::Error::TEXT_TOO_LONG
       *    The given message was too long. Some entities impose maximum
       *    length limits on the messages given, and others will simply
       *    convert them into fragments.
       * \retval Kine::Error::TEXT_TOO_SHORT
       *    The given message was too short. This normally means that the
       *    message was empty, however some entities may impose minimum
       *    length limits.
       * \retval Kine::Error::UNREGISTERED_ENTITY
       *    The \p sender is not registered on the network, and therefore
       *    cannot send messages
       * \retval Kine::Error::UNSUPPORTED_BY_ENTITY
       *    The receiver is unequipped to accept the message
       */
      virtual const Error::error_type
	sendMessage(Sender& sender,
		    const std::wstring& message,
		    const Directivity directivity = Directivity())
	{ return Error::UNSUPPORTED_BY_ENTITY; };


      /*!
       * \brief Send a notice to this recipient
       * 
       * \copydoc sendMessage()
       */
      virtual const Error::error_type
	sendNotice(Sender& sender,
		   const std::wstring& message,
		   const Directivity directivity = Directivity())
	{ return Error::UNSUPPORTED_BY_ENTITY; };
   }; // class Receiver
}; // namespace Kine

# include <kineircd/entity.h>
# include <kineircd/sender.h>

#endif // _INCLUDE_KINEIRCD_RECEIVER_H_
