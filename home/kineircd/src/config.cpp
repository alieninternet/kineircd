/* config.cpp
 * IRC daemon configuration data parsing functions
 */

#include "autoconf.h"

#include <fstream>

#ifdef HAVE_OPENSSL
# include <openssl/ssl.h>
#endif

#include "daemon.h"
#include "debug.h"
#include "operator.h"
#include "utils.h"
#include "lang.h"
#include "servernotice.h"

// Classes table. Many many duplicates to make config file writing easier..
struct {
   char *className;
	void (*classHandler)(ConfigData *conf, String *line, String::size_type *pos, bool firstRun);
}

 configurationClassTable[] = {
     { "ADMIN",			Daemon::configADMIN },
     { "ADMINISTRATION",	Daemon::configADMIN },
     { "ADMINISTRATOR",		Daemon::configADMIN },
     { "ARBITER",		Daemon::configARBITER },
     { "ARBITERS",		Daemon::configARBITER },
     { "ARBITRATOR",		Daemon::configARBITER },
     { "ARBITRATORS",		Daemon::configARBITER },
     { "CONF",			Daemon::configCONF },
     { "CONFIG",		Daemon::configCONF },
     { "CONFIGURATION",		Daemon::configCONF },
     { "FAIL",			Daemon::configFAIL },
     { "FAILOBJECTS",		Daemon::configFAIL },
     { "LISTEN",		Daemon::configLISTEN },
     { "LISTENON",		Daemon::configLISTEN },
     { "LISTENTO",		Daemon::configLISTEN },
     { "OPERS",			Daemon::configOPERS },
     { "OPERATORS",		Daemon::configOPERS },
     { "PROHIBIT",		Daemon::configFAIL },
     { "RANK",			Daemon::configSTATUS },
     { "REDIRECT",		Daemon::configREDIRECT },
     { "REDIRECTION",		Daemon::configREDIRECT },
     { "SSL",			Daemon::configSSL },
     { "STATUS",		Daemon::configSTATUS },
     { 0 }
};


/* ConfigData - Constructor for the temporary data holding class, ConfigData
 * Original 05/09/01 simonb
 */
ConfigData::ConfigData(void)
: adminName(""),
  adminEmail(""),
  adminLocation(""),
  confAutobone(true),
  confDescription(""),
  confHidden(false),
  confLanguageDir("."),
  confMOTD(DEFAULT_CONFIG_MOTD_FILE),
  confNetwork(""),
  confNoop(false),
  confServername("")
{
   failNicknames.clear();
   failChannels.clear();
   opersOperators.clear();
   redirectChannels.clear();
#ifdef STL_HAS_SLIST
   motd.clear();
#endif
}


/* configComplain - Complain about something to the appropriate output
 * Original 05/09/01 simonb
 */
void Daemon::configComplain(bool firstRun, String const &complaint)
{
   String message = String("Configuration error: ") + complaint;
   
   if (firstRun) {
      cout << message << endl;
   } else {
      logger(message, LOGPRI_ERROR);
      serverNotice(ServerNotice::SN_HOUSEKEEPING, message);
   }
}


/* configWarning - Warn about something to the appropriate output
 * Original 05/09/01 simonb
 */
void Daemon::configWarning(bool firstRun, String const &complaint)
{
   String message = String("Configuration warning: ") + complaint;
   
   if (firstRun) {
      cout << message << endl;
   } else {
      logger(message, LOGPRI_WARNING);
      serverNotice(ServerNotice::SN_HOUSEKEEPING, message);
   }
}


/* configCopy - Copy the configuration data into appropriate places
 * Original 14/09/01 simonb
 */
bool Daemon::configCopy(bool firstRun, ConfigData *conf)
{
   /* Now it is time to check the variables. First off, we need complete
    * administrator information, except the 'location' line - that is
    * optional.
    */
   if (!conf->adminName.length()) {
      configComplain(firstRun,
		     "No administator name specified in configuration file!");
      return false;
   }
   
   if (!conf->adminEmail.length()) {
      configComplain(firstRun,
		     "No administrator e-mail specified in configuration "
		     "file!");
      return false;
   }
   
   // We need a server name as well...
   if (!conf->confServername.length()) {
      configComplain(firstRun,
		     "No server name given in the configuration file!");
      return false;
   }
   
   // If there is no server description we will plonk one in there
   if (!conf->confDescription.length()) {
      configWarning(firstRun,
		    "Inserting default server description - this is most "
		    "likely NOT what you want");
      conf->confDescription = DEFAULT_CONFIG_SERVER_DESCRIPTION;
   }

   // Phew, if we got here, it is time to copy those values across...
   adminName = conf->adminName;
   adminEmail = conf->adminEmail;
   adminLocation = conf->adminLocation;
   
   // We can only change this value on the first run..
   if (firstRun) {
      server->hostname = conf->confServername;
   } else if (server->hostname != conf->confServername) {
      configWarning(firstRun,
		    "Cannot change server hostname, requires server restart");
   }
 
   // We can change this outside first run, but it will take a while to sync
   if (firstRun) {
      server->description = conf->confDescription;
   } else if (server->description != conf->confDescription) {
      configWarning(firstRun,
		    "Server description will be out of sync with the network! "
		    "Full reconnections of the server is necessary to repair");
   }
   
   // Copy the network name over, no check needed
   networkName = conf->confNetwork;

   // Boolean stuff that manipulates server modes - this is complicated!
//   String toggleOnStr = "";
//   String toggleOffStr = "";
//   String toggleOnParams = "";
//   String toggleOffParams = "";
//   
   // Check the AUTOBONE setting
//   if (conf->confAutobone) {
      // Is this a change?
//      if (!(server->modes & Server::M_AUTOBONE)) {
//	 server->modes |= Server::M_AUTOBONE;
//	 toggleOnStr = toggleOnStr + String('a');
//      }
//   } else {
//   }

   // Wipe the fail lists clean and copy the new lists across
   wipeFails();
   failNicknames.swap(conf->failNicknames);
   failChannels.swap(conf->failChannels);

   // Clean up the opers list and copy the new list across
   wipeOpers();
   operators.swap(conf->opersOperators);

   // Clean the redirect list and copy the new lists across
   wipeRedirects();
   redirectChannels.swap(conf->redirectChannels);
   
   // Open and read the MOTD file, if we can.
   if (conf->confMOTD.length()) {
      ifstream file(conf->confMOTD.c_str());
      String temp = "";
      
      if (file) {
	 // Reset the motd
	 motd.clear();
	 
	 // Run through the file
	 while (!file.eof()) {
	    // Read a line
//	    file >> temp;
	    std::getline(file, temp);
	    
	    // gay std::string.
	    if (!file.good() || file.eof()) {
	       break;
	    }
	    
#ifdef CONVERT_MOTD_TABS
	    String newLine = "";
	    
	    // Run through the line and look for tabs to fix
	    for (String::size_type i = 0; i < temp.length(); i++) {
	       switch (temp[i]) {
		case '\t':
		  newLine += "        "; // 8 chars
		  continue;
		default:
		  newLine += temp[i];
	       }
	    }

	    // Add the line to the MOTD in memory
# ifdef STL_HAS_SLIST
	    conf->motd.push_front(temp);
# else
	    motd.push_back(newLine);
# endif
#else
	    // Add the line to the MOTD in memory
# ifdef STL_HAS_SLIST
	    conf->motd.push_front(temp);
# else
	    motd.push_back(String(temp);
# endif
#endif
	 }
      
	 // Close down the file
	 file.close();
	 
#ifdef STL_HAS_SLIST
	 // Copy the list across, to make the file the 'right way up'
	 motd.clear();
	 for (ConfigData::motd_buffer_t::iterator it = conf->motd.begin();
	      it != conf->motd.end(); it++) {
	    motd.push_front(*it);
	 }
#endif
      } else {
	 configWarning(firstRun,
		       "Could not open MOTD file (" + conf->confMOTD + ")");
      }
   }

   // Fire up the languages now that we know the directory
   return Lang::loadLanguages(conf->confLanguageDir, "");
}


/* configure - Load/parse the configuration files
 * Original 03/09/01 simonb
 * Note: Nice looking easy to use configuration file = slower and heavier
 *       configuration parser. Fortunate for us this only gets called on the
 *       rare occasion! :)
 */
bool Daemon::configure(bool firstRun)
{
   if (firstRun) {
      Socket *s = 0;
      
      // Kill all listening sockets. They will be recreated, anyway
      wipeListens();
      
      // Temporary: poor checking..
      s = new SocketIPv4(0x00000000, 6666, true, false);
      
      if (!s->setNonBlocking() ||
	  !s->setReuseAddress()) {
	 cout << "EEK sockopts" << endl;
      }
      
      if (s->bind()) {
	 if (!s->listen(LISTEN_MAXQUEUE)) {
	    cout << "EEK listen" << endl;
	 }
      } else {
	 cout << "EEK bind; abort." << endl;
	 exit(1);
      }
      
      // Add to the listen list
      listens.push_front(new Listen(s, false));
      
      // Add this to our select() FD_SET too; but only to the input set.
      addInputFD(s->getFD());

#ifdef HAVE_OPENSSL
      /*
       *  This time, secure.
       */
      
      // Temporary: poor checking..
      s = new SocketIPv4(0x00000000, 8888, true, false, this);
      
      if (!s->setNonBlocking() ||
	  !s->setReuseAddress()) {
	 cout << "EEK sockopts" << endl;
      }
      
      if (s->bind()) {
	 if (!s->listen(LISTEN_MAXQUEUE)) {
	    cout << "EEK listen" << endl;
	 }
      } else {
	 cout << "EEK bind; abort." << endl;
	 exit(1);
      }
      
      // Add to the listen list
      listens.push_front(new Listen(s, true));
      
      // Add this to our select() FD_SET too; but only to the input set.
      addInputFD(s->getFD());
#endif   
      
   }
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   ConfigData conf;
   
   /* We need to open up the main configuration file first. It has information
    * we need to find the other files (motd etc)
    */
   String configData = "", temp = "";
   ifstream file(configFile.c_str());
   
   // Check that we opened the file ok
   if (!file) {
      configComplain(firstRun,
		     String("Could not open configuration file (") +
		     configFile + ") :(");
      return false;
   }

#ifdef DEBUG
   debug("Reading main configuration from '" + configFile + "'...");
#endif
   
   bool inComment = false, inQuote = false;
   char quoteChar = 0;

   /* Read the entire file into our configuration data buffer, cropping away
    * wasteful data such as tabs, spaces, new lines, comments etc.
    */
   while (!file.eof()) {
//      file >> temp;
      std::getline(file, temp);
      
      if (!file.good() || file.eof()) {
	 break;
      }

      // Trim away leading/trailing whitespace
      temp = temp.trim();
      
      // Skip an empty line..
      if (!temp.length()) {
	 continue;
      }
      
      /* Run through the line and MUNG it down into something easier for us
       * to digest :) Yes, bad pun, sorry. :( *hides from the pun police*
       */
      for (String::size_type i = 0; i < temp.length(); i++) {
	 // If we are in a quote, we have to copy this string over literally
	 if (inQuote) {
	    configData += temp[i];
	    
	    // Check if we are leaving a quote..
	    if (temp[i] == quoteChar) {
	       inQuote = false;
	    }
	    
	    continue;
	 } else if (inComment) {
	    /* If we are in a comment, we need to ignore everything until the
	     * end of the comment.. So all we do here is check if we are done.
	     * Note, that C++ style comments run until the end of the line, so
	     * they end up aborting to the next line anyway, so all we are
	     * checking for here are traditional comments. We do not check for
	     * nested comments, but the configuration file isn't a high-level
	     * programming language so SHUSH! :)
	     */
	    if ((temp[i] == '*') && 
		(temp.length() > (i + 1)) &&
		(temp[i + 1] == '/')) {
	       i++;
	       inComment = false;
	    }

	    continue;
	 } else {
	    // Everything else we have to examine...
	    switch (temp[i]) {
	     case ' ':
	     case '\v':
	     case '\t': // Whitespace, we ignore this..
	       continue;
	     case '/': // Comment perhaps?
	       // C or C++ style?
	       if (temp[i + 1] == '*') {
		  // C style, continue on..
		  inComment = true;
	       } else if (temp[i + 1] == '/') {
		  // C++ style, we gotta break out of this one.
		  i = temp.length();
	       } else {
		  // Oh well, pass it through normally...
		  configData += temp[i];
	       }
	       continue;
	     case '\'':
	     case '"':
	     case '`': // Beginning of a quote... (note - no break)
	       quoteChar = temp[i];
	       inQuote = true;
	     default:
	       configData += temp[i];
	    }
	 }
      }
   }

   // Check if comments were terminated properly
   if (inComment) {
	 configComplain(firstRun,
			"Unterminated comment found in configuration file!");
      return false;
   }

   // Check if quotes were terminated properly
   if (inQuote) {
      configComplain(firstRun,
		     "Unterminated quote found in configuration file!");
      return false;
   }

   // Close down the configuration file.
   file.close();
   
   String className = "";
   bool gotClass = false;

   // Run through the generated line
   for (String::size_type i = 0; i < configData.length(); i++) {
      switch (configData[i]) {
       case ';': // Stray semi-colon.. We will ignore it
	 continue;
       case '{': // Start of a class - time to jump into a class...
	 // Fix the class name
	 className = className.toUpper().trimQuotes();
	 
	 // Look for this class name in the class table
	 for (int ii = 0; configurationClassTable[ii].className != 0; ii++) {
	    // Check for a match
	    if (className == configurationClassTable[ii].className) {
	       gotClass = true;
	       
	       // Check if there is a handler for this class..
	       if (configurationClassTable[ii].classHandler) {
#ifdef DEBUG_PSYCHO_
		  debug(String(" +-> Class: ") + className);
#endif
		  
		  // Enter the handler
		  configurationClassTable[ii].classHandler(&conf,
							   &configData, 
							   &i, firstRun);
	       } else {
		  // Otherwise we are ignoring this class
#ifdef DEBUG_EXTENDED
		  debug("Ignoring top-level configuration class set '" +
			className + '\'');
#endif
		  // Skip until we hit the end of this class or the line..
		  while (configData[i] != '}') {
		     // Check we have not skipped over the EOL..
		     if (++i >= configData.length()) {
			configComplain(firstRun,
				       "Unterminated top-level class found "
				       "in configuration file!");
			return false;
		     }
		  }
	       }
	    } 
	 }

	 // If We did not find this in the configuration file, then complain
	 if (!gotClass) {
	    configComplain(firstRun,
			   String("Unknown top-level class '") +
			   className + "'");
	    return false;
	 }
	 
	 // We have returned from that class, reset some variables
	 className = "";
	 gotClass = false;
	 break;
       default:
	 // Copy the character into the class name...
	 className += configData[i];
      }
   }

#ifdef DEBUG_EXTENDED
   debug("Configuration loading/parsing complete.");
#endif
   
   // If we got here, all must be well...
   return configCopy(firstRun, &conf);
}


/* configADMIN
 * Original 03/09/01 simonb
 */
void Daemon::configADMIN(ConfigData *conf, String *line, String::size_type *pos, bool firstRun)
{
   String command = "", parameter = "";
   bool doingCommand = true;
   
   // Run through the string as far as we need to
   while ((*line)[*pos] != '}') {
      // Check we have not skipped over the EOL..
      if (++(*pos) >= line->length()) {
	 Daemon::configComplain(firstRun, 
			      "Unterminated ADMIN class in configuration");
	 return;
      }
      
      // Check this character
      switch ((*line)[*pos]) {
       case ';': // End of command marker, process the given command/paramter
	 // Check if this is right..
	 if (doingCommand) {
	    Daemon::configComplain(firstRun,
				 "Semi-colon found in variable name side of "
				 "'=' in ADMIN class (Missing semi-colon?)");
	    return;
	 }

	 // Fix up the strings
	 command = command.toUpper().trimQuotes();
	 parameter = parameter.trimQuotes();
	 
#ifdef DEBUG_PSYCHO_
	 debug(String::printf(" | +-> Variable: %s, Param: %s",
			      (char const *)command,
			      (char const *)parameter));
#endif

	 // Work out what to do with this. This is ugly, but oh well..
	 if (command == "NAME") {
	    conf->adminName = String(":") + parameter;
	 } else if (command == "LOCATION") {
	    conf->adminLocation = String(":") + parameter;
	 } else if (command == "EMAIL") {
	    conf->adminEmail = String(":") + parameter;
	 } else {
	    Daemon::configComplain(firstRun,
				 String("Ignoring unknown variable in ADMIN "
					"class: ") +
				 command);
	 }
	 
	 // Finish up.
	 doingCommand = true;
	 command = parameter = "";
	 continue;
       case '=': // Command/Parameter separator
	 // Check if we have not already gotten this..
	 if (!doingCommand) {
	    Daemon::configComplain(firstRun,
				 command + 
				 " variable is invalid (two equal signs) in "
				 "ADMIN class");
	    return;
	 }
	 
	 // We are now reading to the parameter
	 doingCommand = false;
	 continue;
       default:
	 // Where do we append this char to?
	 if (doingCommand) {
	    command += (*line)[*pos];
	 } else {
	    parameter += (*line)[*pos];
	 }
      }
   }
   
   return;
}


/* configARBITER
 * Original  simonb
 */
void Daemon::configARBITER(ConfigData *conf, String *line, String::size_type *pos, bool firstRun)
{
   int c = 1;
   (*pos)++;
   while (c > 0) {
      switch ((*line)[*pos]) {
       case '{':
	 c++;
	 break;
       case '}':
	 c--;
	 break;
      }
      
      // Check we have not skipped over the EOL..
      if (++(*pos) >= line->length()) {
	 return;
      }
   }
}


/* configCONF
 * Original 03/09/01 simonb
 */
void Daemon::configCONF(ConfigData *conf, String *line, String::size_type *pos, bool firstRun)
{
   String command = "", parameter = "";
   bool doingCommand = true;
   
   // Run through the string as far as we need to
   while ((*line)[*pos] != '}') {
      // Check we have not skipped over the EOL..
      if (++(*pos) >= line->length()) {
	 Daemon::configComplain(firstRun,
			      "Unterminated CONF class in configuration");
	 return;
      }
      
      // Check this character
      switch ((*line)[*pos]) {
       case ';': // End of command marker, process the given command/paramter
	 // Check if this is right..
	 if (doingCommand) {
	    Daemon::configComplain(firstRun,
				 "Semi-colon found in variable name side of "
				 "'=' in CONF class (Missing semi-colon?)");
	    return;
	 }

	 // Fix up the strings
	 command = command.toUpper().trimQuotes();
	 parameter = parameter.trimQuotes();
	 
#ifdef DEBUG_PSYCHO_
	 debug(String::printf(" | +-> Variable: %s, Param: %s",
			      (char const *)command,
			      (char const *)parameter));
#endif

	 // Work out what to do with this. This is ugly, but oh well..
	 if (command == "AUTOBONE") {
	    conf->confAutobone = Utils::toBool(parameter, conf->confAutobone);
	 } else if ((command == "DESCRIPTION") ||
		    (command == "DESC")) {
	    conf->confDescription = parameter;
	 } else if (command == "HIDDEN") {
	    conf->confHidden = Utils::toBool(parameter, conf->confHidden);
	 } else if (command == "LANGDIR") {
	    // Check the parameter, the directory needs a trailing '/'
	    if (parameter[parameter.length()] != '/') {
	       parameter += '/';
	    }
	    
	    conf->confLanguageDir = parameter;
	 } else if (command == "MOTD") {
	    conf->confMOTD = parameter;
	 } else if ((command == "NETWORK") ||
		    (command == "NETWORKNAME")) {
	    // Ensure we only get the 'first word' or this will break ISUPPORT
	    StringTokens paramTokens(parameter);
	    conf->confNetwork = paramTokens.nextToken();
	 } else if ((command == "NOOP") ||
		    (command == "NOOPER")) {
	    conf->confNoop = Utils::toBool(parameter, conf->confNoop);
	 } else if (command == "SERVERNAME") {
	    conf->confServername = parameter;
	 } else {
	    Daemon::configWarning(firstRun,
				String("Ignoring unknown variable in "
				       "CONF class: ") +
				command);
	 }
	 
	 // Finish up.
	 doingCommand = true;
	 command = parameter = "";
	 continue;
       case '=': // Command/Parameter separator
	 // Check if we have not already gotten this..
	 if (!doingCommand) {
	    Daemon::configComplain(firstRun,
				 command +
				 " variable is invalid (two equal signs) "
				 "in CONF class");
	    return;
	 }
	 
	 // We are now reading to the parameter
	 doingCommand = false;
	 continue;
       default:
	 // Where do we append this char to?
	 if (doingCommand) {
	    command += (*line)[*pos];
	 } else {
	    parameter += (*line)[*pos];
	 }
      }
   }
}


/* configFAIL
 * Original 06/09/01 simonb
 */
void Daemon::configFAIL(ConfigData *conf, String *line, String::size_type *pos, bool firstRun)
{
   String subclassName = "", mask = "", reason = "";
   bool doingMask = true;
   
   // Run through the line
   for ((*pos)++; *pos < line->length(); (*pos)++) {
      switch ((*line)[*pos]) {
       case ';': // Stray semi-colon.. We will ignore it
	 continue;
       case '}': // End of this class, return to caller
	 return;
       case '{': // Start of a subclass
	 // Fix the class name
	 subclassName = subclassName.toUpper().trimQuotes();

	 // Clean up some variables before their first use
	 mask = reason = "";
	 doingMask = true;

	 // We will be assigning this soon, so save some stress!
	 Daemon::relationmask_list_t *failList;

#ifdef DEBUG_PSYCHO_
	 debug(String(" | +-> Sub-Class: ") + subclassName);
#endif
	 
	 // Work out which list we are working on during this subclass
	 if ((subclassName == "NICKNAMES") ||
	     (subclassName == "NICKNAME") ||
	     (subclassName == "NICKS") ||
	     (subclassName == "NICK")) {
	    failList = &conf->failNicknames;
	 } else if ((subclassName == "CHANNELS") ||
		    (subclassName == "CHANNEL")) {
	    failList = &conf->failChannels;
	 } else {
	    Daemon::configComplain(firstRun,
				 String("Unknown subclass '") + subclassName +
				 "' found in FAIL class");
	    return;
	 }
	 
	 // Continue running through the line..
	 while ((*line)[*pos] != '}') {
	    // Check we have not skipped over EOL
	    if (++(*pos) >= line->length()) {
	       Daemon::configComplain(firstRun,
				    String("Unterminated FAIL.") + 
				    subclassName + " class in configuration");
	       return;
	    }
	    
	    // Check this character
	    switch ((*line)[*pos]) {
	     case ';': // End of variable marker. Processing time
	       // Check if this was wrong
	       if (doingMask) {
		  Daemon::configComplain(firstRun,
				       String("Semi-colon found in mask name "
					      "side of '=' in FAIL.") +
				       subclassName + 
				       " class (Missing semi-colon?)");
		  return;
	       }
	       
	       // Fix up the strings
	       mask = mask.toLower().trimQuotes();
	       reason = reason.trimQuotes();
	       
#ifdef DEBUG_PSYCHO_
	       debug(String::printf(" | | +-> Mask: %s (%s)",
				    (char const *)mask,
				    (char const *)reason));
#endif
	       
	       // Add this to the list
	       failList->push_front(new RelationMask(StringMask(mask), 
						     reason));
	       
	       // Clean up
	       mask = reason = "";
	       doingMask = true;
	       continue;
	     case '=': // Mask/Reason separator
	       // Check if this is right
	       if (!doingMask) {
		  Daemon::configComplain(firstRun,
				       mask + " mask is invalid (two "
				       "equal signs) in FAIL." +
				       subclassName + " class");
		  return;
	       }
	       
	       // Swap!
	       doingMask = false;
	       continue;
	     default:
	       // Work out where to append this char
	       if (doingMask) {
		  mask += (*line)[*pos];
	       } else {
		  reason += (*line)[*pos];
	       }
	    }
	 }

	 // We have returned from that class, reset some variables
	 subclassName = "";
	 break;
       default:
	 // Copy the character into the class name...
	 subclassName += (*line)[*pos];
      }
   }

   // If we hit the end, we failed. Damn.
   Daemon::configComplain(firstRun,
			  "Unterminated FAIL class in configuration");
   return;
}


/* configLISTEN
 * Original  simonb
 */
void Daemon::configLISTEN(ConfigData *conf, String *line, String::size_type *pos, bool firstRun)
{
   while ((*line)[*pos] != '}') {
      // Check we have not skipped over the EOL..
      if (++(*pos) >= line->length()) {
	 return;
      }
   }
}


/* configOPERS
 * Original 07/09/01 simonb
 */
void Daemon::configOPERS(ConfigData *conf, String *line, String::size_type *pos, bool firstRun)
{
   String command = "", parameter = "";
   String nickname = "", name = "", password = "", user = "", host = "";
   bool isGlobal = false, doingCommand = true, doingUser = true;
   
   // Run through the line
   for ((*pos)++; *pos < line->length(); (*pos)++) {
      switch ((*line)[*pos]) {
       case ';': // Stray semi-colon.. We will ignore it
	 continue;
       case '}': // End of a class
	 return;
       case '{': // Start of an oper reacord
	 // Fix the operator nickname
	 nickname = nickname.IRCtoLower().trimQuotes();
	 
	 // Fix up some other variables
	 name = nickname;
	 command = parameter = password = "";
	 isGlobal = false;
	 
#ifdef DEBUG_PSYCHO_
	 debug(String(" | +-> Operator: ") + nickname);
#endif
 
	 // Continue reading this line
	 while ((*line)[*pos] != '}') {
	    // Check that we have not gone over the EOL
	    if (++(*pos) >= line->length()) {
	       Daemon::configComplain(firstRun,
				    String("Unterminated operator '") +
				    nickname + 
				    "' in OPERS class in configuration");
	       return;
	    }
	    
	    // Check this character
	    switch ((*line)[*pos]) {
	     case ';': // End of command marker, process the given stuff
	       // Make sure we are not being called prematurely
	       if (doingCommand) {
		  Daemon::configComplain(firstRun,
				       "Semi-colon found in variable name side "
				       "of '=' in ADMIN class (Missing "
				       "semi-colon?)");
		  return;
	       }
	       
	       // Fix up the strings
	       command = command.toUpper().trimQuotes();
	       parameter = parameter.trimQuotes();
	       
#ifdef DEBUG_PSYCHO_
	       debug(String::printf(" | | +-> Variable: %s, Param: %s",
				    (char const *)command,
				    (char const *)parameter));
#endif
	       
	       // Work out what to do here
	       if (command == "NAME") {
		  name = parameter;
	       } else if (command == "PASSWORD") {
		  password = parameter;
	       } else if (command == "TYPE") {
		  // Fix up this parameter
		  parameter = parameter.toUpper().trimQuotes();
		  
		  // Check what we have to do
		  if (parameter == "GLOBAL") {
		     isGlobal = true;
		  } else if (parameter == "LOCAL") {
		     isGlobal = false;
		  } else {
		     Daemon::configComplain(firstRun,
					  String("Unknown operator type in "
						 "operator '") + nickname +
					  "' in OPERS class in configuration");
		     return;
		  }
	       } else {
		  Daemon::configComplain(firstRun,
				       String("Ignoring unknown variable '") +
				       command + "' in operator '" + nickname +
				       "' in OPERS class in configuration");
	       }

	       // Finish up
	       doingCommand = true;
	       command = parameter = "";
	       continue;
	     case '=': // Variable/Paramter separator thingy
	       // Check if we have not already gotten this..
	       if (!doingCommand) {
		  Daemon::configComplain(firstRun,
				       command +
				       " variable is invalid (two equal "
				       "signs) in operator '" + 
				       nickname +
				       "' in OPERS class in configuration");
		  return;
	       }
	       
	       // Switch to reading the parameter
	       doingCommand = false;
	       continue;
	     case '{': // Beginning of a subclass
	       // Fix up the command (subclass name) variable
	       command = command.toUpper().trimQuotes();
	       
#ifdef DEBUG_PSYCHO_
	       debug(String(" | | +-> Sub-Class: ") + command);
#endif

	       // Check what to do
	       if (command != "HOSTS") {

		  Daemon::configComplain(firstRun,
				       String("Unknown sub-class '") +
				       command + "' in operator '" + nickname +
				       "' in OPERS class in configuration");
		  return;
	       }

	       // Fix up some variables
	       user = host = "";
	       doingUser = true;
	       
	       // Keep going through the line - urgh!
	       while ((*line)[*pos] != '}') {
		  // Check for jumping over the EOL..
		  if (++(*pos) >= line->length()) {
		     Daemon::configComplain(firstRun,
					  String("Unterminated HOSTS "
						 "subclass in operator '") +
					  nickname + 
					  "' in OPERS class in "
					  "configuration");
		     return;
		  }
		  
		  // Check what do do with this char
		  switch ((*line)[*pos]) {
		   case ';': // End of command marker
		     /* If we are still doing the user, then assume the
		      * user part of the identity was not specified, so we
		      * need to 'patch' this up
		      */
		     if (doingUser) {
			host = user;
			user = "*";
		     }
		     
		     // Fix the strings
		     user = user.toLower().trimQuotes();
		     host = host.toLower().trimQuotes();
		     
#ifdef DEBUG_PSYCHO_
		     debug(String::printf(" | | | +-> Access Identify: %s@%s",
					  (char const *)user,
					  (char const *)host));
#endif
		     
		     
		     
		     // Finish up
		     doingUser = true;
		     user = host = "";
		     continue;
		   case '@':
		     // Check if we have not already gotten this
		     if (!doingUser) {
			Daemon::configComplain(firstRun,
					     String("Syntax error (identity "
						    "mask has two '@' "
						    "characters) in operator "
						    "'") + nickname +
						    "' in OPERS class in "
						    "configuration?");
			return;
		     }
		     
		     // Switch to reading the hostname
		     doingUser = false;
		     continue;
		   default: // Read a char into a string
		     if (doingUser) {
			user += (*line)[*pos];
		     } else {
			host += (*line)[*pos];
		     }
		  }
		  
	       }

	       // Check if a trailing semi-colon was given; skip it if it was
	       if (((*pos + 1) <= line->length()) &&
		   ((*line)[*pos + 1] == ';')) {
		  (*pos)++;
	       }
	       
	       // Clean up
	       command = "";
	       continue;
	     default: // Reading a char to somewhere
	       // Where to?
	       if (doingCommand) {
		  command += (*line)[*pos];
	       } else {
		  parameter += (*line)[*pos];
	       }
	    }
	 }
	 
	 // Add this operator to our list
	 conf->opersOperators[nickname] = (new Operator(name, password,
							isGlobal));
	 
	 // Increase the char count to skip the end of class set-brace
	 (*pos)++;
	 
	 // Reset the nickname
	 nickname = "";

	 continue;
       default:
	 // Copy the character into the nickname string...
	 nickname += (*line)[*pos];
      }
   }

   // If we hit the end, oops
   Daemon::configComplain(firstRun,
			"Unterminated OPERS class in configuration");
   return;
}


/* configREDIRECT
 * Original 13/09/01 simonb
 */
void Daemon::configREDIRECT(ConfigData *conf, String *line, String::size_type *pos, bool firstRun)
{
   String subclassName = "", mask = "", dest = "";
   bool doingMask = true;
   
   // Run through the line
   for ((*pos)++; *pos < line->length(); (*pos)++) {
      switch ((*line)[*pos]) {
       case ';': // Stray semi-colon.. We will ignore it
	 continue;
       case '}': // End of this class, return to caller
	 return;
       case '{': // Start of a subclass
	 // Fix the class name
	 subclassName = subclassName.toUpper().trimQuotes();

	 // Clean up some variables before their first use
	 mask = dest = "";
	 doingMask = true;

	 // We will be assigning this soon, so save some stress!
	 Daemon::relationmask_list_t *redirectList;

#ifdef DEBUG_PSYCHO_
	 debug(String(" | +-> Sub-Class: ") + subclassName);
#endif
	 
	 // Work out which list we are working on during this subclass
	 if ((subclassName == "CHANNELS") ||
	     (subclassName == "CHANNEL")) {
	    redirectList = &conf->redirectChannels;
	 } else if ((subclassName == "HOSTS") ||
		    (subclassName == "HOST")) {
	    // skip
	    continue;
	 } else {
	    Daemon::configComplain(firstRun,
				 String("Unknown subclass '") + subclassName +
				 "' found in REDIRECT class");
	    return;
	 }
	 
	 // Continue running through the line..
	 while ((*line)[*pos] != '}') {
	    // Check we have not skipped over EOL
	    if (++(*pos) >= line->length()) {
	       Daemon::configComplain(firstRun,
				    String("Unterminated REDIRECT.") + 
				    subclassName + " class in configuration");
	       return;
	    }
	    
	    // Check this character
	    switch ((*line)[*pos]) {
	     case ';': // End of variable marker. Processing time
	       // Check if this was wrong
	       if (doingMask) {
		  Daemon::configComplain(firstRun,
				       String("Semi-colon found in mask name "
					      "side of '=' in REDIRECT.") +
				       subclassName + 
				       " class (Missing semi-colon?)");
		  return;
	       }
	       
	       // Fix up the strings
	       mask = mask.toLower().trimQuotes();
	       dest = dest.trimQuotes();
	       
#ifdef DEBUG_PSYCHO_
	       debug(String::printf(" | | +-> Redirect: %s, To: %s",
				    (char const *)mask,
				    (char const *)dest));
#endif
	       
	       // Add this to the list
	       redirectList->push_front(new RelationMask(StringMask(mask), 
							 dest));
	       
	       // Clean up
	       mask = dest = "";
	       doingMask = true;
	       continue;
	     case '=': // Mask/dest separator
	       // Check if this is right
	       if (!doingMask) {
		  Daemon::configComplain(firstRun,
				       mask + " mask is invalid (two "
				       "equal signs) in REDIRECT." +
				       subclassName + " class");
		  return;
	       }
	       
	       // Swap!
	       doingMask = false;
	       continue;
	     default:
	       // Work out where to append this char
	       if (doingMask) {
		  mask += (*line)[*pos];
	       } else {
		  dest += (*line)[*pos];
	       }
	    }
	 }

	 // We have returned from that class, reset some variables
	 subclassName = "";
	 break;
       default:
	 // Copy the character into the class name...
	 subclassName += (*line)[*pos];
      }
   }

   // If we hit the end, we failed. Damn.
   Daemon::configComplain(firstRun,
			"Unterminated REDIRECT class in configuration");
   return;
}


/* configSSL
 * Original 17/09/01 simonb
 */
void Daemon::configSSL(ConfigData *conf, String *line, String::size_type *pos, bool firstRun)
{
   /* We can only read this information on the FIRST RUN as it could screw up
    * any SSL connections being run at this point in time
    */
   if (firstRun) {
#ifndef HAVE_OPENSSL
      Daemon::configWarning(firstRun, 
			  "Ignoring SSL class - No SSL support has been "
			  "compiled");
#else
      String command = "", parameter = "";
      bool doingCommand = true;
      
      // Run through the string as far as we need to
      while ((*line)[*pos] != '}') {
	 // Check we have not skipped over the EOL..
	 if (++(*pos) >= line->length()) {
	    Daemon::configComplain(firstRun,
				 "Unterminated SSL class in configuration");
	    return;
	 }
	 
	 // Check this character
	 switch ((*line)[*pos]) {
	  case ';': // End of command marker, process the given command/paramter
	    // Check if this is right..
	    if (doingCommand) {
	       Daemon::configComplain(firstRun,
				    "Semi-colon found in variable name side "
				    "of '=' in SSL class (Missing "
				    "semi-colon?)");
	       return;
	    }
	    
	    // Fix up the strings
	    command = command.toUpper().trimQuotes();
	    parameter = parameter.trimQuotes();
	    
# ifdef DEBUG_PSYCHO_
	    debug(String::printf(" | +-> Variable: %s, Param: %s",
				 (char const *)command,
				 (char const *)parameter));
# endif
	    
	    // Work out what to do with this. This is ugly, but oh well..
	    if ((command == "CERTIFICATE") ||
		(command == "CERT")) {
//	       conf->confServername = parameter;
	    } else {
	       Daemon::configComplain(firstRun,
				    String("Ignoring unknown variable in "
					   "SSL class: ") +
				    command);
	    }
	    
	    // Finish up.
	    doingCommand = true;
	    command = parameter = "";
	    continue;
	  case '=': // Command/Parameter separator
	    // Check if we have not already gotten this..
	    if (!doingCommand) {
	       Daemon::configComplain(firstRun,
				    command +
				    " variable is invalid (two equal signs) "
				    "in SSL class");
	       return;
	    }
	    
	    // We are now reading to the parameter
	    doingCommand = false;
	    continue;
	  default:
	    // Where do we append this char to?
	    if (doingCommand) {
	       command += (*line)[*pos];
	    } else {
	       parameter += (*line)[*pos];
	    }
	 }
      }
#endif
   } 
#ifdef HAVE_OPENSSL
   else {
#endif
      
      int c = 1;
      (*pos)++;
      
      // Run over the line
      while (c > 0) {
	 switch ((*line)[*pos]) {
	  case '{':
	    c++;
	    break;
	  case '}':
	    c--;
	    break;
	 }
	 
	 // Check we have not skipped over the EOL..
	 if (++(*pos) >= line->length()) {
	    Daemon::configComplain(firstRun,
				 "Unterminated SSL class in configuration");
	    return;
	 }
      }
#ifdef HAVE_OPENSSL
   }
#endif
}


/* configSTATUS
 * Original  simonb
 */
void Daemon::configSTATUS(ConfigData *conf, String *line, String::size_type *pos, bool firstRun)
{
   while ((*line)[++(*pos)] != '}');
}
