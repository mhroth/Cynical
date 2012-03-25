# Cynical Overview
Cynical is a [Native Client](https://developers.google.com/native-client/) (NaCl) wrapper for the [ZenGarden](https://github.com/mhroth/ZenGarden) [Pure Data](http://puredata.info/) (Pd) runtime. It allows Pd patches to be run in the [Google Chrome](https://www.google.com/chrome/) webbrowser and exposes a simple JavaScript API for creating, querying, and modifying them in real time.

# How to Get Started
  + Download the NaCl SDK.  http://code.google.com/chrome/nativeclient/docs/download.html

  + You might as well read the tutorial. http://code.google.com/chrome/nativeclient/docs/tutorial.html

  + Be sure to read the requirements to run NaCl plugins in Chrome. http://code.google.com/chrome/nativeclient/docs/running.html

  + You must be using at least Chrome v13, and have Native Client enabled in about:flags and about:plugins.

  + Build the project with scons.

  + ...

# The JavaScript API
There is currently a very primitive JavaScript API available. Cynical is limited to loading a patch (with *no* abstractions), and sending arbitrary messages to arbitrary receivers. It is not currenly possible to send messages back to JavaScipt, for example via the [print] object or external receivers.


## newGraph
A single graph can be easily loaded with new `newGraph` command. The netlist (as written in a .pd file) is provided as is, and will be parsed by ZenGarden.

```Javascript
function newGraph(netlist) {
  ZgnaclModule.postMessage("newGraph:" + netlist);
}
```

## sendMessage
Messages may be sent to the graph with the `sendMessage` command. The receiver name, timestamp at which the message should be delivered, and the message formatted as a string are required arguments. In practice the timestamp should always be zero (i.e. `0.0`) such that the message is delivered immediately. The message string can be anything that describes a valid Pd message. Examples include:

  + `0 0 0`: a message with three floats, each equal to zero.
  + `hello world 0`: a message with three elements, the first two are strings and the last one is a float.
  + `!`: a message with one bang element

```Javascript
function sendMessage(receiverName, timestamp, messageString) {
  ZgnaclModule.postMessage("sendMessage:" + receiverName + ":" + timestamp + ":" + messageString);
}
```

## play & pause
Audio can be easily started and stopped via the `play` and `pause` commands, their function being self-explanatory. Note that when Cynical is paused, neither audio nor messages are processed. Cynical will behave as if time were standing still. If some kind of mute functionality is desired, while messages continue to be processed, this must be build directly into the patch.

```Javascript
function play() {
  ZgnaclModule.postMessage("play");
}

function pause() {
  ZgnaclModule.postMessage("pause");
}
```

## An Example

```Javascript
// TODO(mhroth)
```

# Restrictions and Limitations
Currenty there are a number of important limitations to the implementation. These are due to technical hurdles which can be overcome, with the exception of the last point.

  + <del>Messages cannot be sent from ZenGarden to JavaScript. This includes the output of objects such as [print], or messages sent to external receivers.</del>
    + This issue is no longer a problem. Arbitrary messages may be sent to ZenGarden from JavaScript (forward direction), and from ZenGarden to JavaScript (reverse direction). In order to receive messages sent in the reverse direction, a receiver name must be registered with ZenGarden. Print statements will also be sent to JavaScript.
  + The loaded patch cannot refer to any abstractions. Subpatches are acceptable.
  + Binary assets, such as samples, cannot be loaded.
  + Microphone or line input is not available. This is unlikely to change soon, if at all, and is due to security issues raised by allowing automatic access to the system microphone in the browser.

Cynical is currently best suited for making synthesizers.

# Where the Action is

All of the wrapper code is currently here: https://github.com/mhroth/Cynical/blob/master/zgnacl/zgnacl.cpp

# Build the project with scons.

  + Download the NaCl SDK.  http://code.google.com/chrome/nativeclient/docs/download.html

  + Ensure you have downloaded the ZenGarden submodule into your zgnacl directory
  + (Whilst cd'd into your zgnacl directory) git submodule init
	+ git submodule update
	+ if you get 'Permission denied (publickey), follow the following instructions http://help.github.com/linux-key-setup/

  + In your zgnacl directory edit 'scons' (using vi or nano or any other text editor)
	+ vi scons
	+ edit the line beginning "export NACL_SDK_ROOT=/Users…" to reflect the path to the NaCl SDK on your machine e.g. export NACL_SDK_ROOT=/Users/[yourusername]/[pathto]/native_client_sdk_0_5_1052
	+ exit your text editor

  + Check this has worked by cleaning the target repository
	+ ./scons -c

  + To build
	+ ./scons
	+ You should now see the files being compiled in your terminal window (may take 5-10 minutes)
	+ Once that has finished there should be (among others) four .nexe files in your zgnacl directory

# Testing your plug-in in the browser

  + Create a symlink to your zgnacl directory in the examples folder of the NaCl SDK
	+ ln -s /Users/[yourusername]/[pathto]/zgnacl /Users/[yourusername]/[pathto]/native_client_sdk_0_5_1052/examples

  + Start the NaCl web server
	+ python /Users/[yourusername]/[pathto]/native_client_sdk_0_5_1052/examples/httpd.py
	+ note down the port number

  + Run your plug-in in Chrome
	+ go to http://localhost:[portnumber] (from previous step)
	+ your zgnacl project should appear here, along with the other example projects
	+ if it does not, simply append /zgnacl/zgnacl.html to http://localhost:[portnumber]

# Native Client docs

http://code.google.com/p/nativeclient/

http://www.chromium.org/nativeclient/native-client-documentation-index

http://code.google.com/chrome/nativeclient/

http://code.google.com/chrome/nativeclient/docs/technical_overview.html