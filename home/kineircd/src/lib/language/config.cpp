/* $Id$
 * Language configuration handler thing
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

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif
#include "kineircd/kineircdconf.h"

#include "language/config.h"
#include "kineircd/languagelist.h"
#include "kineircd/config.h"
#include "debug.h"

using namespace Kine;


/* varHandler - Read configuration parameters and load a given language file
 * Original 22/08/2002 simonb
 */
LIBAISUTIL_CONFIG_VARIABLE_HANDLER(LanguageConfig::varHandler)
{
#ifdef KINE_DEBUG_ASSERT
   // If we were called in error, get cranky
   assert(!defName.empty());
#endif

#ifdef KINE_DEBUG_PSYCHO
   debug("LanguageConfig::defHandler() - Was given '" + defName + '\'');
#endif
   
   // Do the loading..
   return ((dataClass.*((LanguageList AISutil::ConfigData::*)dataVariable)).
	   loadFile(defName, errString));
}
