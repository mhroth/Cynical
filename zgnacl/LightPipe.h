/*
 *  Copyright 2012 Martin Roth
 *            mhroth@gmail.com
 * 
 *  This file is part of Cynical.
 *
 *  Cynical is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Cynical is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *  
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with Cynical.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef _LIGHT_PIPE_
#define _LIGHT_PIPE_

class LightPipe {
  
  public:
    /** Create a shared memory buffer of length numBytes. */
    LightPipe(unsigned int numKBytes);
    ~LightPipe();
  
    void write(int numBytes, const char *buffer);
  
    /** Reads into the given buffer. Returns the number of bytes read. */
    int read(int numBytes, char *buffer);
    
  private:
    unsigned long numMemoryBytes;
    char *writePos;
    char *readPos;
    char *memory;
};

#endif // _LIGHT_PIPE_