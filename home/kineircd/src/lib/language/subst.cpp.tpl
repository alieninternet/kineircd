[+ AutoGen5 template cpp +]
/* $Id$
 * 
 * Copyright (c) 2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2003 KineIRCd Development Team
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
 *
 *
[+(dne " * ")+]
 */

#include "libkineircd/language/subst.h"

using namespace Kine::Internal;
 

// Substitution map for the langtags preprocessor
const LangTags::SubstitutionChar LangTags::substitutionMap[] = {
   { 'n',	newLineMarkerChar },	// New line/section marker char
   { 's',	'\040' },		// Force a space
   { 't',	'\011' },		// Horizontal tab
   { '\\',	'\\' },			// Slash - copy directly[+FOR chars+]
   { '[+langtag_code+]',	'[+char+]' },		// [+name+][+ENDFOR+]
   { 0, 0 }
};


// Marker chars
const char LangTags::parameterMarkerChar =
  '\000'; // NUL
const char LangTags::newLineMarkerChar =
  '\001'; // SOH
