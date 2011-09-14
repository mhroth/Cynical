Cynical
=======

Overview
--------

Cynical is a Native Client (NaCl) wrapper for the ZenGarden Pure Data (Pd) runtime. It allows Pd patches to be run in the Chrome webbrowser and exposes a rich JavaScript API for creating, querying, and modifying them in real time.

How to Get Started
------------------

  + Download the NaCl SDK.  http://code.google.com/chrome/nativeclient/docs/download.html

  + You might as well read the tutorial. http://code.google.com/chrome/nativeclient/docs/tutorial.html

  + Be sure to read the requirements to run NaCl plugins in Chrome. http://code.google.com/chrome/nativeclient/docs/running.html

  + You must be using at least Chrome v13, and have Native Client enabled in about:flags and about:plugins.

  + Build the project with scons.

  + ...

Where the Action is
-------------------

All of the wrapper code is currently here: https://github.com/mhroth/Cynical/blob/master/zgnacl/zgnacl.cpp

Build the project with scons.
------------------

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

Testing your plug-in in the browser
------------------

  + Create a symlink to your zgnacl directory in the examples folder of the NaCl SDK
	+ ln -s /Users/[yourusername]/[pathto]/zgnacl /Users/[yourusername]/[pathto]/native_client_sdk_0_5_1052/examples

  + Start the NaCl web server
	+ python /Users/[yourusername]/[pathto]/native_client_sdk_0_5_1052/examples/httpd.py
	+ note down the port number

  + Run your plug-in in Chrome
	+ go to http://localhost:[portnumber] (from previous step)
	+ your zgnacl project should appear here, along with the other example projects
	+ if it does not, simply append /zgnacl/zgnacl.html to http://localhost:[portnumber]

Native Client docs
------------------

http://code.google.com/p/nativeclient/

http://www.chromium.org/nativeclient/native-client-documentation-index

http://code.google.com/chrome/nativeclient/

http://code.google.com/chrome/nativeclient/docs/technical_overview.html