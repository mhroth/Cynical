/// @file zgnacl.cpp
/// This example demonstrates loading, running and scripting a very simple NaCl
/// module.  To load the NaCl module, the browser first looks for the
/// CreateModule() factory method (at the end of this file).  It calls
/// CreateModule() once to load the module code from your .nexe.  After the
/// .nexe code is loaded, CreateModule() is not called again.
///
/// Once the .nexe code is loaded, the browser than calls the CreateInstance()
/// method on the object returned by CreateModule().  It calls CreateInstance()
/// each time it encounters an <embed> tag that references your NaCl module.
///
/// The browser can talk to your NaCl module via the postMessage() Javascript
/// function.  When you call postMessage() on your NaCl module from the browser,
/// this becomes a call to the HandleMessage() method of your pp::Instance
/// subclass.  You can send messages back to the browser by calling the
/// PostMessage() method on your pp::Instance.  Note that these two methods
/// (postMessage() in Javascript and PostMessage() in C++) are asynchronous.
/// This means they return immediately - there is no waiting for the message
/// to be handled.  This has implications in your program design, particularly
/// when mutating property values that are exposed to both the browser and the
/// NaCl module.

#include <cstdio>
#include <string>
#include "ppapi/cpp/audio.h"
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/var.h"

#include "ZenGarden.h"
#include "ZGCallbackFunction.h"

using namespace std;

#define kPlaySoundId "playSound"
#define kStopSoundId "stopSound"
#define kMessageArgumentSeparator ':'

#define kSampleFrameCount 512 // The sample count we will request.
#define kNumInputChannels 2
#define kNumOutputChannels 2

extern "C" {
  extern void zgCallbackFunction(ZGCallbackFunction, void *, void *);
}

// Note to the user: This glue code reflects the current state of affairs.  It
// may change.  In particular, interface elements marked as deprecated will
// disappear sometime in the near future and replaced with more elegant
// interfaces.  As of the time of this writing, the new interfaces are not
// available so we have to provide this code as it is written below.

/// The Instance class.  One of these exists for each instance of your NaCl
/// module on the web page.  The browser will ask the Module object to create
/// a new Instance for each occurence of the <embed> tag that has these
/// attributes:
///     type="application/x-nacl"
///     src="zgnacl.nmf"
/// To communicate with the browser, you must override HandleMessage() for
/// receiving messages from the borwser, and use PostMessage() to send messages
/// back to the browser.  Note that this interface is entirely asynchronous.
class ZgnaclInstance : public pp::Instance {
 public:
  /// The constructor creates the plugin-side instance.
  /// @param[in] instance the handle to the browser-side plugin instance.
  explicit ZgnaclInstance(PP_Instance instance) : pp::Instance(instance)
  {}
  virtual ~ZgnaclInstance() {}
  
  // Called by the browser once the NaCl module is loaded and ready to
  // initialize.  Creates a Pepper audio context and initializes it. Returns
  // true on success.  Returning false causes the NaCl module to be deleted and
  // no other functions to be called.
  virtual bool Init(uint32_t argc, const char* argn[], const char* argv[]);

  /// Handler for messages coming in from the browser via postMessage().  The
  /// @a var_message can contain anything: a JSON string; a string that encodes
  /// method names and arguments; etc.  For example, you could use
  /// JSON.stringify in the browser to create a message that contains a method
  /// name and some parameters, something like this:
  ///   var json_message = JSON.stringify({ "myMethod" : "3.14159" });
  ///   nacl_module.postMessage(json_message);
  /// On receipt of this message in @a var_message, you could parse the JSON to
  /// retrieve the method name, match it to a function call, and then call it
  /// with the parameter.
  /// @param[in] var_message The message posted by the browser.
  virtual void HandleMessage(const pp::Var& var_message);
  
  // property accessor
  ZGContext *zgContext() const { return zgContext_; }
  
  // property accessor
  uint32_t blockSize() const { return blockSize_; }
  
 private:
  static void ZenGardenCallback(void* samples, uint32_t buffer_size, void* data) {
    ZgnaclInstance* zgnaclInstance = reinterpret_cast<ZgnaclInstance*>(data);
    
    // samples are channel interleaved shorts
    short *buffer = (short *) samples;
    
    zg_context_process_s(zgnaclInstance->zgContext(), buffer, buffer);
  }
  
  pp::Audio audio_;
  
  // The count of sample frames per channel in an audio buffer.
  uint32_t blockSize_;
  
  ZGContext *zgContext_;
};

bool ZgnaclInstance::Init(uint32_t argc, const char* argn[], const char* argv[]) {
  
  // Ask the device for an appropriate sample count size.
  blockSize_ = pp::AudioConfig::RecommendSampleFrameCount(PP_AUDIOSAMPLERATE_44100, kSampleFrameCount);
  audio_ = pp::Audio(this, pp::AudioConfig(this, PP_AUDIOSAMPLERATE_44100, blockSize_),
      ZenGardenCallback, this);
  
  zgContext_ = zg_context_new(kNumInputChannels, kNumOutputChannels, blockSize_, 44100.0f, zgCallbackFunction, this);
  zg_context_register_receiver(zgContext_, "#PATCH_TO_WEB");
  
  // construct a very basic osc~ graph. Only for testing.
  ZGGraph *zgGraph = zg_context_new_empty_graph(zgContext_);
  ZGObject *zgOscObject = zg_graph_add_new_object(zgGraph, "osc~ 440", 0.0f ,0.0f);
  ZGObject *zgDacObject = zg_graph_add_new_object(zgGraph, "dac~", 0.0f, 0.0f);
  zg_graph_add_connection(zgGraph, zgOscObject, 0, zgDacObject, 0);
  zg_graph_add_connection(zgGraph, zgOscObject, 0, zgDacObject, 1);
  zg_graph_attach(zgGraph);
  
  return true;
}

// Called by the browser to handle the postMessage() call in Javascript.
// |var_message| is expected to be a string that contains the name of the
// method to call.  Note that the setFrequency method takes a single
// parameter, the frequency.  The frequency parameter is encoded as a string
// and appended to the 'setFrequency' method name after a ':'.  Examples
// of possible message strings are:
//     playSound
//     stopSound
//     setFrequency:880
// If |var_message| is not a recognized method name, this method does nothing.
void ZgnaclInstance::HandleMessage(const pp::Var& var_message) {
  if (!var_message.is_string()) {
    return;
  }
  std::string message = var_message.AsString();
  if (message == kPlaySoundId) {
    audio_.StartPlayback();
  } else if (message == kStopSoundId) {
    audio_.StopPlayback();
  } else if (message == "info") {
    // NOTE(mhroth): temporary for testing
    char stringBuffer[32];
    snprintf(stringBuffer, 32, "blocksize: %i", blockSize_);
    zgCallbackFunction(ZG_PRINT_STD, this, stringBuffer);
  } else {
    // if we mess something up, return the input
    PostMessage(var_message);
  }
}

/// The Module class.  The browser calls the CreateInstance() method to create
/// an instance of your NaCl module on the web page.  The browser creates a new
/// instance for each <embed> tag with type="application/x-nacl".
class ZgnaclModule : public pp::Module {
 public:
  ZgnaclModule() : pp::Module() {}
  virtual ~ZgnaclModule() {}

  /// Create and return a ZgnaclInstance object.
  /// @param[in] instance The browser-side instance.
  /// @return the plugin-side instance.
  virtual pp::Instance* CreateInstance(PP_Instance instance) {
    return new ZgnaclInstance(instance);
  }
};

namespace pp {
/// Factory function called by the browser when the module is first loaded.
/// The browser keeps a singleton of this module.  It calls the
/// CreateInstance() method on the object you return to make instances.  There
/// is one instance per <embed> tag on the page.  This is the main binding
/// point for your NaCl module with the browser.
Module* CreateModule() {
  return new ZgnaclModule();
}
}  // namespace pp

// just post everything that comes out of ZenGarden to the browser
extern "C" {
  void zgCallbackFunction(ZGCallbackFunction function, void *userData, void *ptr) {
    switch (function) {
      case ZG_PRINT_STD: {
        ZgnaclInstance *zgnaclInstance = (ZgnaclInstance *) userData;
        zgnaclInstance->PostMessage((const char *) ptr);
        break;
      }
      case ZG_PRINT_ERR: {
        ZgnaclInstance *zgnaclInstance = (ZgnaclInstance *) userData;
        string errorStr = string("ERROR: ");
        string str = string((const char *) ptr);
        zgnaclInstance->PostMessage(pp::Var(errorStr + str));
        break;
      }
      case ZG_RECEIVER_MESSAGE: {
        ZgnaclInstance *zgnaclInstance = (ZgnaclInstance *) userData;
        ZGReceiverMessagePair *pair = (ZGReceiverMessagePair *) ptr;
        const char *receiverName = pair->receiverName;
        ZGMessage *message = pair->message;
        zgnaclInstance->PostMessage(pp::Var(string(zg_message_get_symbol(0, message))));
        break;
      }
      default: {
        break;
      }
    }
  }
};