/* $Id$
 * Logging functions via any file/device
 * 
 * Copyright (c) 2000,2002 Alien Internet Services
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

#include <ctime>

#include "mod_filelog/filelog.h"

using namespace Kine::mod_filelog;


/* FileLog - Generic file logger component constructor
 * Original 18/02/2000 simonb
 * 04/04/2002 simonb - Added debugging info
 */
FileLog::FileLog(Config& c)
  : Kine::Logger(c.getLogMask()),
    config(c)
{
   logFile.open(c.getFilename().c_str(),
		std::ofstream::out | std::ofstream::ate);
}


/* ~FileLog - Generic file logger component destructor
 * Original 18/02/2000 simonb
 * 04/04/2002 simonb - Added debugging info
 */
FileLog::~FileLog(void)
{
   logFile.close();
}


/* logLine - Log a line to the file
 * Original 18/02/2000 simonb
 * 04/04/2002 simonb - Added debugging info
 * Note: This logging output sucks.
 */
void FileLog::logLine(const std::string& str,
		      const Kine::Logger::Mask::type mask)
{
   if (ok()) {
      logFile << '[' << time(NULL) << ':' << mask << "] " << str << std::endl;
   }
}
