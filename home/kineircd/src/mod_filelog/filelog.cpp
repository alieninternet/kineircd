/* $Id$
 * Logging functions via any file/device
 * 
 * Copyright (c) 2000,2002 Simon Butcher <pickle@alien.net.au>
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
#include <iomanip>
#include <kineircd/daemon.h>

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
		std::ofstream::out | std::ofstream::app);
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
      struct tm currentTime;
      
      // Break up the time, depending on what timezone setting the user has
      if (config.getUTC()) {
	 currentTime = *gmtime(&daemon().getTime().tv_sec);
      } else {
	 currentTime = *localtime(&daemon().getTime().tv_sec);
      }

      // Process the format and output the line
      for (std::string::size_type i = 0; 
	   i != config.getPrefixFormat().length(); i++) {
	 // Is this the start of a conversion specifier?
	 if (config.getPrefixFormat()[i] == '%') {
	    // Make sure the next char is not at the end of the line..
	    if (++i >= config.getPrefixFormat().length()) {
	       break;
	    }
	    
	    // What is needed here?
	    switch (config.getPrefixFormat()[i]) {
	     case '!':	// The character corresponding with the log mask
		 {
		    const Logger::MaskMapper& mapper = getMaskMapper(mask);
		    logFile << mapper.character;
		    continue;
		 }
	       
	     case '#':	// The name of what is being logged
		 {
		    const Logger::MaskMapper& mapper = getMaskMapper(mask);
		    logFile << mapper.name;
		    continue;
		 }
	       
	     case '@':	// The name of what is being logged, upper-cased
		 {
		    const Logger::MaskMapper& mapper = getMaskMapper(mask);
		    logFile << mapper.nameUpper;
		    continue;
		 }
	       
	     case '$':	// The log string
	       logFile << str;
	       continue;
	       
	     case '%':	// A percentage sign :)
	       logFile << '%';
	       continue;
	       
	     case 'C':	// Century (2 digits)
	       logFile << ((currentTime.tm_year + 1900) / 100);
	       continue;
	       
	     case 'd':	// Day of the month # (01..31, 0 pre-pad)
	       logFile << std::setfill('0') << 
		 std::setw(2) << currentTime.tm_mday;
	       continue;
	       
	     case 'e':	// Day of the month # ( 1..31, space pre-pad)
	       logFile << std::setfill(' ') << 
		 std::setw(2) << currentTime.tm_mday;
	       continue;
	       
	     case 'H':	// Hour (24-hour clock; 00..23, 0 pre-pad)
	       logFile << std::setfill('0') << 
		 std::setw(2) << currentTime.tm_hour;
	       continue;
	       
	     case 'I':	// Hour (12-hour clock; 01..12, 0 pre-pad)
	       logFile << std::setfill('0') << 
		 std::setw(2) << ((currentTime.tm_hour < 12) ? 
				  (currentTime.tm_hour + 1) : 
				  (currentTime.tm_hour - 12));
	       continue;
	       
	     case 'j':	// Day of the year as a number (001..366, 0 pre-pad)
	       logFile << std::setfill('0') << 
		 std::setw(3) << currentTime.tm_yday;
	       continue;
	       
	     case 'k':	// Hour (24-hour clock, space pre-pad)
	       logFile << std::setfill(' ') << 
		 std::setw(2) << currentTime.tm_hour;
	       continue;
	       
	     case 'l':	// Hour (12-hour clock, space pre-pad)
	       logFile << std::setfill(' ') << 
		 std::setw(2) << ((currentTime.tm_hour < 12) ? 
				  (currentTime.tm_hour + 1) : 
				  (currentTime.tm_hour - 12));
	       continue;
	       
	     case 'm':	// Month # (01..12, 0 pre-pad)
	       logFile << std::setfill('0') << 
		 std::setw(2) << (currentTime.tm_mon + 1);
	       continue;
	       
	     case 'M':	// Minute (00..59, 0 pre-pad)
	       logFile << std::setfill('0') << 
		 std::setw(2) << currentTime.tm_min;
	       continue;
	       
	     case 'n':	// New line
	       logFile << '\n';
	       continue;
	       
	     case 'p':	// 'AM' or 'PM' (noon is pm, midnight is am)
	       logFile << ((currentTime.tm_hour < 12) ? "AM" : "PM");
	       continue;
	       
	     case 'P':	// 'am' or 'pm' (noon is pm, midnight is am)
	       logFile << ((currentTime.tm_hour < 12) ? "am" : "pm");
	       continue;
	       
	     case 'r':	// Equivalent to %I:%M:%S %p
	       logFile << std::setfill('0') <<
		 std::setw(2) << ((currentTime.tm_hour < 12) ? 
				  (currentTime.tm_hour + 1) :
				  (currentTime.tm_hour - 12)) << ':' <<
		 std::setw(2) << currentTime.tm_min << ':' <<
		 std::setw(2) << currentTime.tm_sec << ' ' <<
		 ((currentTime.tm_hour < 12) ? "AM" : "PM");
	       continue;
	       
	     case 'R':	// Equivalent to %H:%M
	       logFile << std::setfill('0') << 
		 std::setw(2) << currentTime.tm_hour << ':' <<
		 std::setw(2) << currentTime.tm_min;
	       continue;
	       
	     case 's':	// Number of seconds since the 'unix epoch'
	       logFile << daemon().getTime().tv_sec;
	       continue;
	       
	     case 'S':	// Number of seconds (00..61, 0 pre-pad)
	       logFile << std::setfill('0') << 
		 std::setw(2) << currentTime.tm_sec;
	       continue;
	       
	     case 't':	// A tab character
	       logFile << '\t';
	       continue;
	       
	     case 'T':	// Equivalent to %H:%M:%S
	       logFile << std::setfill('0') << 
		 std::setw(2) << currentTime.tm_hour << ':' <<
		 std::setw(2) << currentTime.tm_min << ':' <<
		 std::setw(2) << currentTime.tm_sec;
	       continue;
	       
	     case 'u':	// Number of the day of the week (1..7)
	       logFile << (currentTime.tm_wday + 1);
	       continue;
	       
	     case 'w':	// Number of the day of the week (0..6)
	       logFile << currentTime.tm_wday;
	       continue;
	       
	     case 'y':	// Year without the century (00..99, 0 pre-pad)
	       logFile << std::setfill('0') << 
		 std::setw(2) << (currentTime.tm_year % 100);
	       continue;
	       
	     case 'Y':	// Year with the century
	       logFile << (currentTime.tm_year + 1900);
	       continue;
	       
	     default:	// Unknown.. we skip over it..
	       continue;
	    }
	 }
	 
	 // Oh well, copy the char flat.
	 logFile << config.getPrefixFormat()[i];
      }
      
      // Flush the output..
      logFile << std::flush;
   }
}
