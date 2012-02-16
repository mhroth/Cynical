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

// just post everything that comes out of ZenGarden to the browser
void *zgCallbackFunction(ZGCallbackFunction function, void *userData, void *ptr) {
  ZgnaclInstance *zgnaclInstance = (ZgnaclInstance *) userData;
  switch (function) {
    case ZG_PRINT_STD: {
      zgnaclInstance->PostMessage(pp::Var((const char *) ptr));
      break;
    }
    case ZG_PRINT_ERR: {
      char buffer[snprintf(NULL, 0, "ERROR: %s", (const char *) ptr)+1];
      snprintf(buffer, sizeof(buffer), "ERROR: %s", (const char *) ptr);
      zgnaclInstance->PostMessage(pp::Var(buffer));
      break;
    }
    case ZG_RECEIVER_MESSAGE: {
      ZGReceiverMessagePair *pair = (ZGReceiverMessagePair *) ptr;
      const char *receiverName = pair->receiverName;
      ZGMessage *message = pair->message;
      char *messageString = zg_message_to_string(message);
      char buffer[snprintf(NULL, 0, "receiveMessage:%s:%f:%s",
          receiverName, zg_message_get_timestamp(message), messageString)+1];
      snprintf(buffer, sizeof(buffer), "receiveMessage:%s:%f:%s",
          receiverName, zg_message_get_timestamp(message), messageString);
      zgnaclInstance->PostMessage(pp::Var(buffer));
      free(messageString);
      break;
    }
    default: {
      zgnaclInstance->PostMessage(pp::Var("Received unknown callback function."));
      break;
    }
  }
  
  return NULL;
}
