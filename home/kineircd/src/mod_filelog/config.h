/* $Id$
 * Config data for mod_filelog
 * 
 * Copyright (c) 2002 Alien Internet Services
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

#ifndef _SRC_MOD_FILELOG_CONFIG_H_
# define _SRC_MOD_FILELOG_CONFIG_H_ 1

# include <kineircd/loggerconfig.h>
# include <aisutil/string/string.h>

namespace Kine {
   namespace mod_filelog {
      class Config : public Kine::LoggerConfig {
       public:
	 // The definition table, given to Kine's config parserr
	 static const AISutil::ConfigParser::defTable_type definitionTable;

       private:
	 // Variables..
	 AISutil::String defFilename;
	 AISutil::String defPrefixFormat;
	 
       public:
	 // Constructor
	 Config(void)
	   : defFilename("./ircd.log"),
	     defPrefixFormat("%Y%m%d%H%M%S ")
	   {};
	 
	 // Destructor
	 ~Config(void)
	   {};
	 
	 // Return the variables
	 const AISutil::String& getFilename(void) const
	   { return defFilename; };
	 const AISutil::String& getPrefixFormat(void) const
	   { return defPrefixFormat; };
      };
   }; // namespace mod_filelog
}; // namespace Kine
   
#endif // _SRC_MOD_FILELOG_CONFIG_H_
   
