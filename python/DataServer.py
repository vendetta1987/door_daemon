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

import socket
import sys
import os

import SimpleFileWriter

class dataServer(object):
    m_SocketPath = 0
    m_LoggingPath = 0
    m_Socket = 0
    m_pathSocketSet = False
    m_FileWriter = SimpleFileWriter.simpleFileWriter()

    def __init__(self):
        pass

    def setSocketPath(self, path):
        self.m_SocketPath = path
        self.m_pathSocketSet = True

    def setLoggingPath(self, path):
        self.m_LoggingPath = path

    def openServerSocket(self):
        if self.m_pathSocketSet:
            try:
                os.unlink(self.m_SocketPath)
            except OSError:
                if os.path.exists(self.m_SocketPath):
                    raise

            self.m_FileWriter.openFile(self.m_LoggingPath)

            self.m_Socket = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)

            self.m_Socket.bind(self.m_SocketPath)

            self.m_Socket.listen(1)

            while True:
                print >>sys.stderr, 'waiting for a connection'
                connection, client_address = self.m_Socket.accept()
                try:
                    print >>sys.stderr, 'connection from', client_address

                    while True:
                        data = connection.recv(60)
                        print >>sys.stderr, 'received "%s"' % data
                        self.m_FileWriter.writeNextLineSafe(str(data) + "\n")
                        if not data:
                            self.m_FileWriter.closeFile()
                            print >>sys.stderr, 'no more data from', client_address
                            break
                finally:
                    connection.close()
