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

#ifndef _INCLUDE_KINEIRCD_CLIENT_H_
# define _INCLUDE_KINEIRCD_CLIENT_H_ 1

# include <string>
# include <map>
# include <aisutil/time.h>

# include <kineircd/name.h>
# include <kineircd/denizen.h>
# include <kineircd/sender.h>
# include <kineircd/receiver.h>

namespace Kine {
   class Channel;
   
   /*!
    * \brief A client
    *
    * An abstract class defining a client. A client is a single Entity
    * which normally connects to a server, but is not a server itself, since
    * it cannot provide any additional entities other than itself.
    *
    * Clients may be real people, or automated programs (\e bots). In
    * practise, though, the real people (\e users) outweigh the automated
    * clients vastly. Automated clients which are considered trusted and/or
    * have a specific purpose on the network are usually \e services.
    *
    * Clients have the ability to interact strongly with other entities, since
    * they can query information from servers, become members of channels,
    * and, of course, they have the ability to both send and receive messages
    * (compare this to a Server or a Channel).
    */
   class Client : public Denizen, public Sender, public Receiver {
    public:
      //! A client's nickname, conforming to IRC rules
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
	 
	 // Check if the name is valid, according to client nickname rules
	 const Error::error_type checkValidity(void) const;
      }; // class Name

    private:
      std::wstring hostname;			//!< Client's hostname

      static const wchar_t notDeafGlyph = 0;	//!< Glyph for not being deaf
      wchar_t attentionGlyph;			//!< Listen prefix while deaf

      // The channel list for this client
      typedef std::map < std::string, Channel* const > channels_type;
      channels_type channels;
      
    protected:
      /*!
       * \brief Constructor
       *
       * \param _hostname The hostname the client is connected from
       * \param _signonTime The time the client connected to the network. For
       *    more information, see Kine::Entity::signonTime
       */
      explicit Client(const std::wstring& _hostname,
		      const AIS::Util::Time& _signonTime)
	: Entity(_signonTime),
          hostname(_hostname),
          attentionGlyph(notDeafGlyph)
	{};

      //! An event called when someone (maybe us) has joined a channel
      virtual void doEventChannelJoin(const Channel& channel,
				      const Client& client)
	{};

      //! An event called when someone (maybe us) has parted a channel
      virtual void doEventChannelPart(const Channel& channel,
				      const Client& client,
				      const std::wstring* const reason)
	{};

      //! An event called whenever a channel's topic has changed
      virtual void doEventChannelTopicChange(const Channel& channel,
					     const Denizen& entity)
	{};
      
      //! An event called when a message was successfully sent to a channel
      virtual void doEventReceiveChannelMessage(Sender& from,
						const Channel& to,
						const std::wstring& message)
	{};
      
      //! An event called when a notice was successfully sent to a channel
      virtual void doEventReceiveChannelNotice(Sender& from,
					       const Channel& to,
					       const std::wstring& message)
	{};

    public:
      //! Destructor
      virtual ~Client(void)
	{};
      
      //! Return the client's nickname
      virtual const Name& getNickname(void) const = 0;

      // Return the client's nickname (which is also its unique 'name')
      const Kine::Name& getName(void) const
	{ return getNickname(); };


      //! Return the client's user name
      virtual const std::wstring& getUsername(void) const = 0;


      // Return the client's hostname
      const std::wstring& getHostname(void) const
	{ return hostname; };

      
      //! Return the attention character used whilst being deaf
      const wchar_t getAttentionGlyph(void) const
	{ return attentionGlyph; };
      
      //! Are we 'deaf' to normal (un-prefixed) channel messages?
      const bool hasSelectiveHearing(void) const
	{ return (attentionGlyph != notDeafGlyph); };
      

      //@{
      /*!
       * \brief Quit, with no reason
       * 
       * This is a voluntary action by the client to leave the network.
       * 
       * Unlike kill(), this is not a forceful removal of the client; Rather,
       * the client is simply being innocuously disconnected, at their own
       * behest. If the client must be removed for more truculent reasons,
       * you need to use kill() instead.
       * 
       * \return The status of the operation
       * \retval Kine::Error::NO_ERROR
       *    The client has quit and been deregistered successfully
       * \retval Kine::Error::UNREGISTERED_ENTITY
       *    The client isn't registered, and therefore (obviously) cannot
       *    leave the network and be deregistered
       */
      const Error::error_type quit(void);
      
      /*!
       * \brief Quit, with a \p reason
       * 
       * Use this method if the client has specified a reason for quitting.
       * 
       * \copydoc quit(void)
       * \param reason The given reason for quitting
       * \retval Kine::Error::TEXT_TOO_LONG
       *    The given \p reason is too long and cannot be accepted. If the
       *    client is expecting the quit to continue, you should crop the
       *    \p reason text prior to calling this.
       */
      const Error::error_type quit(const std::wstring reason);


      /*!
       * \brief Kill this client
       * 
       * This will \e kill the client, disconnecting them from the network
       * with the given reason. This indeed sounds somewhat more
       * melodramatic than it really is, since the user is free to return
       * following the kill (they are not banned).
       * 
       * Killing could occur as a responce to a <em>nickname collisions</em>,
       * or simply an \e dead client being removed from the network if the
       * connection has somehow become stale.
       * 
       * This can, of course, be abused by <em>IRC Operators</em> and should
       * be used carefully. Modern client software have reconnection systems
       * in place which can make the forceful removal of a client from the
       * network somewhat useless, so killing an abusing user will probably
       * be futile. Depending on each individual operator's access
       * configuration, and indeed the server's configuration itself,
       * operators may not be able to kill at all, or may only be able to kill
       * clients which are connected locally.
       * 
       * \param killer The entity initiating the execution
       * \param reason The reason for this user being killed. If the reason
       *    comes from a server protocol which appends a \e bang-path style
       *    route for the kill, please strip it from the reason string.
       * \return The result of the kill
       * \retval Kine::Error::NO_ERROR
       *    The client has been killed
       * \retval Kine::Error::PERMISSION_DENIED
       *    The given \p killer is a client, but is not allowed to kill this
       *    client. This may be because the client has some special protection
       *    (such as it's a Service which performs nickname registration),
       *    or the \p killer is not an <em>IRC Operator</em>. If the \p killer
       *    is an <em>IRC Operator</em>, they may not be allowed to kill, or
       *    can only kill clients locally connected (if this client is not
       *    connected to MyServer).
       * \retval Kine::Error::TEXT_TOO_SHORT
       *    The given reason for killing this client is too short to be
       *    considered acceptable. It needs to be lengthened. This is only
       *    returned when the \p killer is another Client
       * \retval Kine::Error::UNREGISTERED_ENTITY
       *    This is caused by two problems. Either the given \p killer is not
       *    registered to the network, and therefore cannot kill clients, or
       *    this client itself is not registered, and cannot be killed!
       */
      const Error::error_type kill(Denizen& killer, const std::wstring& reason)
	{ return Kine::Error::UNKNOWN_ERROR; };
      //@}
   }; // class Client
}; // namespace Kine

# include <kineircd/channel.h>

#endif // _INCLUDE_KINEIRCD_CLIENT_H_
