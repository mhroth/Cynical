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
