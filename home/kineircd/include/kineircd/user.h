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
   //! A user (refinement of a client)
   class User : public Client {
    private:
      Name nickname;				//!< User's nickname
      std::string username;			//!< User's username
      std::string hostname;			//!< User's hostname
      std::string awayMessage;			//!< Away message (empty = off)

      //! Languages in use by the user
      Languages::languageDataList_type languageList;
      
    protected:
      //! Constructor
      User(const Name& _nickname,
	   const std::string& _username,
	   const std::string& _hostname,
	   const AISutil::Time& _signonTime)
	: Client(_signonTime),
          nickname(_nickname),
          username(_username),
          hostname(_hostname)
	{};

      //! An event called if we are marked away or returning from away
      virtual void doEventAwayToggle(void) {};

      //! An event called whenever the language list is modified
      virtual void doEventLanguageChange(void) {};
      
    public:
      //! Destructor
      virtual ~User(void)
	{};
      
      //! Return the users' nickname
      const Name& getNickname(void) const
	{ return nickname; };

      //! Return the users' username / identd reply
      const std::string& getUsername(void) const
	{ return username; };

      //! Return the users' hostname
      const std::string& getHostname(void) const
	{ return hostname; };

      //! Return the virtual hostname of this user
      virtual const std::string& getVirtualHostname(void) const
	{ return getHostname(); };
      
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
      
      //! Return the languages list
      const Languages::languageDataList_type& getLanguageList(void) const
	{ return languageList; };
      
      //! Set the language list to the given list (replaces the list)
      const Error::error_type
	setLanguageList(const Languages::languageDataList_type& languages,
			const bool secret = false);
   }; // class User
}; // namespace Kine

#endif // _INCLUDE_KINEIRCD_USER_H_
