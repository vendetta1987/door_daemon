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

Class: abstractFileParser

A base class for file parsers. It provides all derived
classes with basic read functionality.
"""

import AbstractFileParser
import json
from re import match

"""
Basic data container for retrieved
information.
"""
class doorStateObject(object):

    m_State = ''
    m_Date  = ''
    m_Time  = ''

    def __init__(self):
        pass

    def setValuesFromDict(self, obj):
        self.m_State = obj['State']
        self.m_Date  = obj['Year']
        self.m_Time  = obj['Time']

    def setValuesFromCopy(self, obj):
        self.m_State = obj.m_State
        self.m_Date  = obj.m_Date
        self.m_Time  = obj.m_Time

    def setState(self, state):
        self.m_State = state
    def setDate(self, date):
        self.m_Date = date
    def setTime(self, time):
        self.m_Time = time

    def getState(self):
        return self.m_State
    def getDate(self):
        return self.m_Date
    def getTime(self):
        return self.m_Time

    def printValues(self):
        print "State: " + self.m_State
        print "Date: "  + self.m_Date
        print "Time: "  + self.m_Time



class logFileParser(AbstractFileParser.abstractFileParser):

    m_Pattern = ''

    def __init__(self):
        pass

    def parseJSONToObject(self):
        if self.handleState():
            raw = self.getNextLine()
            if self.isMatch(raw):
                #print "string is: " + raw
                return json.loads(raw)
            else:
                print "no match!"
                return ""


    def parseLogFile(self):
        line = 'x'
        list = []

        while line != "":
            line = self.parseJSONToObject()

            if line == "":
                continue
            else:
                tmp = doorStateObject()
                tmp.setValuesFromDict(line)
                list.append(tmp)

        return list

    def setPattern(self, pattern):
        self.m_Pattern = pattern

    def isMatch(self, value):
        if match(self.m_Pattern, value):
            return True
        else:
            return False


#debug

if __name__ == "__main__":

    parser = logFileParser()

    parser.openFile("logFile.log", "r")

    parser.setPattern("^\{.*\}$")

    list = parser.parseLogFile()

    numberOfItems = len(list)

    if numberOfItems > 0:

        for item in list:
            item.printValues()
            print "\n"
    else:
        print "No items in list"
