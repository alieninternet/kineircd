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

#ifndef _INCLUDE_KINEIRCD_USER_H_
# define _INCLUDE_KINEIRCD_USER_H_ 1

# include <string>
# include <kineircd/client.h>
# include <kineircd/errors.h>
# include <kineircd/languages.h>

namespace Kine {
   //! A user (refinement of a client)
   class User : public Client {
    public:
      //! The maximum length of the 'status status' field
      static const std::string::size_type maxStaffStatusLength;
      
    private:
      //!< User's nickname
      ClientName nickname;
      
      //! The User's username
      std::string username;
      
      //! Away message. If this is empty, the user should be considered 'here'
      std::string awayMessage;

      //! Away time - the time the user was last marked as being 'away'
      AIS::Util::Time awaySince;
      
      //! If the user is a helper, this is a single word defining their status
      std::string staffStatus;
      
      //! Languages in use by the user
      Languages::languageDataList_type languageList;

    protected:
      //! Constructor
      explicit User(const ClientName& _nickname,
		    const std::string& _username,
		    const std::string& _hostname,
		    const AIS::Util::Time& _signonTime)
	: Client(_hostname, _signonTime),
          nickname(_nickname),
          username(_username)
	{};

      //! An event called if we are marked away or returning from away
      virtual void doEventAwayToggle(const Denizen& changer)
	{};

      //! An event called whenever the language list is modified
      virtual void doEventLanguageChange(const Denizen& changer)
	{};

      //! An event called whenever a user's nickname has been changed (maybe us)
      virtual void doEventNicknameChange(const Denizen& changer,
					 const User& user,
					 const ClientName& newNickname)
	{};
      
      //! An event called when a message was successfully sent to us
      virtual void doEventReceivePrivateMessage(Sender& from,
						const std::string& message)
	{};

      //! An event called when a notice was successfully sent to us
      virtual void doEventReceivePrivateNotice(Sender& from,
					       const std::string& message)
	{};

      //! An event called whenever the staff status is changed/turned on or off
      virtual void doEventStaffStatusChange(const Denizen& changer)
	{};
      
    public:
      //! Destructor
      virtual ~User(void)
	{};
      
      
      // Return the user's nickname
      const ClientName& getNickname(void) const
	{ return nickname; };

      
      // Return the user's username / identd reply
      const std::string& getUsername(void) const
	{ return username; };

      
      //! Return the virtual hostname of this user
      virtual const std::string& getVirtualHostname(void) const
	{ return getHostname(); };

      //! Return true if the hostname or IP should be hidden from the given user
      const bool hideHostFrom(const User& user) const
	{ return false; /* fix me */ };

      
      //! Change this user's nickname
      const Error::error_type changeNickname(const Denizen& changer,
					     const ClientName& newNickname);
      

      //! Return the modes/flags bitmask
//      const ? getModes(void) const
//	{ return modes; };
      
      //! Check to see if a particular mode/flag is set on this user
//      const bool isSet( ? ) const
//	{ return (modes & ?); };
	
      //! Handy function to see if the user is a global operator
      const bool isGlobalOperator(void) const
	{ return false; };
      
      //! Handy function to see if the user is a local operator
      const bool isLocalOperator(void) const
	{ return false; };

      //! Is the user an operator (may be local or global)?
      const bool isOperator(void) const
	{ return (isGlobalOperator() || isLocalOperator()); };
      

      //! Return the away message for this user. If blank, there is none set
      const std::string& getAwayMessage(void) const
	{ return awayMessage; };

      //! Return the time the user was last set as being away
      const AIS::Util::Time& getAwaySince(void) const
	{ return awaySince; };
      
      //! Check if this user is away
      const bool isAway(void) const
	{ return (!getAwayMessage().empty()); };

      //! Set this user 'away' (with the given string as the reason)
      const Error::error_type setAway(const Denizen& changer,
				      const std::string& reason);

      //! Set this user as 'here' (or in IRC terminology, 'UNAWAY' ;)
      const Error::error_type setHere(const Denizen& changer); 
 
      
      //! Return the user's staff status
      const std::string& getStaffStatus(void) const
	{ return staffStatus; };
      
      //! Is this user a staff member?
      const bool isStaff(void) const
	{ return (!staffStatus.empty()); };
      
      //! Change a user's staff status. Empty status is equal to setStaffOff()
      const Error::error_type changeStaffStatus(const Denizen& changer,
						const std::string& status);

      //! Remove the user's staff status, if it was even set
      const Error::error_type setStaffOff(const Denizen& changer);
      
      
      //! Return the languages list
      const Languages::languageDataList_type& getLanguageList(void) const
	{ return languageList; };
      
      //! Set the language list to the given list (replaces the list)
      const Error::error_type
	setLanguageList(const Denizen& changer,
			const Languages::languageDataList_type& languages,
			const bool secret = false);
      

      // Handle the reception of a message
      const Error::error_type
	sendMessage(Sender& from, const std::string& message,
		    const Receiver::Directivity directivity =
		    Receiver::Directivity());
      
      // Handle the reception of a notice
      const Error::error_type
	sendNotice(Sender& from, const std::string& message,
		   const Receiver::Directivity directivity =
		   Receiver::Directivity());
   }; // class User
}; // namespace Kine

#endif // _INCLUDE_KINEIRCD_USER_H_
