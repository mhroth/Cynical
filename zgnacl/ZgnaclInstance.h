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

#ifndef _ZGNACL_INSTANCE_H_
#define _ZGNACL_INSTANCE_H_

#include <cstdio>
#include <string>
#include "ppapi/cpp/audio.h"
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/var.h"
#include "ZenGarden.h"
using namespace std;

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
  explicit ZgnaclInstance(PP_Instance instance) : pp::Instance(instance) {}
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
  static void audioCallback(void *samples, uint32_t buffer_size, void *data);
  
  pp::Audio audio_;
  
  // The count of sample frames per channel in an audio buffer.
  uint32_t blockSize_;
  
  ZGContext *zgContext_;
};

#endif // _ZGNACL_INSTANCE_H_
