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
 */

#ifndef _SRC_LIBKINEIRCD_LANGUAGE_SUBST_H_
# define _SRC_LIBKINEIRCD_LANGUAGE_SUBST_H_ 1

namespace Kine {
   namespace Internal {
      namespace LangTags {
	 // The escape character substitution map, used by the preprocessor
	 struct SubstitutionChar {
	    const char from;
	    const char to;
	 } extern const substitutionMap[];

	 // Marker chars, substituted for internal processing of the strings
	 extern const char parameterMarkerChar;
	 extern const char newLineMarkerChar;
      }; // namespace LangTags
   }; // namespace Internal
}; // namespace Kine
   
#endif // _SRC_LIBKINEIRCD_LANGUAGE_SUBST_H_
   
