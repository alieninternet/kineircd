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
# include <kineircd/name.h>
# include <kineircd/client.h>
# include <kineircd/errors.h>
# include <kineircd/languages.h>

namespace Kine {
   class LocalUser;
   
   //! A user (refinement of a client)
   class User : public Client {
    public:
      //! The maximum length of the 'status status' field
      static const std::string::size_type maxStaffStatusLength;
      
    private:
      //! The User's username
      std::string username;
      
      //! Away message. If this is empty, the user should be considered 'here'
      std::string awayMessage;

      //! If the user is a helper, this is a single word defining their status
      std::string staffStatus;
      
      //! Languages in use by the user
      Languages::languageDataList_type languageList;

    protected:
      //! Constructor
      User(const Name& _nickname,
	   const std::string& _username,
	   const std::string& _hostname,
	   const AISutil::Time& _signonTime)
	: Client(_nickname, _hostname, _signonTime),
          username(_username)
	{};

      //! An event called if we are marked away or returning from away
      virtual void doEventAwayToggle(void)
	{};

      //! An event called whenever the language list is modified
      virtual void doEventLanguageChange(void)
	{};
      
      //! An event called when a message was successfully sent to us
      virtual void doEventReceivePrivateMessage(Entity& from,
						const std::string& message)
	{};
      
      //! An event called when a notice was successfully sent to us
      virtual void doEventReceivePrivateNotice(Entity& from,
					       const std::string& message)
	{};

      //! An event called whenever the staff status is changed/turned on or off
      virtual void doEventStaffStatusChange(void)
	{};
      
    public:
      //! Destructor
      virtual ~User(void)
	{};
      
      
      //! Return the users' username / identd reply
      const std::string& getUsername(void) const
	{ return username; };

      
      //! Return the virtual hostname of this user
      virtual const std::string& getVirtualHostname(void) const
	{ return getHostname(); };

      
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
      
      //! Is the user hidden from normal users (i.e. 'invisible')?
      const bool isHidden(void) const
	{ return false; /* <=- tmp */ };
      
      //! Return the away message for this user. If blank, there is none set
      const std::string& getAwayMessage(void) const
	{ return awayMessage; };
      
      //! Check if this user is away
      const bool isAway(void) const
	{ return (!getAwayMessage().empty()); };

      //! Set this user 'away' (with the given string as the reason)
      const Error::error_type setAway(const std::string& reason);
      
      //! Set this user as 'here' (or in IRC terminology, 'UNAWAY' ;)
      const Error::error_type setHere(void); 
 
      
      //! Return the user's staff status
      const std::string& getStaffStatus(void) const
	{ return staffStatus; };
      
      //! Is this user a staff member?
      const bool isStaff(void) const
	{ return (!staffStatus.empty()); };
      
      //! Change a user's staff status. Empty status is equal to setStaffOff()
      const Error::error_type changeStaffStatus(const std::string& status);

      //! Remove the user's staff status, if it was even set
      const Error::error_type setStaffOff(void);
      
      
      //! Return the languages list
      const Languages::languageDataList_type& getLanguageList(void) const
	{ return languageList; };
      
      //! Set the language list to the given list (replaces the list)
      const Error::error_type
	setLanguageList(const Languages::languageDataList_type& languages,
			const bool secret = false);
      
      
      //! If this user is local, this will return the local version of this
      virtual const LocalUser* const getLocalSelf(void) const
	{ return 0; };
      
      // Check if this user is a local user
      const bool isLocalUser(void) const
	{ return (getLocalSelf() != 0); };
      
      
      // Handle the reception of a message
      const Error::error_type
	sendMessage(Entity& from, const std::string& message);
      
      // Handle the reception of a notice
      const Error::error_type
	sendNotice(Entity& from, const std::string& message);
   }; // class User
}; // namespace Kine

# include <kineircd/localuser.h>

#endif // _INCLUDE_KINEIRCD_USER_H_
