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

Class: abstractFileWriter

A base class for file writer. It provides all derived
classes with basic write functionality.
"""

import os
import fcntl

class abstractFileWriter(object):

    m_FileHandle  = 0     #File handle to open file
    m_bHandleOpen = False #Denotes if handle is open
    m_bDebugMode  = True

    def __init__(self):
        pass

    def openFile(self, name):

        accessMode = ""

        if self.handleState():
            self.dPrint("Handle is already open")
            return

        if os.path.exists(name):
            accessMode = 'a'
        else:
            accessMode = 'w'

        try:
            self.m_FileHandle = open(name, accessMode)
            self.m_bHandleOpen = True
        except IOError:
            self.m_bHandleOpen = False

    def closeFile(self):
        if not self.handleState():
            self.dPrint("Handle is not open!")
            return

        self.m_FileHandle.close()
        self.m_bHandleOpen = False
        self.dPrint("File closed!")

    def handleState(self):
        return self.m_bHandleOpen

    def dPrint(self, msg):
        if self.m_bDebugMode:
            print msg

    def writeNextLine(self, line):
        if self.handleState():
            try:
                self.m_FileHandle.write(str(line))
                self.m_FileHandle.flush()
            except:
                raise

    def writeNextLineSafe(self, line):
        if self.handleState():
            try:
                fcntl.flock(self.m_FileHandle, fcntl.LOCK_EX)
                self.m_FileHandle.write(str(line))
                self.m_FileHandle.flush()
                fcntl.flock(self.m_FileHandle, fcntl.LOCK_UN)
            except:
                raise


"""
    def writeContent(self, content):
        if self.handleState():
            stillValid = True
            while stillValid:
                line = self.getNextLine()
                if line != '':
                    self.dPrint(line)
                else:
                    stillValid = False
                    self.closeFile()
            else:
                self.dPrint("The End")
        else:
            self.dPrint("Filehandle is not open!")
"""
