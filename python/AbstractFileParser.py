"""
Copyright (c) 201 Michael Flau <michael@flau.net>

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

Class: abstractFileParser

A base class for file parsers. It provides all derived
classes with basic read functionality.
"""

class abstractFileParser(object):

    m_FileHandle  = 0     #File handle to open file
    m_bHandleOpen = False #Denotes if handle is open
    m_bDebugMode  = True

    def __init__(self):
        pass

    def openFile(self, name, accessMod):
        if self.handleState():
            self.dPrint("Handle is already open")
            return

        try:
            self.m_FileHandle = open(name, accessMod)
            self.m_bHandleOpen = True
        except IOError:
            self.m_bHandleOpen = False

    def closeFile(self):
        if self.handleState():
            self.dPrint("Handle is not open!")
            return

        self.m_FileHandle.close()
        self.m_bHandleOpen = False

    def handleState(self):
        return self.m_bHandleOpen

    def dPrint(self, msg):
        if self.m_bDebugMode:
            print msg

    def getNextLine(self):
        if self.handleState():
            line = self.m_FileHandle.readline()
            return line

    def printContent(self):
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
