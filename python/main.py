#!/usr/bin/python

import DataServer

if __name__ == "__main__":
    server = DataServer.dataServer()

    server.setSocketPath('/tmp/door_daemon_data_socket')
    server.setLoggingPath('./logFile.log')

    server.openServerSocket()
