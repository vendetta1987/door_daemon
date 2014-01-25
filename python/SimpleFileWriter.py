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