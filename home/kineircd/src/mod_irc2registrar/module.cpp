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

#include <kineircd/module.h>
#include <kineircd/daemon.h>
#include <kineircd/irc2/library.h>

#include "mod_irc2registrar/module.h"
#include "mod_irc2registrar/protocolinfo.h"
#include "mod_irc2registrar/language.h"


namespace {
   // Information about this module
   static const Kine::Module::Info info = {
      // Firstly, we list the version/copyright information for this module
      MOD_IRC2REGISTRAR_NAME_SHORT,
      MOD_IRC2REGISTRAR_NAME_LONG,
      MOD_IRC2REGISTRAR_VERSION_MAJOR, 
      MOD_IRC2REGISTRAR_VERSION_MINOR,
      0,

      // Flags to define how we need to be run
      Kine::Module::Flags::UNIQUE_INSTANCE,

      // Configuration stuff
      0
   };


   class mod_irc2registrar : public Kine::Module {
    private:
      Kine::mod_irc2registrar::ProtocolInfo protocolInfo;
      
    public:
      // Constructor
      mod_irc2registrar(void)
	{};
      
      // Destructor
      ~mod_irc2registrar(void) {
	 // Deregister our language tag map
	 Kine::languages().
	   deregisterMap(Kine::mod_irc2registrar::Language::tagMap);
	 
	 // Deregister the protocol
	 Kine::daemon().deregisterProtocol(protocolInfo);
	 
	 // Deinitialise the IRC-2 library
	 Kine::LibIRC2::deinit();
      };

      // Return the information
      const Kine::Module::Info& getInfo(void) const
	{ return info; };
      
      /* start - Fire up the module
       * Original 03/04/2003 simonb
       */
      bool start(void) {
	 // Initialise the IRC-2 library
	 if (!Kine::LibIRC2::init()) {
	    return false;
	 }
	 
	 // Try to register our protocol information to the core
	 if (!Kine::daemon().registerProtocol(protocolInfo)) {
	    return false;
	 }
	 
	 // Attempt to register our language tag map
	 if (!Kine::languages().
	     registerMap(Kine::mod_irc2registrar::Language::tagMap)) {
	    return false;
	 }
	 
	 // Be all happy and stuff
	 return true;
      };
   }; // class mod_irc2registrar
}; // namespace {anonymous}


// The initialisation function, called by Kine
KINE_MODULE_INIT { return new mod_irc2registrar(); };
