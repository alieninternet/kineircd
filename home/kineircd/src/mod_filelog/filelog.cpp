/* file.cpp
 * Logging functions via any file/device
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

#include "austhex/austhexconf.h"

#include <ctime>

#include "austhex/loggerfile.h"
#include "austhex/debug.h"


/* LoggerFile - Generic file logger component constructor
 * Original 18/02/2000 simonb
 * 04/04/2002 simonb - Added debugging info
 */
LoggerFile::LoggerFile(mask_type mask, const char *fileName)
  : Logger(Logger::TYPE_FILE, mask)
{
#ifdef DEBUG_EXTENDED
   debug(String("LoggerFile::LoggerFile(): logFile.open(") + fileName + ')');
#endif
   
   logFile.open(fileName);
}


/* ~LoggerFile - Generic file logger component destructor
 * Original 18/02/2000 simonb
 * 04/04/2002 simonb - Added debugging info
 */
LoggerFile::~LoggerFile(void)
{
#ifdef DEBUG_EXTENDED
   debug("LoggerFile::~LoggerFile(): logFile.close()");
#endif
   
   logFile.close();
}


/* logLine - Log a line to the file
 * Original 18/02/2000 simonb
 * 04/04/2002 simonb - Added debugging info
 * Note: This logging output sucks.
 */
void LoggerFile::logLine(const String &line, const mask_type mask)
{
   if (ok()) {
#ifdef DEBUG_PSYCHO
      debug("LoggerFile::log(" + line + ')');
#endif

      logFile << '[' << time(NULL) << ':' << mask << "] " << line << endl;
   }
}
