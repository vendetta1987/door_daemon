#!/usr/bin/python

import socket
import sys
import os

class data_server(object):
    m_SocketPath = 0
    m_Socket = 0
    m_pathSet = False

    def __init__(self):
        pass

    def setSocketPath(self, path):
        self.m_SocketPath = path
        self.m_pathSet = True

    def openServerSocket(self):
        if self.m_pathSet:
            try:
                os.unlink(self.m_SocketPath)
            except OSError:
                if os.path.exists(self.m_SocketPath):
                    raise

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
                        if not data:
                            print >>sys.stderr, 'no more data from', client_address
                            break
                finally:
                    connection.close()

if __name__ == "__main__":
    server = data_server()

    server.setSocketPath('/tmp/door_daemon_data_socket')

    server.openServerSocket()
