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

#include "kineircd/clbp/input.h"
#include "kineircd/clbp/output.h"

using namespace Kine::LibCLBP;


/* handleInput - Handle incoming data
 * Original 11/08/2001 simonb
 */
void Input::handleInput(std::stringstream& data)
{
   std::string::size_type pos = 0;
   
   for (;;) {
      // Find the next \r or \n
      std::string::size_type eolPos = 
	data.str().find_first_of(Output::EOL_CR_LF, pos);
      
      // Is this an incomplete line? (no termination in the given input)
      if (eolPos == std::string::npos) {
	 // Buffer what we were given and simply return
	 inputQueue += data.str();
	 return;
      }
      
      // If the input queue is empty, we can skip throwing the string around
      if (inputQueue.empty()) {
	 // Throw it to the line parser..
	 parseLine(data.str().substr(pos, eolPos));
      } else {
	 // Throw the input queue *and* the broken apart data to the parser
	 parseLine(inputQueue + data.str().substr(pos, eolPos));
	 
	 // Clear the input queue
	 inputQueue.clear();
      }
      
      // Reposition the starting position
      pos = eolPos + 1;
      
      // Check if this is a \r\n sequence
      if ((data.str()[pos - 1] == '\r') &&
	  (data.str()[pos] == '\n')) {
	 // Skip the next char too!
	 ++pos;
      }
      
      // If the position is at the end of the string, break
      if (pos >= data.str().length()) {
	 return;
      }
   }
}
