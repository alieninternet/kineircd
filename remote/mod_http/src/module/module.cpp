/* $Id$
 * 
 * Copyright (c) 2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2003 KineIRCd Development Team
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
# include "config.h"
#endif

#include <kineircd/module.h>
#include <kineircd/daemon.h>
#include <iostream>

#include "module/module.h"
#include "module/protocolinfo.h"


namespace {
   // Information about this module
   static const Kine::Module::Info info = {
      // Firstly, we list the version/copyright information for this module
      "mod_http",
      "HTTP Protocol Module",
      "Copyright (c) 2003 KineIRCd Development Team",
      0, // major version number
      0, // minor version number
      0, // patch level
      0, // extra version info (char*)
      0,
	
      // Flags to define how we need to be run
      Kine::Module::Flags::UNIQUE_INSTANCE,

      // Configuration stuff
      0
   };


   class mod_http : public Kine::Module {
    private:
      Kine::mod_http::ProtocolInfo protocolInfo;
      
    public:
      // Constructor
      mod_http(void)
	{};
      
      // Destructor
      ~mod_http(void)
	{
	   // Deregister the protocol
	   Kine::daemon().deregisterProtocol(protocolInfo);
	};
      
      // Return the information
      const Kine::Module::Info& getInfo(void) const
	{ return info; };
      
      // Start the module
      bool start(void) {
	 // Try to register our protocol info
	 if (!Kine::daemon().registerProtocol(protocolInfo)) {
	    return false;
	 }
	 
	 // Be happy :)
	 return true;
      };
   }; // class mod_http
}; // namespace {anonymous}


// The initialisation function, called by Kine
KINE_MODULE_INIT { return new mod_http(); };
