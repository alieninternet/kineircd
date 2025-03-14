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

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif

#ifdef HAVE_TZSET
# include <ctime>
#endif
#include <kineircd/module.h>
#include <kineircd/daemon.h>
#include <kineircd/libirc2/library.h>

#include "mod_irc2user/module.h"
#include "mod_irc2user/protocolinfo.h"
#include "mod_irc2user/isupport.h"
#include "mod_irc2user/commands.h"
#include "mod_irc2user/language.h"
#ifdef KINE_MOD_IRC2USER_SCHEME_ENABLED
# include "mod_irc2user/scheme.h"
#endif


namespace {
   // Information about this module
   static const Kine::Module::Info info = {
      // Firstly, we list the version/copyright information for this module
      MOD_IRC2USER_NAME_SHORT,
      MOD_IRC2USER_NAME_LONG,
      MOD_IRC2USER_VERSION_MAJOR, 
      MOD_IRC2USER_VERSION_MINOR,
      0,

      // Flags to define how we need to be run
      Kine::Module::Flags::UNIQUE_INSTANCE,

      // Configuration stuff
      0
   };


   class mod_irc2user : public Kine::Module {
    private:
      Kine::mod_irc2user::ProtocolInfo protocolInfo;

    public:
      // Constructor
      mod_irc2user(void)
	{};
      
      // Destructor
      ~mod_irc2user(void) {
	 // Deregister our language tag map
	 Kine::languages().deregisterMap(Kine::mod_irc2user::Language::tagMap);
	 
	 // Deregister the protocol itself
	 Kine::daemon().deregisterProtocol(protocolInfo);
	 
	 // Destroy the ISUPPORT information
	 delete &Kine::mod_irc2user::isupport();
	 
	 // Destroy the dynamic commands table thingy
	 delete &Kine::mod_irc2user::commands();
	 
	 // Deinitialise the IRC-2 library
	 Kine::LibIRC2::deinit();
      };

      // Return the information
      const Kine::Module::Info& getInfo(void) const
	{ return info; };
      
      /* start - Fire up the module
       * Original 04/11/2002 simonb
       */
      bool start(void) {
#ifdef HAVE_TZSET
	 // Make sure the timezone information exists (we need it)
	 tzset();
#endif
	 
	 // Initialise the IRC-2 library
	 if (!Kine::LibIRC2::init()) {
	    return false;
	 }
	 
	 // Initialise the ISUPPORT information
	 Kine::mod_irc2user::ISupport::initInstance();
	 
	 // Attempt to register our protocol to the daemon
	 if (!Kine::daemon().registerProtocol(protocolInfo)) {
	    return false;
	 }
	 
	 // Register our language tag map to the langtags engine..
	 if (!Kine::languages().
	     registerMap(Kine::mod_irc2user::Language::tagMap)) {
	    return false;
	 }
	 
	 // Smile :)
	 return true;
      };
   }; // class mod_irc2user
}; // namespace {anonymous}


// The initialisation function, called by Kine
KINE_MODULE_INIT {
#ifdef KINE_MOD_IRC2USER_SCHEME_ENABLED
   Kine::mod_irc2user::Scheme::initScheme();
#endif 
   
   // Fire up the commands singleton (important to do this before configuring)
   Kine::mod_irc2user::Commands::initInstance();
   
   // Return the module :)
   return new mod_irc2user();
};
