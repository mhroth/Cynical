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

#define kPlaySoundId "playSound"
#define kStopSoundId "stopSound"
#define kMessageArgumentSeparator ':'

bool ZgnaclInstance::Init(uint32_t argc, const char* argn[], const char* argv[]) {
  
  // Ask the device for an appropriate block size, with a default of 512
  blockSize_ = pp::AudioConfig::RecommendSampleFrameCount(PP_AUDIOSAMPLERATE_44100, 512);
  audio_ = pp::Audio(this, pp::AudioConfig(this, PP_AUDIOSAMPLERATE_44100, blockSize_),
      audioCallback, this);
  
  // create the ZGContext with 0 input and 2 output channels
  zgContext_ = zg_context_new(0, 2, blockSize_, 44100.0f, zgCallbackFunction, this);
  
  // register an external receiver
  zg_context_register_receiver(zgContext_, "#PATCH_TO_WEB");
  
  // TODO(mhroth): initialise the light pipe here
  pipe_ = new LightPipe(32);
  
  // kick off the LightPipe reader
  zgReadAndProcessPipe(this, 0);
  
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
          // NOTE(mhroth): reinterpret_cast<int32_t> could cause problems with 64-bit systems. It
          // is intended that this returned value can be used to refer to the specific according to
          // its memory address.
        } else {
          PostMessage(pp::Var("Graph could not be created. Reason unknown."));
        }
      } else if (!message.compare(0, pos, "sendMessage")) {
        // receiver:timestamp:arguments
        // "recName:0.0:0 0"

        // first argument is receiver
        size_t pos2 = message.find_first_of(kMessageArgumentSeparator, pos+1);
        string receiverName = message.substr(pos+1, pos2-pos-1);
        
        // second argument is timestamp
        size_t pos3 = message.find_first_of(kMessageArgumentSeparator, pos2+1);
        string timestamp = message.substr(pos2+1, pos3-pos2-1);

        // third argument is message description
        string initString = message.substr(pos3+1, string::npos);
        
        // send the message into ZenGarden
        zg_context_send_message_from_string(zgContext_, receiverName.c_str(),
            strtod(timestamp.c_str(), NULL), initString.c_str());
      } else {
        PostMessage(var_message); // if we mess something up, return the input
      }
    } else {
      PostMessage(var_message); // if we mess something up, return the input
    }
  }
}

/*
 * The Module class.  The browser calls the CreateInstance() method to create
 * an instance of your NaCl module on the web page.  The browser creates a new
 * instance for each <embed> tag with type="application/x-nacl".
 */
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
} // end namespace pp
