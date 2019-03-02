#ifndef ITEA_HANDLER
#define ITEA_HANDLER

#include <SPI.h>

#define ITEA_STATE_ERROR 0 
#define ITEA_STATE_INIT 1
#define ITEA_STATE_RUN 2

// handler function 
typedef uint8_t (*StateHandler)(uint8_t, void * ...);

// main class
class iTeaHandlerClass {
	public:
		iTeaHandlerClass();
		
		// call this in loop() and handle the return value
		uint8_t loop(void *...);
		uint8_t setup(uint8_t = ITEA_STATE_INIT);
		void add(uint8_t, StateHandler);
		uint8_t call(uint8_t, void *...);
		void setState(uint8_t);
	private:
		uint8_t _state = ITEA_STATE_INIT;
		StateHandler _states[UINT8_MAX] = { NULL };
};

static iTeaHandlerClass iTeaHandler;
uint8_t notfoundHandler(uint8_t state, void *params ...); 
#endif // ITEA_HANDLER
