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
from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer

import LogFileParser

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
            parser = LogFileParser.logFileParser()

            try:
                parser.openFile(self.m_logfile_path, "r")
                parser.setPattern("^\{.*\}$")
                list = parser.parseLogFile()

                page = "<html>\n<head>\n</head>\n<body>\n"

                print "Number of items: " + str(len(list))

                for item in list:
                    page += "<div>" + item.getState() + " - " + item.getDate() + " - " + item.getTime() + "</div>\n"

                page += "</body>\n</html>\n"

                self.send_response(200)
                self.send_header('Content-type', 'text-html')
                self.end_headers()
                self.wfile.write(page)
                parser.closeFile()
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
    server = BasicHTMLServer()
    server.init('10.116.1.40', 8000)
    server.run()
