#!/usr/bin/python

"""
Copyright (c) 2014 Michael Flau <michael@flau.net>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
"""

import os
import sys
from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer

from LogFileParser import logFileParser
from HTMLGenerator import LogfileToHTMLGenerator
from HTMLGenerator import LogfileToHTMLTableGenerator

class HTTPRequestHandler(BaseHTTPRequestHandler):

    m_logfile_path = "../log/logFile.log"
    m_dbgHTML_path = "../www/debug.html"
    m_debug = False

    def do_GET(self):

        print("Debugging starts here...")

        if self.m_debug:
            try:
                fd = open(self.m_dbgHTML_path, 'r')
                self.send_response(200)
                self.send_header('Content-type', 'text-html')
                self.end_headers()
                self.wfile.write(fd.read())
                fd.close()
                return
            except IOError:
                self.send_error(404, 'file not found')
        else:
            parser = logFileParser()
            #generator = LogfileToHTMLGenerator()
            generator = LogfileToHTMLTableGenerator()

            try:
                if self.path.endswith(".css"):
                    f = open("../www/table_style_a.css")
                    self.send_response(200)
                    self.send_header('Content-type', 'text/css')
                    self.end_headers()
                    self.wfile.write(f.read())
                    f.close()
                    return

                elif self.path.endswith(".html") or self.path == "/":
                    parser.openFile(self.m_logfile_path, "r")
                    parser.setPattern("^\{.*\}$")
                    mylist = parser.parseLogFile()

                    #Create html page
                    page = """<html>
                                <head>
                                    <link rel=\"stylesheet\" type=\"text/css\" href=\"../www/table_style_a.css\">
                                </head>
                                <body>"""

                    #add content
                    page += generator.generateHTMLDivFromDoorStateObjectList(mylist, "24/02/2014", "stateList")

                    #finalize
                    page += """</body>
                            </html>"""

                    self.send_response(200)
                    self.send_header('Content-type', 'text/html')
                    self.end_headers()
                    self.wfile.write(page)
                    parser.closeFile()
                    return
                elif self.path.endswith(".js"):
                    #Handle javascript
                    return
            except IOError:
                self.send_error(404, 'file not found')

class BasicHTMLServer(object):
    m_debug = True
    m_httpd = 0

    def init(self, ip, port):
        serverAddress = (str(ip), port)
        self.m_httpd = HTTPServer(serverAddress, HTTPRequestHandler)

    def run(self):
        if self.m_debug:
            print('Starting http server in debug mode...')
        else:
            print('Starting http server...')

        self.m_httpd.serve_forever()
        print('...Server running!')


if __name__ == "__main__":

	ip = ''
	port = 0

	print(sys.argv)

	if len(sys.argv) == 3:
		ip = sys.argv[1]
		port = int(sys.argv[2])

		print(ip)
		print(port)
	else:
		print("not the required amount of arguments given!")
	
    	server = BasicHTMLServer()
    	server.init(ip, port)
    	server.run()
