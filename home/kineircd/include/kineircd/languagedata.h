/* $Id$
 * Language data container (one per language)
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

#ifndef _INCLUDE_KINEIRCD_LANGUAGEDATA_H_
# define _INCLUDE_KINEIRCD_LANGUAGEDATA_H_ 1

# include <string>
# include <vector>

# include <kineircd/languages.h>

namespace Kine {
   class LanguageData {
    public:
      // Tag data type
      typedef std::vector < std::string* > tagData_type;
      
    private:
      std::string languageCode;			// RFC-3066 compliant code
      std::string languageName;			// Name of the language (UTF-8)
      std::string languageNote;			// Optional notice
      std::string maintainer;			// Maintainer of the file
      long fileRevision;			// File revision
      unsigned long tagCount;			// *real* number of tags

      tagData_type tagData;			// The tag data itself!
      
    public:
      // Constructor
      LanguageData(void)
	: fileRevision(0),
          tagCount(0)
	{};
      
      // Destructor
      ~LanguageData(void);

      // Look for a given TID's data, and return it if possible
      const std::string* const 
	findTag(const Languages::tagID_type tagID) const;
      
      // Grab other stuff..
      const std::string& getLanguageCode(void) const
	{ return languageCode; };
      const std::string& getLanguageName(void) const
	{ return languageName; };
      const std::string& getLanguageNote(void) const
	{ return languageNote; };
      const std::string& getMaintainer(void) const
	{ return maintainer; };
      const unsigned long getFileRevision(void) const
	{ return fileRevision; };
      const unsigned long getTagCount(void) const
	{ return tagCount; };
      
      // Friends
      friend struct LanguageConfig;
   };
};

#endif // _INCLUDE_KINEIRCD_LANGUAGEDATA_H_

