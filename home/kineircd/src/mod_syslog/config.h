/* $Id$
 * 
 * Copyright (c) 2002,2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2002,2003 KineIRCd Development Team
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

#ifndef _SRC_MOD_SYSLOG_CONFIG_H_
# define _SRC_MOD_SYSLOG_CONFIG_H_ 1

# include <kineircd/logger.h>
# include <aisutil/string/string.h>

namespace Kine {
   namespace mod_syslog {
      // Config data for mod_syslog
      class Config : public Kine::Logger::Config {
       public:
	 // The definition table, given to Kine's config parserr
	 static const AIS::Util::ConfigParser::defTable_type definitionTable;

       private:
	 // Variables..
	 bool defConsoleOutput;
	 AIS::Util::String defIdentity;
	 bool defShowPID;
	 
       public:
	 // Constructor
	 Config(void)
	   : defConsoleOutput(false),
	     defIdentity("kineircd"),
	     defShowPID(true)
	   {};
	 
	 // Destructor
	 ~Config(void)
	   {};
	 
	 // Return the variables
	 const bool getConsoleOutput(void) const
	   { return defConsoleOutput; };
	 const AIS::Util::String& getIdentity(void) const
	   { return defIdentity; };
	 const bool getShowPID(void) const
	   { return defShowPID; };
      };
   }; // namespace mod_syslog
}; // namespace Kine
   
#endif // _SRC_MOD_SYSLOG_CONFIG_H_
