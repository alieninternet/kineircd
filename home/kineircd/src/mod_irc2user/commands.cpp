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

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif

#include <cassert>

#include "mod_irc2user/commands.h"

using namespace Kine::mod_irc2user;


// The pointer to our single instance
Commands* Commands::instance = 0;


/* Commands - Constructor for the command list managing singleton
 * Original 01/04/2003 simonb
 */
Commands::Commands(void)
{
   /* Migrate the data held in our pre-initialisation table into the dynamic
    * table, ready to be modified possibly by configuration, or added to by
    * aliases and so forth
    */
   for (unsigned int i = 0; preInitCommands[i].commandName != 0; i++) {
      // Insert the data into the dynamic command table
      commandsList.
	insert(commandsList_type::value_type(preInitCommands[i].commandName,
					     preInitCommands[i]));
   }
}


/* initInstance - Create the single instance of this class
 * Original 01/04/2003 simonb
 */
void Commands::initInstance(void)
{
   // Make sure we don't already exist - big woops if we do
   assert(instance == 0);
   
   // Okay, create ourselves then
   instance = new Commands();
}


/* findCommand - Find the given command, and return any info we have (if any)
 * Original 01/04/2003 simonb
 */
const Commands::CommandInfo* const
  Commands::findCommand(const std::string& name)
{
   // Look for the command..
   commandsList_type::iterator it = commandsList.find(name);
   
   // Did we find it?
   if (it != commandsList.end()) {
      // We found it!! Since we did find it, increase the call count
      (*it).second.callCount++;

      // Return a pointer to this command's information
      return &((*it).second);
   }
   
   // We didn't find the command.. oh well..
   return 0;
}
