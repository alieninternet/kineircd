/* $Id$
 * Config data for mod_filelog
 * 
 * Copyright (c) 2002 Simon Butcher <pickle@alien.net.au>
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

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif

#include "mod_filelog/config.h"

using namespace Kine::mod_filelog;


// Our definition table
const AISutil::ConfigParser::defTable_type Config::definitionTable = {
     {
	"FILENAME", 4,
	  (void*)&Config::defFilename,
	  0,
	  0,
	  0
     },
     {
	"LOGMASK", 3,
	  (void*)&Config::defLogMask,
	  &varHandleLogMask,
	  0,
	  0
     },
     {
	"PREFIXFORMAT", 3,
	  (void*)&Config::defPrefixFormat,
	  &varHandleString,
	  0,
	  0
     },
     {
	0, 0,
	  0,
	  0,
	  0,
	  0
     }
};
