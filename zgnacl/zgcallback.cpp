/*
 *  Copyright 2011,2012 Martin Roth
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

#include "ZgnaclInstance.h"
#include "zgcallback.h"
#include "ppapi/cpp/completion_callback.h"

#define PIPE_READER_INTERVAL_MS (1000/20) // read pipe 20 times per second

void *zgCallbackFunction(ZGCallbackFunction function, void *userData, void *ptr) {
  ZgnaclInstance *zgnacl = reinterpret_cast<ZgnaclInstance *>(userData);
  switch (function) {
    case ZG_PRINT_STD: {
      size_t ptrLen = strlen((const char *) ptr);
      char buffer[sizeof(function)+ptrLen+1];
      *((ZGCallbackFunction *) buffer) = function;
      memcpy(buffer+sizeof(function), ptr, ptrLen);
      zgnacl->getPipe()->write(sizeof(buffer), buffer);
      break;
    }
    case ZG_PRINT_ERR: {
      size_t ptrLen = strlen((const char *) ptr);
      char buffer[sizeof(function)+ptrLen+1];
      *((ZGCallbackFunction *) buffer) = function;
      memcpy(buffer+sizeof(function), ptr, ptrLen);
      zgnacl->getPipe()->write(sizeof(buffer), buffer);
      break;
    }
    case ZG_RECEIVER_MESSAGE: {
      ZGReceiverMessagePair *pair = reinterpret_cast<ZGReceiverMessagePair *>(ptr);
      const char *receiverName = pair->receiverName;
      ZGMessage *message = pair->message;
      char *messageString = zg_message_to_string(message);
      char buffer[sizeof(function)+snprintf(NULL, 0, "%s:%f:%s",
          receiverName, zg_message_get_timestamp(message), messageString)+1];
      *((ZGCallbackFunction *) buffer) = function;
      snprintf(buffer+sizeof(function), sizeof(buffer)-sizeof(function), "%s:%f:%s",
          receiverName, zg_message_get_timestamp(message), messageString);
      zgnacl->getPipe()->write(sizeof(buffer), buffer);
      free(messageString);
      break;
    }
    default: {
      char buffer[sizeof(function)+snprintf(NULL, 0,
          "Received unknown callback function: %i", function)+1];
      *((ZGCallbackFunction *) buffer) = function;
      snprintf(buffer+sizeof(function), sizeof(buffer)-sizeof(function),
          "Received unknown callback function: %i", function);
      zgnacl->getPipe()->write(sizeof(buffer), buffer);
      break;
    }
  }
  
  return NULL;
}

// this function is called on the main thread
void zgReadAndProcessPipe(void *user_data, int32_t result) {
  ZgnaclInstance *zgnacl = reinterpret_cast<ZgnaclInstance *>(user_data);
  int numBufferBytes = 1024;
  char buffer[numBufferBytes];
  int bytesRead = 0;
  // TODO(mhroth): if the buffer is too small to read the object, reallocate it larger 
  while ((bytesRead = zgnacl->getPipe()->read(numBufferBytes, buffer)) > 0 && bytesRead <= numBufferBytes) {
    ZGCallbackFunction function = *((ZGCallbackFunction *) buffer);
    switch (function) {
      case ZG_PRINT_STD: {
        char str[snprintf(NULL, 0, "printStd:%s", buffer+sizeof(function))+1];
        snprintf(str, sizeof(str), "printStd:%s", buffer+sizeof(function));
        zgnacl->PostMessage(pp::Var(str));
        break;
      }
      case ZG_PRINT_ERR: {
        char str[snprintf(NULL, 0, "printErr:%s", buffer+sizeof(function))+1];
        snprintf(str, sizeof(str), "printErr:%s", buffer+sizeof(function));
        zgnacl->PostMessage(pp::Var(str));
        break;
      }
      case ZG_RECEIVER_MESSAGE: {
        char str[snprintf(NULL, 0, "receiveMessage:%s", buffer+sizeof(function))+1];
        snprintf(str, sizeof(str), "receiveMessage:%s", buffer+sizeof(function));
        zgnacl->PostMessage(pp::Var(str));
        break;
      }
      default: {
        zgnacl->PostMessage(pp::Var("Unknown function."));
        break;
      }
    }
  }
  
  pp::Module::Get()->core()->CallOnMainThread(PIPE_READER_INTERVAL_MS,
      pp::CompletionCallback(zgReadAndProcessPipe, user_data), 0);
}
