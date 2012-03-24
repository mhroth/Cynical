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

LightPipe::LightPipe(unsigned int numKBytes) {
  writePos = NULL;
  readPos = NULL;
  numMemoryBytes = numKBytes * 1024 * sizeof(char);
  memory = (char *) malloc(numMemoryBytes);
}

LightPipe::~LightPipe() {
  free(memory);
}

void LightPipe::write(unsigned int numBytes, const char *buffer) {
  // if there isn't enough memory left in the memory to write the buffer, return to the beginning
  if (numMemoryBytes - (writePos - memory) < numBytes) {
    writePos = 0;
  }
  memcpy(writePos+sizeof(numBytes), buffer, numBytes);
  
  // create a memory fence in order to make sure that the ops are not reordered
  asm volatile("" ::: "memory");
  
  char *oldWritePos = writePos;
  writePos += ((numBytes & 0xFFFFFFF0) + 16); // 16-byte is memory alignment?
  *((unsigned int *) oldWritePos) = numBytes;
}

int LightPipe::read(unsigned int numBytes, char *buffer) {
  unsigned int numBytesAtPos = *((unsigned int *) readPos);
  if (numBytesAtPos == 0) return 0;
  if (numBytesAtPos <= numBytes) {
    char *oldReadPos = readPos;
    memcpy(buffer, readPos+sizeof(unsigned int), numBytes);
    
    asm volatile("" ::: "memory");
    
    readPos += ((numBytes & 0xFFFFFFF0) + 16);; // update the read position
    *((unsigned int *) oldReadPos) = 0; // reset the old read position to zero
    return numBytes;
  } else {
    return (numBytesAtPos - numBytes);
  }
}
