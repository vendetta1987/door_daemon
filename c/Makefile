# Makefile for the C component of the doord daemon
# As this programm can be deployed on normal desktop 
# Linux systems and the Raspberry Pi, the compilation
# underlies some conditions, which can be controlled
# with command line flags when calling 'make'. The
# The following flags are valid:
#
# PLATFORM 	- No value when on normal desktop platforms, 'rpi' for the Raspberry Pi
# MODE 		- No value if not, 'debug' if yes. This option should be always enabled for now!
# RUNAS 	- No value for normal usage, 'daemon' to have the programm automatically start
#	  	  as background process.
# POLL		- No value will read the pin state via callback, 'active' will 
#		  enable active polling of pin state (high CPU usage!)

CC = cc
OBJECTS = main.o pin_polling.o data_socket.o
CFLAGS = -g -Wall -std=gnu99
BINARY = doord

#Set computer platform
ifeq ($(PLATFORM),rpi)
	LDFLAGS = -pthread -lwiringPi -o
	RPI = -DRPI
else
	LDFLAGS = -pthread -o
	RPI =
endif

#Enable debug modes
ifeq ($(MODE),debug)
	DEBUG = -DDEBUG
else
	DEBUG =
endif

#Start program as daemon
ifeq ($(RUNAS),daemon)
	DAEMON = -DDAEMON
else
	DAEMON =
endif

#Enable active polling (CPU usage high)
ifeq ($(POLL), active)
	READ_MODE = -DACTIVE_POLL
else
	READ_MODE = 
endif

DEFINES = $(DEBUG) $(DAEMON) $(RPI) $(READ_MODE)

.PHONY = clean distclean rebuild

all: build

build: $(OBJECTS)
	$(CC) $+ $(LDFLAGS) $(BINARY)

pin_polling.o: pin_polling.c pin_polling.h data_socket.h app_utils.h app_includes.h
	$(CC) $(DEFINES) $(CFLAGS) -c pin_polling.c -o pin_polling.o

data_socket.o: data_socket.c data_socket.h app_utils.h app_includes.h
	$(CC) $(DEFINES) $(CFLAGS) -c data_socket.c -o data_socket.o

main.o: main.c app_types.h pin_polling.h
	$(CC) $(DEFINES) $(CFLAGS) -c main.c -o main.o

clean:
	rm -rf $(OBJECTS)

distclean:
	rm -rf $(OBJECTS) $(BINARY)

rebuild: distclean all
