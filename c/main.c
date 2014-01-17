#include "pin_polling.h"

int main(int argc, char** argv){


	initIO();
	setPinReadMode(0);

	return getCircuitState() ? EXIT_SUCCESS : EXIT_FAILURE;
}