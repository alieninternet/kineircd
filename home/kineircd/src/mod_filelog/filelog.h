/* $Id$
 * 
 * Copyright (c) 2000,2002,2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2000,2002,2003 Alien Internet Services
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

#ifndef _SRC_MOD_FILELOG_LOGGER_H_
# define _SRC_MOD_FILELOG_LOGGER_H_ 1

# include <fstream>
# include <kineircd/logger.h>

# include "mod_filelog/config.h"

namespace Kine {
   namespace mod_filelog {
      // The generic file logging class
      class FileLog : public Kine::Logger {
       private:
	 // The configuration
	 mod_filelog::Config& config;

	 // The log file
	 std::ofstream logFile;
	 
	 // Log a string of text
	 void logLine(const std::string& str,
		      const Kine::Logger::Mask::type mask);
	 
       public:
	 // Constructor
	 FileLog(mod_filelog::Config& c);
	 
	 // Destructor
	 ~FileLog(void);				
	 
	 // Is the log ok?
	 bool ok(void) const
	   { return logFile.good(); };
      };
   }; // namespace mod_filelog
}; // namespace Kine
   
#endif // _SRC_MOD_FILELOG_LOGGER_H_
   
