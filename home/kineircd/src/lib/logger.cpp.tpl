[+ AutoGen5 template cpp +]
/* $Id$
 * Logging functions base class
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
 *
 *
[+(dne " * ")+]
 */

#include "autoconf.h"
#include "kineircd/kineircdconf.h"

#include "kineircd/logger.h"

using namespace Kine;


// Our list of masks, and how to translate them around nicely
const Logger::maskTable_type Logger::maskTable[Logger::maskTableSize] = {[+ 
   FOR logger_masks "," +]
     { "[+name+]", '[+char+]', Logger::MASK_[+name+] }[+ ENDFOR logger_masks +]
};
