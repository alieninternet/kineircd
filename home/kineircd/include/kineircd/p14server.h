/* p14server.h
 * The handler class for the P14 server protocol
 */

#ifdef HAVE_P14SERVER_PROTOCOL
# ifndef __P14SERVER_H_
#  define __P14SERVER_H_

#  include "handler.h"

// P14-handler sub-class
class p14serverHandler : public Handler {
 private:
   Server *server;				// Server data
   
   struct functionTableStruct {
      void (*function)(p14serverHandler *handler, String const &origin, 
		       StringTokens *tokens);	// Parsing handler function
   }; 
   static struct functionTableStruct const functionsTable[];
   
 public:
   p14serverHandler(Connection *, Server *);	// Constructor
   ~p14serverHandler(void);			// Destructor
};

# endif
#endif
