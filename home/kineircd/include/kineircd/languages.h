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

#ifndef _INCLUDE_KINEIRCD_LANGUAGES_H_
# define _INCLUDE_KINEIRCD_LANGUAGES_H_ 1

# include <kineircd/kineircdconf.h>

# include <set>
# ifdef KINE_STL_HAS_HASH
#  include <hash_map>
# else
#  include <map>
# endif
# include <string>
# include <vector>

namespace Kine {
   //! Languages interface (langtags)
   class Languages {
    public:
      /* The tag identifier type (also referred to as a 'TID').
       * Note that TID #0 has a special purpose -- "unknown".
       */
      typedef unsigned int tagID_type;
      
      /* This is the 'tag map' structure. In order for your module to know
       * the correlation of tag name to tag ID upon run-time, you need to
       * define this structure within your module and register it with the
       * registerMap() function (defined below). Once registered, and after
       * languages have been loaded (or reloaded), we will attempt to form
       * the link between a tag name and its run-time ID (a number) using
       * this map. Using the number set within this structure, you can then
       * look up your tags. If the tag name cannot be found (i.e. it does
       * not exist within any loaded language files) the tag ID will be set
       * to 0, and an empty string will be returned whenever you try to
       * reference that tag's data.
       * 
       * You do not need to fill in the tag ID field, only the tag name. Both
       * upper and lower case is acceptable within the tag name, since the
       * lookup performed uses case-insensitive checks.
       * 
       * Terminate the map array with a null tagName (0).
       */
      struct TagMapEntry {
	 const char* const tagName;
	 tagID_type tagID;
      };
      typedef TagMapEntry tagMap_type[];

      //! The type of a 'parameter' (used in substitutions)
      typedef std::string parameter_type;
      
      /* The type of a 'parameter list'. Parameter lists are sent along side
       * a request to return language data in order to substitute parameter
       * tokens within the language data itself with their appropriate values
       * prior to being returned back to the software.
       */
      typedef std::vector < const parameter_type* > parameterList_type;

      /* The call-out function used for multi-call get's.
       * Inherit from this, and use the operator() to do whatever you need
       * with each call. Each call is given a string, and must return a true
       * if you are willing to accept more data, or false to stop iterating
       * through the string and generating calls.
       */
      struct callFunction_type {
	 callFunction_type(void) {};
	 virtual ~callFunction_type(void) {};
	 virtual bool operator()(const std::string& data) = 0;
      };
      
      /* Language data class, this holds information about the language, along
       * with the tag data. It also provides basic look-up functions per
       * language
       */
      class LanguageData {
       public:
	 //! Tag data type
	 typedef std::vector < std::string* > tagData_type;
	 
       private:
	 std::string languageCode;		//!< RFC-3066 compliant code
	 std::string languageName;		//!< Name of the language
	 std::string languageNote;		//!< Optional notice
	 std::string maintainer;		//!< Maintainer of the file
	 long fileRevision;			//!< File revision
	 unsigned long tagCount;		//!< *real* number of tags
	 
	 tagData_type tagData;			//!< The tag data itself!

	 //! Merge the given language data "over" this language data
	 void mergeWith(const LanguageData& newData);

	 //! Process the given string (deal with substitutions and so forth)
	 const std::string
	   process(const std::string& data,
		   const parameterList_type* const parameters) const;
	 
	 //! Look for a given TID's data, and return it if possible
	 const std::string
	   get(const tagID_type tagID,
	       const parameterList_type* const parameters = 0) const;

	 //! Look for a given TID's data, and call the given function as needed
	 bool get(const tagID_type tagID,
		  callFunction_type& callFunction,
		  const parameterList_type* const parameters = 0) const;

       public:
	 //! Constructor
	 LanguageData(void)
	   : fileRevision(0),
	     tagCount(0)
	   {};
	 
	 //! Destructor
	 ~LanguageData(void);
	 
	 //! Return the language code (BCP-0047) related to the data
	 const std::string& getLanguageCode(void) const
	   { return languageCode; };
	 
	 //! Return the name of the language, as said in its own language
	 const std::string& getLanguageName(void) const
	   { return languageName; };
	 
	 //! Return a note, if any, associated with this language
	 const std::string& getLanguageNote(void) const
	   { return languageNote; };
	 
	 //! Return the contact address (email/url/etc) of a maintainer, if any
	 const std::string& getMaintainer(void) const
	   { return maintainer; };
	 
	 //! Return the unique revision number of this data
	 const unsigned long getFileRevision(void) const
	   { return fileRevision; };
	 
	 //! Return the real number of tags contained within this data
	 const unsigned long getTagCount(void) const
	   { return tagCount; };
	 
	 // Friends
	 friend struct Languages;
      };

      //! The language data list type (as used internally)
# ifdef KINE_STL_HAS_HASH
      typedef std::hash_map < std::string, LanguageData* >
	languageDataMap_type;
# else
      typedef std::map < std::string, LanguageData* >
	languageDataMap_type;
# endif
      
      //! The language data list type (as used for preferential language lists)
      typedef std::vector < LanguageData* > languageDataList_type;
      
      //! The ID of the 'unknown tag'
      static const tagID_type unknownTagID = 0;

    protected:
      /* Replacement glyphs, both used to substitute missing or erroneous
       * values of some description. These are UTF-8 Encoded.
       */
      static const char* const replacementObjectGlyph;
      static const char* const replacementCharacterGlyph;

      // Marker chars, used internally
      static const char parameterMarkerChar;
      static const char newLineMarkerChar;
      
    private:
      // A set full of language tag name to tag ID mapping arrays
      typedef std::set < tagMap_type* > tagMaps_type;
      tagMaps_type tagMaps;
      
      /* Our "Tag Dictionary". As language files are loaded, tags are thrown
       * into the tag dictionary with a unique identifier for easier reference.
       * Later, the tag table bindery can discover each tag's unique identifier
       * to find the appropriate data within the language data vectors.
       */
# ifdef KINE_STL_HAS_HASH
      typedef std::hash_map < std::string, Languages::tagID_type >
	tagDictionary_type;
# else
      typedef std::map < std::string, Languages::tagID_type >
	tagDictionary_type;
# endif
      tagDictionary_type tagDictionary;

      //! The highest known TID
      tagID_type highestTagID;

      //! The language data itself
      languageDataMap_type languageDataMap;
      
      //! Our default language, to use if all else fails..
      const LanguageData* defaultLanguage;
      
      //! Our single instance (we exist once, and only once)
      static Languages* instance;
      
      //! Constructor
      Languages(void)
	: highestTagID(0),
	  defaultLanguage(0)
	{};

      //! Process a given tag name -> tag id map table using the tag dictionary
      void processTagMap(tagMap_type map) const;
      
    public:
      //! Destructor
      ~Languages(void)
	{};
      
      //! Create the single instance of this class
      static void initInstance(void);
      
      //! Return the single instance of this class (hopefully it exists ;)
      static Languages& getInstance(void)
	{ return *instance; };

      //! Load a given language file
      bool loadFile(const std::string& fileName, std::string& errString,
		    const bool makeDefault = false);
      
      //! Add/remove/process tag name/ID correlation maps
      bool registerMap(tagMap_type map);
      void deregisterMap(const tagMap_type map);
      void processMaps(void) const;

      //! Return the language data list itself (read-only access)
      const languageDataMap_type& getLanguageDataMap(void) const
	{ return languageDataMap; };
      
      //! Return the pointer to the default language's data
      const LanguageData* const getDefaultLanguage(void) const
	{ return defaultLanguage; };
      
      //! Find the given language, by its code..
      LanguageData* const findByCode(const std::string& code) const;
      
      //! Return the given language data, from the given language data
      const std::string
	get(const LanguageData* const languageData,
	    const tagID_type tagID,
	    const parameterList_type* const parameters = 0) const;

      //! Handle a multi-call tag with the given language data
      void get(const LanguageData* const languageData,
	       const tagID_type tagID,
	       callFunction_type& callFunction,
	       const parameterList_type* const parameters = 0) const;
      
      //! Return the given language data, from a language in the given list
      const std::string
	get(const languageDataList_type& languageDataList,
	    const tagID_type tagID,
	    const parameterList_type* const parameters = 0) const;

      //! Return the given language data, from the given language (by code)
      const std::string
	get(const std::string& languageCode,
	    const tagID_type tagID,
	    const parameterList_type* const parameters = 0) const
	{ return get(findByCode(languageCode), tagID, parameters); };

      //! Handle a multi-call tag with the given language data (by code)
      const void get(const std::string& languageCode,
		     const tagID_type tagID,
		     callFunction_type& callFunction,
		     const parameterList_type* const parameters = 0) const
	{ 
	   return get(findByCode(languageCode), tagID, callFunction, 
		      parameters);
	};

      //! Lazy functions for use when you have one to five parameters..
      template <class T>
	const std::string get(const T& languageCode,
			      const tagID_type tagID,
			      const parameter_type& p0) const
	{
	   parameterList_type parameters;
	   parameters.push_back(&p0);
	   std::string output(get(languageCode, tagID, &parameters));
	   return output;
	};
      template <class T>
	const std::string get(const T& languageCode,
			      const tagID_type tagID,
			      const parameter_type& p0,
			      const parameter_type& p1) const
	{
	   parameterList_type parameters;
	   parameters.push_back(&p0);
	   parameters.push_back(&p1);
	   std::string output(get(languageCode, tagID, &parameters));
	   return output;
	};
      template <class T>
	const std::string get(const T& languageCode,
			      const tagID_type tagID,
			      const parameter_type& p0,
			      const parameter_type& p1,
			      const parameter_type& p2) const
	{
	   parameterList_type parameters;
	   parameters.push_back(&p0);
	   parameters.push_back(&p1);
	   parameters.push_back(&p2);
	   std::string output(get(languageCode, tagID, &parameters));
	   return output;
	};
      template <class T>
	const std::string get(const T& languageCode,
			      const tagID_type tagID,
			      const parameter_type& p0,
			      const parameter_type& p1,
			      const parameter_type& p2,
			      const parameter_type& p3) const
	{
	   parameterList_type parameters;
	   parameters.push_back(&p0);
	   parameters.push_back(&p1);
	   parameters.push_back(&p2);
	   parameters.push_back(&p3);
	   std::string output(get(languageCode, tagID, &parameters));
	   return output;
	};
      template <class T>
	const std::string get(const T& languageCode,
			      const tagID_type tagID,
			      const parameter_type& p0,
			      const parameter_type& p1,
			      const parameter_type& p2,
			      const parameter_type& p3,
			      const parameter_type& p4) const
	{
	   parameterList_type parameters;
	   parameters.push_back(&p0);
	   parameters.push_back(&p1);
	   parameters.push_back(&p2);
	   parameters.push_back(&p3);
	   parameters.push_back(&p4);
	   std::string output(get(languageCode, tagID, &parameters));
	   return output;
	};
   }; // class Languages

   
   /* Lazy reference thingy for lazy coders.. like me.. but shh, don't tell
    * anyone I'm lazy and use lazy things like this lazy reference function :(
    */
   inline static Languages& languages(void)
     { return Languages::getInstance(); };
   
   
   // Oh dear, another lazy function for the truely pathetic lazy sods ;)
   inline static Languages& langs(void)
     { return Languages::getInstance(); };
}; // namespace Kine

#endif // _INCLUDE_KINEIRCD_LANGUAGES_H_
