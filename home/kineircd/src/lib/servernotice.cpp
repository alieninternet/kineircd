/* servernotice.cpp
 * Little extra stuff to control server notices
 * 
 * Copyright (c) 2001,2002 AustHex Development Team
 * (See DEV-TEAM file for details)
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

#include "autoconf.h"

#include "servernotice.h"

// The number of server notice types - must be in sync with the table below
const unsigned int ServerNotice::NUM_TYPES = 18;

// The server notice type to type-char table, for mode changes and output
struct ServerNotice::typeTableStruct const ServerNotice::typeTable[] = {
     { 'b',	ServerNotice::SN_BURST },
     { 'D', 	ServerNotice::SN_DEBUG },
     { 'G',	ServerNotice::SN_GLOBOPS },
     { 'h',	ServerNotice::SN_HELPME },
     { 'H',	ServerNotice::SN_HTM },
     { 'I',	ServerNotice::SN_IPMISMATCH },
     { 'j',	ServerNotice::SN_NETJOIN_MINOR },
     { 'J',	ServerNotice::SN_NETJOIN_MAJOR },
     { 'k',	ServerNotice::SN_KILL_LOCAL },
     { 'K',	ServerNotice::SN_KILL_REMOTE },
     { 'L',	ServerNotice::SN_LOCOPS },
     { 'n',	ServerNotice::SN_NETSPLIT_MINOR },
     { 'N',	ServerNotice::SN_NETSPLIT_MAJOR },
     { 'o',	ServerNotice::SN_OPERING_LOCAL },
     { 'O',	ServerNotice::SN_OPERING_GLOBAL },
     { 's',	ServerNotice::SN_SIGNON },
     { 'S',	ServerNotice::SN_SIGNOFF },
     { 'z',	ServerNotice::SN_SERVER },
     { 'Z',	ServerNotice::SN_HOUSEKEEPING },
     { 0 }
};
