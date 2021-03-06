#include "ch.h"
#include "hal.h"

#include "vm/natives.h"
#include "discovery_demo/discovery_natives.h"

/*
 * Descriptors
 */
const AsebaVMDescription vmDescription = {
	"Epuck2",
	{
		// {Number of element in array, Name displayed in aseba studio}
		{1, "_id"},
		{1, "event.source"},
		{VM_VARIABLES_ARG_SIZE, "event.args"},
		{2, "_fwversion"},
		{1, "_productId"},

        {6, "leds"},
        {3, "acc"},

		{0, NULL}
	}
};

// Event descriptions
static const AsebaLocalEventDescription localEvents[] = {
    {"new_acc", "New accelerometer measurement"},
	{NULL, NULL}
};

// Native functions
static AsebaNativeFunctionDescription AsebaNativeDescription__system_reboot =
{
    "_system.reboot",
    "Reboot the microcontroller",
    {
        {0,0}
    }
};

void AsebaNative__system_reboot(AsebaVMState *vm)
{
    NVIC_SystemReset();
}

// Native function descriptions
static const AsebaNativeFunctionDescription* nativeFunctionsDescription[] = {
	&AsebaNativeDescription__system_reboot,
    ASEBA_NATIVES_STD_DESCRIPTIONS,
    DISCOVERY_NATIVES_DESCRIPTIONS,
    0
};

// Native function pointers
static AsebaNativeFunctionPointer nativeFunctions[] = {
    AsebaNative__system_reboot,
	ASEBA_NATIVES_STD_FUNCTIONS,
    DISCOVERY_NATIVES_FUNCTIONS
};
