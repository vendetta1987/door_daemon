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

import AbstractFileWriter

class simpleFileWriter(AbstractFileWriter.abstractFileWriter):

    def init(self):
        pass

if __name__ == "__main__":

    writer = simpleFileWriter()

    writer.openFile("test3.cfg")

    writer.writeNextLineSafe("Das ist ein test\n")
    writer.writeNextLineSafe("Ohne Umbruch\ngeht es nicht\n")
    writer.writeNextLineSafe("Das ist ein noch ein test test\n")

    writer.closeFile()
