/* irc3user.h
 * The handler class for the IRC-3 user protocol
 */

#ifdef HAVE_PROTOCOL_IRC3USER
# ifndef __IRC3USER_H_
#  define __IRC3USER_H_

#  include "handler.h"

// IRC-3 User-handler sub-class
class irc3userHandler : public Handler {
 private:
   User *user;					// Link to the user data
   
 public:
   irc3userHandler(Connection *, User *, String);	// Constructor
   ~irc3userHandler(void);				// Destructor
};

# endif
#endif
