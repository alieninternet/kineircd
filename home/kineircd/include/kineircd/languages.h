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
# include <algorithm>
# include <string>
# include <vector>


namespace Kine {
   /*!
    * \brief %Languages interface (\e langtags)
    * 
    * This is the language interface, used to retrieve language data in a
    * given language (or normally more specifically, a given User's language).
    * 
    * Each \e string is given a unique identifier, or a
    * \link Kine::Languages::tagID_type tag ID \endlink, which is used to
    * reference the particular string you wish to retrieve.
    */
   class Languages {
    public:
      //@{
      /*!
       * \brief The tag identifier type (also referred to as a 'TID').
       * 
       * \note TID #0 has a special purpose, as it represents unknownTagID.
       */
      typedef unsigned int tagID_type;
      
      //! The ID of the 'unknown tag'
      static const tagID_type unknownTagID = 0;
      //@}

      
      //@{
      /*!
       * \brief This is the <em>tag map</em> structure.
       * 
       * In order for your module to know the correlation of <em>tag name</em>
       * to <em>tag ID</em> upon run-time, you need to define this structure
       * within your module and register it, using the registerMap() function.
       * 
       * Once registered, and anytime language files are loaded/reloaded while
       * your map is registered, the language system will attempt to form the
       * link between tag names and their respective run-time ID's, filling
       * the map with the correct values.
       * 
       * Using the ID's set within this structure, you can then look up your
       * tags correctly using the array as a reference.
       *
       * If the tag name cannot be found (i.e. it does not exist within any
       * currently loaded language files) the tag ID will be set to
       * \link unknownTagID an unknown identifier \endlink, and an empty
       * string will be returned whenever you try to reference that tag's
       * data.
       *
       * Terminate the map array with a null-pointer \a tagName (a value of
       * \e 0).
       */
      struct TagMapEntry {
	 const char* const tagName;
	 tagID_type tagID;
      };
      
      typedef TagMapEntry tagMap_type[];
      //@}

      
      //@{
      //! The type of a 'parameter' (used in substitutions)
      typedef std::wstring parameter_type;
      
      /* The type of a 'parameter list'. Parameter lists are sent along side
       * a request to return language data in order to substitute parameter
       * tokens within the language data itself with their appropriate values
       * prior to being returned back to the software.
       */
      typedef std::vector < const parameter_type* > parameterList_type;
      //@}
      
      
      /* The call-out function used for multi-call get's.
       * Inherit from this, and use the operator() to do whatever you need
       * with each call. Each call is given a string, and must return a true
       * if you are willing to accept more data, or false to stop iterating
       * through the string and generating calls.
       */
      struct callFunction_type {
	 callFunction_type(void) {};
	 virtual ~callFunction_type(void) {};
	 virtual bool operator()(const std::wstring& data) = 0;
      };
      
      
      /* Language data class, this holds information about the language, along
       * with the tag data. It also provides basic look-up functions per
       * language
       */
      class LanguageData {
       public:
	 //! Tag data type
	 typedef std::vector < std::wstring* > tagData_type;
	 
       private:
	 std::string languageCode;		//!< RFC-3066 compliant code
	 std::wstring languageName;		//!< Name of the language
	 std::wstring languageNote;		//!< Optional notice
	 std::wstring maintainer;		//!< Maintainer of the file
	 long fileRevision;			//!< File revision
	 unsigned long tagCount;		//!< *real* number of tags
	 
	 tagData_type tagData;			//!< The tag data itself!

	 //! Merge the given language data "over" this language data
	 void mergeWith(const LanguageData& newData);

	 //! Process the given string (deal with substitutions and so forth)
	 const std::wstring
	   process(const std::wstring& data,
		   const parameterList_type* const parameters) const;
	 
	 //! Look for a given TID's data, and return it if possible
	 const std::wstring
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
	 const std::wstring& getLanguageName(void) const
	   { return languageName; };
	 
	 //! Return a note, if any, associated with this language
	 const std::wstring& getLanguageNote(void) const
	   { return languageNote; };
	 
	 //! Return the contact address (email/url/etc) of a maintainer, if any
	 const std::wstring& getMaintainer(void) const
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
      typedef std::vector < const LanguageData* > languageDataList_type;

      
      //! The code associated with a 'null' language (null pointer, no output)
      static const char* const nullLanguageCode;
      
      //! The code associated with the default language (not the real one)
      static const char* const defaultLanguageCode;
      
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
      typedef std::hash_map < std::string, tagID_type >
	tagDictionary_type;
# else
      typedef std::map < std::string, tagID_type >
	tagDictionary_type;
# endif
      tagDictionary_type tagDictionary;

      //! The highest known TID
      tagID_type highestTagID;

      //! The language data itself
      languageDataMap_type languageDataMap;
      
      //! Our default language, to use if all else fails..
      LanguageData* defaultLanguage;
      
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

      
      /*!
       * \brief Convert an ordinary std::string to a std::wstring
       * 
       * \param str The input string, only containing chars from the
       *    ISO-8859-1 set
       * \return A string of wchar_t characters with the same value as the
       *    input string
       * \warning This function sucks. It should not be used, as it should
       *    be removed sometime soon.
       */
      static const std::wstring toWideStr(const std::string str)
	{
	   std::wstring output;
	   for (std::string::const_iterator it = str.begin();
		it != str.end(); ++it) {
	      output += (wchar_t)(*it);
	   }
	   return output;
	};
      
      //! Load a given language file
      bool loadFile(const std::string& fileName, std::string& errString,
		    const bool makeDefault = false);

      //! Return highest known tag ID (the total number of tags known)
      const tagID_type getHighestTagID(void) const
	{ return highestTagID; };
      
      //! Find a tag by its name (It's recommended to use a tag map, below)
      const tagID_type getTagID(const std::string& name) const;
      
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
      const std::wstring
	get(const LanguageData* const languageData,
	    const tagID_type tagID,
	    const parameterList_type* const parameters = 0) const;

      //! Handle a multi-call tag with the given language data
      void get(const LanguageData* const languageData,
	       const tagID_type tagID,
	       callFunction_type& callFunction,
	       const parameterList_type* const parameters = 0) const;
      
      //! Return the given language data, from a language in the given list
      const std::wstring
	get(const languageDataList_type& languageDataList,
	    const tagID_type tagID,
	    const parameterList_type* const parameters = 0) const;

      //! Return the given language data, from the given language (by code)
      const std::wstring
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

      //! Lazy functions for use when you have one to six parameters..
      template <class T>
	const std::wstring get(const T& languageCode,
			       const tagID_type tagID,
			       const parameter_type& p0) const
	{
	   parameterList_type parameters;
	   parameters.push_back(&p0);
	   std::wstring output(get(languageCode, tagID, &parameters));
	   return output;
	};
      template <class T>
	const std::wstring get(const T& languageCode,
			       const tagID_type tagID,
			       const parameter_type& p0,
			       const parameter_type& p1) const
	{
	   parameterList_type parameters;
	   parameters.push_back(&p0);
	   parameters.push_back(&p1);
	   std::wstring output(get(languageCode, tagID, &parameters));
	   return output;
	};
      template <class T>
	const std::wstring get(const T& languageCode,
			       const tagID_type tagID,
			       const parameter_type& p0,
			       const parameter_type& p1,
			       const parameter_type& p2) const
	{
	   parameterList_type parameters;
	   parameters.push_back(&p0);
	   parameters.push_back(&p1);
	   parameters.push_back(&p2);
	   std::wstring output(get(languageCode, tagID, &parameters));
	   return output;
	};
      template <class T>
	const std::wstring get(const T& languageCode,
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
	   std::wstring output(get(languageCode, tagID, &parameters));
	   return output;
	};
      template <class T>
	const std::wstring get(const T& languageCode,
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
	   std::wstring output(get(languageCode, tagID, &parameters));
	   return output;
	};
      template <class T>
	const std::wstring get(const T& languageCode,
			       const tagID_type tagID,
			       const parameter_type& p0,
			       const parameter_type& p1,
			       const parameter_type& p2,
			       const parameter_type& p3,
			       const parameter_type& p4,
			       const parameter_type& p5) const
	{
	   parameterList_type parameters;
	   parameters.push_back(&p0);
	   parameters.push_back(&p1);
	   parameters.push_back(&p2);
	   parameters.push_back(&p3);
	   parameters.push_back(&p4);
	   parameters.push_back(&p5);
	   std::wstring output(get(languageCode, tagID, &parameters));
	   return output;
	};
   }; // class Languages

   
   /*!
    * \brief Return the current instance of the Languages:: class
    *
    * \return The current instance of the Languages class
    * \relates Kine::Languages
    */
   inline static Languages& languages(void)
     { return Languages::getInstance(); };
}; // namespace Kine

#endif // _INCLUDE_KINEIRCD_LANGUAGES_H_
