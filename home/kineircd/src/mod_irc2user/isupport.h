/* $Id$
 * 
 * Copyright (c) 2001,2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2001,2003 KineIRCd Development Team
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

#ifndef _SRC_MOD_IRC2USER_ISUPPORT_H_
# define _SRC_MOD_IRC2USER_ISUPPORT_H_ 1

# include <string>
# include <vector>

namespace Kine {
   namespace mod_irc2user {
      // IRC-2 RPL_ISUPPORT information
      class ISupport {
       public:
	 // The type of the isupport information list
	 typedef std::vector < std::string > info_type;
	 
       private:
	 // Pre-initialisation table (static stuff we always know about)
	 static const char* const preInitInfo[];
	 
	 // Our ISUPPORT information
	 info_type info;
	 
	 // Our single instance
	 static ISupport* instance;
	 
	 // Constructor
	 ISupport(void)
	   { initInfo(); };

	 // Initialise or reinitialise the ISUPPORT info
	 void initInfo(void);
	 
       public:
	 // Destructor
	 ~ISupport(void)
	   {};
	 
	 // Create the single instance of this class
	 static void initInstance(void);
	 
	 // Grab the single instance of ourself
	 static ISupport& getInstance(void)
	   { return *instance; };
	 
	 // Return the information
	 const info_type& getInfo(void) const
	   { return info; };
      };
      
      
      // Lazy handy function to return the instance of the above singleton
      static inline ISupport& isupport(void)
	{ return ISupport::getInstance(); };
   }; // namespace mod_irc2user
}; // namespace Kine

#endif // _SRC_MOD_IRC2USER_ISUPPORT_H_
