#!/usr/bin/python
# -*- coding: utf-8 -*-

__docformat__ = "reStructuredText"

import os
import sys
import socket
import logging
import subprocess
import BaseHTTPServer
import SimpleHTTPServer
from ConfigParser import ConfigParser

logging.getLogger().setLevel(logging.INFO)


def run(addr, server_class, handler_class):
    """Run a HTTP Server"""

    ## Create HTTP server instance
    httpd = server_class(addr, handler_class)
    logging.info("Starting local server on http://%s:%d" % addr)
    logging.info("To shut down send press CRTL+C.")

    ## Read config.cfg to get Chrome bin and open URL in new window
    conf = ConfigParser()
    conf.read(["config.cfg"])
    cmd = [
        conf.get("chrome","bin"),
        '--new-window',
        '--url=http://%s/' % ":".join([str(s) for s in addr]),
        ]
    ## Does not work yet ...
    #import subprocess
    #subprocess.call(cmd)

    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        ## Catch keyboard interrupt
        logging.info("Received keyboard interrupt.")
        httpd.server_close()

    logging.info("Shutting down local server.")


def runserver(port=8000):
    """Configure a simple HTTP server that serves the current directory"""

    try:
        hostname = socket.gethostname()
        hostname = socket.gethostbyaddr(hostname)[0]
    except (socket.gaierror,socket.herror):
        hostname = 'localhost'

    addr = (hostname, int(port))

    conf = ConfigParser()
    conf.read(["config.cfg"])
    path_to_sdk = conf.get("sdk","directory")
    sdk_version = conf.get("sdk","version")

    logging.info("SDK_PATH: %s" % path_to_sdk)
    logging.info("SDK_VERSON: %s" % sdk_version)

    run(addr,
        server_class=BaseHTTPServer.HTTPServer,
        handler_class=SimpleHTTPServer.SimpleHTTPRequestHandler)

    sys.exit(0)


if __name__ == "__main__":
    runserver(*sys.argv[1:])
