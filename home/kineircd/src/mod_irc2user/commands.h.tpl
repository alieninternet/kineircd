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
 */

[+(make-header-guard "kine")+]

# include <kineircd/kineircdconf.h>
# include <kineircd/languages.h>
# include <string>
# ifdef KINE_STL_HAS_HASH
#  include <hash_map>
# else
#  include <map>
# endif

# include "mod_irc2user/protocol.h"

namespace Kine {
   namespace mod_irc2user {
      // IRC-2 user commands
      class Commands {
       public:
	 struct Flags { // <=- should be namespace?
	    enum flag_type {
	       NONE			= 0x00000000,
	       CHANGE_IDLE_TIME		= 0x00000001	// Reset user idle time
	    };
	    typedef unsigned int lazy_type;
	 };
	 
	 struct Access { // <=- namespace :(
	    enum access_type {
	       ANYBODY = 0,
	       OPERATOR,
	       GLOBAL_OPERATOR
	    };
	 };
	 
       private:
	 // A table holding the built-in commands we always support
	 struct preInitCommand_type {
	    const char* const commandName;		// Name of the command
	    Protocol::handler_type Protocol::* const
	      handler;					// Handler function
	    const Access::access_type access;		// Required access
	    const bool enabledByDefault;		// Enabled from init?
	    const unsigned char defaultPenalty;		// Initial penalty rate
	    const unsigned char minimumParams;		// Minimum param count
	    const Kine::Languages::tagID_type* const 
	      helpUsage;				// Command usage hint
	    const Kine::Languages::tagID_type* const 
	      helpInfo;					// Detailed help tag
	    const Flags::lazy_type flags;		// Flags
	 } static const preInitCommands[];

       public:
	 // A 'command descriptor' for our commands table.
	 class CommandInfo {
	  private:
	    // Check the access required by a user (in-depth)
	    const bool checkAccess(const User& user) const;
	    
	  public:
	    Protocol::handler_type Protocol::* const
	      handler;					// Handler function
	    const Access::access_type access;		// Required access
	    bool enabled;				// Is enabled?
	    unsigned char penalty;			// Set penalty rate
	    const unsigned char minimumParams;		// Minimum param count
	    const Kine::Languages::tagID_type* const
	      helpUsage;				// Command usage hint
	    const Kine::Languages::tagID_type* const
	      helpInfo;					// Detailed help tag
	    Flags::lazy_type flags;			// Flags
	    mutable unsigned long long callCount;	// # times used
	    
	    // Copy constructor to aid in initialisation
	    CommandInfo(const preInitCommand_type& info)
	      : handler(info.handler),
	        access(info.access),
	        enabled(info.enabledByDefault),
	        penalty(info.defaultPenalty),
	        minimumParams(info.minimumParams),
	        helpUsage(info.helpUsage),
	        helpInfo(info.helpInfo),
	        flags(info.flags),
	        callCount(0)
	      {};
	    
	    // Check if a user has access to this command (inline for speed)
	    inline const bool hasAccess(const User& user) const
	      {
	         return ((access == Access::ANYBODY) ?
	              	 true : checkAccess(user));
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
	 const CommandInfo* const findCommand(const std::string& name) const;
	 
	 // Return the command list (read-only)
	 const commandList_type& getCommandList(void) const
	   { return commandList; };
      };
      
      
      // Lazy handy function to return the instance of the above singleton
      static inline Commands& commands(void)
	{ return Commands::getInstance(); };
   }; // namespace mod_irc2user
}; // namespace Kine


// These defines help determine what commands are available[+FOR commands+]
# define KINE_MOD_IRC2USER_HAVE_CMD_[+
   (string-upcase
      (get "command"))
 +] 1[+ENDFOR+]


#endif // [+(. header-guard)+]
   
