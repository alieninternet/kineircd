/* $Id$
 * IRC-2 user commands
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

#ifndef _SRC_MOD_IRC2USER_COMMANDS_H_
# define _SRC_MOD_IRC2USER_COMMANDS_H_ 1

# include <kineircd/kineircdconf.h>
# include <kineircd/languages.h>
# include <string>
# ifdef KINE_STL_HAS_HASH
#  include <hash_map>
# else
#  include <map>
# endif

# include "mod_irc2user/access.h"
# include "mod_irc2user/protocol.h"

namespace Kine {
   namespace mod_irc2user {
      class Commands {
       private:
	 // A table holding the built-in commands we always support
	 struct preInitCommand_type {
	    const char* const commandName;		// Name of the command
	    Protocol::handler_type Protocol::* const
	      handler;					// Handler function
	    Access::function_type* const accessChecker;	// Access checker
	    const bool enabledByDefault;		// Enabled from init?
	    const unsigned char defaultPenalty;		// Initial penalty rate
	    const unsigned char minimumParams;		// Minimum param count
	    const Kine::Languages::tagID_type* const 
	      helpUsage;				// Command usage hint
	    const Kine::Languages::tagID_type* const 
	      helpInfo;					// Detailed help tag
	 } static const preInitCommands[];

       public:
	 // A 'command descriptor' for our commands table.
	 struct CommandInfo {
	    Protocol::handler_type Protocol::* const
	      handler;					// Handler function
	    Access::function_type* accessChecker;	// Access checker
	    bool enabled;				// Is enabled?
	    unsigned char penalty;			// Set penalty rate
	    const unsigned char minimumParams;		// Minimum param count
	    const Kine::Languages::tagID_type* const
	      helpUsage;				// Command usage hint
	    const Kine::Languages::tagID_type* const
	      helpInfo;					// Detailed help tag
	    unsigned long long callCount;		// # times used
	    
	    // Copy constructor to aid in initialisation
	    CommandInfo(const preInitCommand_type& info)
	      : handler(info.handler),
	        accessChecker(info.accessChecker),
	        enabled(info.enabledByDefault),
	        penalty(info.defaultPenalty),
	        minimumParams(info.minimumParams),
	        helpUsage(info.helpUsage),
	        helpInfo(info.helpInfo),
	        callCount(0)
	      {};
	    
	    // Convenient access checking thing :)
	    const bool hasAccess(const User& user) const
	      {
		 if (accessChecker == 0) {
		    return true;
		 }
		 return (accessChecker)(user);
	      };
	 };
	 
	 // Commands list type
# ifdef KINE_STL_HAS_HASH
	 typedef std::hash_map < std::string, CommandInfo > commandList_type;
# else
	 typedef std::map < std::string, CommandInfo > commandList_type;
# endif
	 
       private:
	 // A list of commands
	 commandList_type commandList;
	 
	 // Our single instance
	 static Commands* instance;
	 
	 // Constructor
	 Commands(void);
	 
       public:
	 // Destructor
	 ~Commands(void)
	   {};
	 
	 // Create the single instance of this class
	 static void initInstance(void);
	 
	 // Grab the single instance of ourself
	 static Commands& getInstance(void)
	   { return *instance; };
	 
	 // Find a command in the command list
	 const CommandInfo* const findCommand(const std::string& name);
	 
	 // Return the command list (read-only)
	 const commandList_type& getCommandList(void) const
	   { return commandList; };
      };
   }; // namespace mod_irc2user
}; // namespace Kine
   
#endif // _SRC_MOD_IRC2USER_COMMANDS_H_
   
