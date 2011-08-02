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

Native Client docs
------------------

http://code.google.com/p/nativeclient/

http://www.chromium.org/nativeclient/native-client-documentation-index

http://code.google.com/chrome/nativeclient/

http://code.google.com/chrome/nativeclient/docs/technical_overview.html