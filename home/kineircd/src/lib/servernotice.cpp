/* servernotice.cpp
 * Little extra stuff to control server notices
 */

#include "autoconf.h"

#include "servernotice.h"

// The number of server notice types - must be in sync with the table below
const unsigned int ServerNotice::NUM_TYPES = 18;

// The server notice type to type-char table, for mode changes and output
struct ServerNotice::typeTableStruct const ServerNotice::typeTable[] = {
     { 'b',	ServerNotice::SN_BURST },
     { 'D', 	ServerNotice::SN_DEBUG },
     { 'G',	ServerNotice::SN_GLOBOPS },
     { 'h',	ServerNotice::SN_HELPME },
     { 'H',	ServerNotice::SN_HTM },
     { 'I',	ServerNotice::SN_IPMISMATCH },
     { 'j',	ServerNotice::SN_NETJOIN_MINOR },
     { 'J',	ServerNotice::SN_NETJOIN_MAJOR },
     { 'k',	ServerNotice::SN_KILL },
     { 'L',	ServerNotice::SN_LOCOPS },
     { 'n',	ServerNotice::SN_NETSPLIT_MINOR },
     { 'N',	ServerNotice::SN_NETSPLIT_MAJOR },
     { 'o',	ServerNotice::SN_OPERING_LOCAL },
     { 'O',	ServerNotice::SN_OPERING_GLOBAL },
     { 's',	ServerNotice::SN_SIGNON },
     { 'S',	ServerNotice::SN_SIGNOFF },
     { 'z',	ServerNotice::SN_SERVER },
     { 'Z',	ServerNotice::SN_HOUSEKEEPING },
     { 0 }
};
