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

#include <stdlib.h>
#include <string.h>
#include "LightPipe.h"

/*
 * NOTE(mhroth): used for debugging. Remove at earliest convenience.
int main(int argc, char * const argv[]) {
  LightPipe *pipe = new LightPipe(1);
  
  const char *str = "Hello World!";
  pipe->write(strlen(str), str);
  
  char buffer[32];
  pipe->read(32, buffer);
  
  return 0;
}
*/

LightPipe::LightPipe(unsigned int numKBytes) {
  numMemoryBytes = numKBytes * 1024 * sizeof(char);
  memory = (char *) calloc(numMemoryBytes, sizeof(char));
  writePos = readPos = memory; // start read/write positions at the start of the buffer
}

LightPipe::~LightPipe() {
  free(memory);
}

void LightPipe::write(int numBytes, const char *buffer) {
  // if there isn't enough memory left in the memory to write the buffer, return to the beginning
  if (numMemoryBytes - (writePos - memory) < numBytes) {
    *((int *) writePos) = -1; // indicate to read to return to start of buffer
    writePos = memory;
  }
  memcpy(writePos+sizeof(numBytes), buffer, numBytes);
  
  // create a memory fence in order to make sure that the ops are not reordered
  //asm volatile("" ::: "memory");
  
  int *oldWritePos = (int *) writePos;
//  writePos += ((numBytes & 0xFFFFFFF0) + 16); // 16-byte is memory alignment?
  writePos += numBytes + sizeof(numBytes);
  *oldWritePos = numBytes;
}

int LightPipe::read(int numBytes, char *buffer) {
  unsigned int numBytesAtPos = *((unsigned int *) readPos);
  if (numBytesAtPos == 0) return 0;
  if (numBytesAtPos <= numBytes) {
    memcpy(buffer, readPos+sizeof(numBytes), numBytes);
    
    //asm volatile("" ::: "memory");
    
//    readPos += ((numBytes & 0xFFFFFFF0) + 16);; // update the read position
    readPos += numBytesAtPos + sizeof(numBytes);
    if (*((int *) readPos) == -1) readPos = memory;
    return numBytesAtPos;
  } else {
    return (numBytesAtPos - numBytes);
  }
}
