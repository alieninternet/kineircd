/* $Id$
 * Debugging support code
 * 
 * Copyright (c) 2002 KineIRCd Development Team
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

#include "kineircd/kineircdconf.h"

#include <iostream>

#include "debug.h"
//#include "kineircd/daemon.h"

// Little check to make sure we are being compiled sanely
#ifndef KINE_DEBUG
# warning "Compiling without DEBUG defined?!"
#endif


/* debug - Output a line of debugging information (and broadcast)
 * Original 25/09/01 simonb
 */
void debug(LibAIS::String const &line)
{
   // Output it to stderr
   std::cerr << line << std::endl;
   
//   // Also, log it
//   Daemon::logger(line, Logger::MASK_DEBUG);
}
