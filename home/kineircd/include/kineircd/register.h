/* register.h
 * The handler class for the registration handler
 * 
 * Copyright (c) 2001,2002 KineIRCd Development Team
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

#ifndef _INCLUDE_KINEIRCD_REGISTER_H_
# define _INCLUDE_KINEIRCD_REGISTER_H_

# include "kineircd/handler.h"

// Register-handler sub-class
class registerHandler : public Handler {
 private:
   String password;				// Logon password
# ifdef ALLOW_CLIENT_CONNECTIONS
   String nickname;				// Nickname (client)
# endif
   String username;				// Username (server & client)
   String hostname;				// Host name (server & client)
   String realname;				// Client Real Name/Server description
# ifdef ALLOW_SERVICE_CONNECTIONS
   String distribution;				// Visibility range (service)
# endif
# ifdef USER_CONNECTION_PINGPONG
   String pingpong;				// Ping/Pong string
   bool gotPong;				// Did we got a pong?
# endif
   String modes;				// Modes for next handler
   int protocol;				// Protocol version number
   time_t startStamp;				// Received start time-stamp
   time_t linkStamp;				// Received link time-stamp
# ifdef MAX_REGISTRATION_LINES
   unsigned char numLines;			// Number of lines received
# endif
   
   enum {
      IN_PROGRESS,			// Waiting for info
      CLIENT,				// User connection
      SERVER,				// Server connection
      SERVICE				// Service connection
   } regmode;					// Registration mode
   
   struct functionTableStruct {
      char const *command;			// Command name
      void (*function)(registerHandler *handler,
		       StringTokens *tokens);	// Parsing handler function
   };
   
   static struct functionTableStruct const functionsTable[];
   
   void sendGeneric(char const *, 
		    String const &) const;	// Generic send routine
   void sendNumeric(Numerics::numeric_t, User *, 
		    String const &) const;	// Minature send-numeric

   // Protocol Input
   static void parseCAPAB(registerHandler *handler, StringTokens *tokens);
   static void parseNICK(registerHandler *handler, StringTokens *tokens);
   static void parsePASS(registerHandler *handler, StringTokens *tokens);
   static void parsePONG(registerHandler *handler, StringTokens *tokens);
   static void parseQUIT(registerHandler *handler, StringTokens *tokens);
# ifdef ALLOW_SERVER_CONNECTIONS
   static void parseSERVER(registerHandler *handler, StringTokens *tokens);
# endif
# ifdef ALLOW_SERVICE_CONNECTIONS
   static void parseSERVICE(registerHandler *handler, StringTokens *tokens);
# endif
# ifdef ALLOW_CLIENT_CONNECTIONS
   static void parseUSER(registerHandler *handler, StringTokens *tokens);
# endif

 public:
   registerHandler(Connection *);		// Constructor
   
   // Protocol Input
   void parseLine(String const &);
};

#endif

