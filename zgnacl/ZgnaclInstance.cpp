#include "ZgnaclInstance.h"
#include "zgcallback.h"

#define kPlaySoundId "playSound"
#define kStopSoundId "stopSound"
#define kMessageArgumentSeparator ':'

#define kSampleFrameCount 512 // The preferred block size
#define kNumInputChannels 0
#define kNumOutputChannels 2

bool ZgnaclInstance::Init(uint32_t argc, const char* argn[], const char* argv[]) {
  
  // Ask the device for an appropriate sample count size.
  blockSize_ = pp::AudioConfig::RecommendSampleFrameCount(PP_AUDIOSAMPLERATE_44100, kSampleFrameCount);
  audio_ = pp::Audio(this, pp::AudioConfig(this, PP_AUDIOSAMPLERATE_44100, blockSize_),
      audioCallback, this);
  
  // create the ZGContext
  zgContext_ = zg_context_new(kNumInputChannels, kNumOutputChannels, blockSize_, 44100.0f, zgCallbackFunction, this);
  
  // register an external receiver
  zg_context_register_receiver(zgContext_, "#PATCH_TO_WEB");
  
  return true;
}

void ZgnaclInstance::audioCallback(void *samples, uint32_t buffer_size, void *data) {
  ZgnaclInstance* zgnaclInstance = reinterpret_cast<ZgnaclInstance*>(data);
  
  // samples are channel interleaved shorts
  short *buffer = (short *) samples;
  
  zg_context_process_s(zgnaclInstance->zgContext(), buffer, buffer);
}

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
    snprintf(stringBuffer, sizeof(stringBuffer), "blocksize: %i", blockSize_);
    zgCallbackFunction(ZG_PRINT_STD, this, stringBuffer);
  } else { // process functions with arguments
    size_t pos = message.find_first_of(kMessageArgumentSeparator); // find position of first argument
    if (pos != string::npos) {
      if (!message.compare(0, pos, "newGraph")) {
        string graphDesc = message.substr(pos+1, string::npos); // get the graph description
        ZGGraph *graph = zg_context_new_graph_from_string(zgContext_, graphDesc.c_str());
        if (graph != NULL) {
          zg_graph_attach(graph);
          PostMessage(pp::Var(reinterpret_cast<int32_t>(graph))); // return the id of the new graph
        } else {
          PostMessage(pp::Var("Graph could not be created. Reason unknown."));
        }
      } else {
        // if we mess something up, return the input
        PostMessage(var_message);
      }
    } else {
      // if we mess something up, return the input
      PostMessage(var_message);
    }
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
