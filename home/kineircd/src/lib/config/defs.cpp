/* defs.cpp
 * Configuration file class/variable definitions
 */

#include "autoconf.h"

#include "config.h"

namespace Config {
   // Top definitions
   Config::def_type const Config::topDefs[] = {
	{ 
	   "CONFIG",	
	     NULL,		
	     NULL,
	     def_type::DEF_IGNORE 
	},
	{ NULL }
   };
};
