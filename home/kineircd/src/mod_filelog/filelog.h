/* loggerfile.h
 * Logging functions via a given file
 * 
 * Copyright (c) 2000,2002 Alien Internet Services
 *
 * This file is a part of AustHex.
 * 
 * AustHex is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * AustHex is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with AustHex; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __LOGGERFILE_H__
# define __LOGGERFILE_H__

# include <fstream>

# include "logger.h"


// The generic file logging class
class LoggerFile : public Logger {
 private:
   ofstream logFile;
   
   void logLine(const String &, 
		const mask_type = MASK_HOUSEKEEPING);	// Log a string of text
   
 public:
   LoggerFile(const char *);				// Constructor
   ~LoggerFile(void);					// Destructor

   // Is the log ok?
   bool ok(void) const
     { return logFile.good(); };
};

#endif
