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

from LogFileParser import doorStateObject

import abc

"""
Abstract base class for a HTML generator which will create
HTML content based on the input arguments.
"""
class AbstractHTMLGenerator(object):

    __metaclass__ = abc.ABCMeta

    def init(self):
        pass

    @abc.abstractmethod
    def generateHTMLFromDoorStateObject(self, item, def_id="def_id", def_class="def_class"):
        """Generates HTML code with passed String content in it"""
        return

    @abc.abstractmethod
    def generateHTMLDivFromDoorStateObjectList(self, list, def_id_part="def_id_part", def_class="def_class"):
        """Generates a HTML code div structure with passed strings in list"""
        return


"""
Concrete implementation of a HTML generator which generates HTML code
based on logfile information generated by the door daemon.
"""
class LogfileToHTMLGenerator(AbstractHTMLGenerator):

    def init(self):
        pass

    def generateDivHeader(self, def_class="def_class", def_id="def_id"):
        header = "<div "

        if not def_class == "def_class":
            header += "class=" + def_class + " "

        if not def_id == "def_id":
            header += "id=" + def_id + " "

        header += ">"

        return header


    def finalizeDiv(self, div):
        return div + "</div>"

    """
        Generates a div object from input arguments.
        def_id and def_class are optional arguments.
        They will not be regarded if members self.classTag
        or self.idTag are set respectivley.
        example:
        <div [class=doorInfo] [id=state] > </div>
    """
    def generateDivContainer(self, content_string, def_id="def_id", def_class="def_class"):

        div= self.generateDivHeader(def_class, def_id)

        div += content_string

        return self.finalizeDiv(div)


    """
        Generates <div> container for state, date and time
        of a passed door state object.
        example:
        <div [id= ] [class= ]>
        <div [class=doorInfo] [id=state] > </div>
        <div [class=doorInfo] [id=date] > </div>
        <div [class=doorInfo] [id=time] > </div>
        </div>
    """
    def generateHTMLFromDoorStateObject(self, item, def_id="def_id", def_class="def_class"):

        #Generate content
        subdiv1 = self.generateDivContainer(item.getState(), "state", "doorInfo")
        subdiv2 = self.generateDivContainer(item.getDate(), "date", "doorInfo")
        subdiv3 = self.generateDivContainer(item.getTime(), "time", "doorInfo")

        #Generate header and add content
        div = self.generateDivHeader(def_id, def_class) + subdiv1 + subdiv2 + subdiv3

        #Finalize and return
        return self.finalizeDiv(div)



    """
        Generates a full div structure tree from a logfile information list
        example:
        <div [id= ] [class=doorInfoObject]>
            <div [id=event_ ] [class=doorInfoObject]>
                <div [class=doorInfo] [id=state] > </div>
                <div [class=doorInfo] [id=date] > </div>
                <div [class=doorInfo] [id=time] > </div>
            </div>
        </div>
    """
    def generateHTMLDivFromDoorStateObjectList(self, list, def_class="def_class", def_id_part="def_id_part"):

	divclass = ''
	divid = ''

	if def_class != "def_class":
		divclass = def_class
		
		if def_id_part != "def_id_part":
			divid = def_id_part
			page_content = self.generateDivHeader(divclass, divid)
			print("Got class and id!")
		else:
			page_content = self.generateDivHeader(divclass)
			print("Got class only!")
		
	else:
		page_content = self.generateDivHeader()
		print("Got neither!")

        counter = 0

        for item in list:
            page_content += self.generateHTMLFromDoorStateObject(item, "event_" + str(counter), "doorInfoObject")
            counter += 1

        return self.finalizeDiv(page_content)
