/* $Id$
 * 
 * Copyright (c) 2001,2002,2003 Simon Butcher <pickle@alien.net.au>
 * Copyright (c) 2001,2002,2003 KineIRCd Development Team
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

#ifdef HAVE_CONFIG_H
# include "autoconf.h"
#endif

#include "kineircd/clbp/output.h"

using namespace Kine::LibCLBP;


// End of line characters, used for reference to avoid them being repeated lots
const char* const Output::EOL_CR_LF	= "\r\n";
const char* const Output::EOL_CR	= "\r";
const char* const Output::EOL_LF	= "\n";


/* withdrawOutput - Remove up to the amount given of octets from the queue
 * Original 28/09/2002 simonb
 * Note: This could be more efficient :(
 */
std::string Output::withdrawOutput(AIS::Util::Socket::blockSize_type amount)
{
   std::string output;
   
   /* Append as much data as we can to the output without breaking the amount
    * limit we were given
    */
   while (!outputQueue.empty() && (outputQueue.front().size() <= amount)) {
      output += outputQueue.front();
      amount -= outputQueue.front().size();
      outputQueue.pop();
   }
   
   // Is there anything left we might also be able to send?
   if (!outputQueue.empty() && (amount > 0)) {
      --amount;
      output += outputQueue.front().substr(0, amount);
      outputQueue.front().erase(0, amount);
   }
   
   // Return the output
   return output;
}
