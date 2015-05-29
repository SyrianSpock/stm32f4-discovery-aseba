#include <string.h>

// ChibiOS includes
#include "ch.h"
#include "hal.h"

// Aseba includes
#include "vm/natives.h"
#include "vm/vm.h"
#include "common/consts.h"
#include "transport/buffer/vm-buffer.h"

#include "skel.h"
#include "skel_user.c"

unsigned int events_flags = 0;


/*
 * VM
 */
struct _vmVariables vmVariables;
static uint16 vmBytecode[VM_BYTECODE_SIZE];
static sint16 vmStack[VM_STACK_SIZE];

AsebaVMState vmState = {
    0,

    VM_BYTECODE_SIZE,
    vmBytecode,

    sizeof(vmVariables) / sizeof(sint16),
    (sint16*)&vmVariables,

    VM_STACK_SIZE,
    vmStack
};


/*
 * Callbacks
 */
void AsebaIdle(void)
{
    chThdSleepMilliseconds(1);
}

void AsebaPutVmToSleep(AsebaVMState *vm)
{
    chThdSleepMilliseconds(1000);
}

void AsebaResetIntoBootloader(AsebaVMState *vm)
{
    NVIC_SystemReset();
}

void AsebaNativeFunction(AsebaVMState *vm, uint16 id)
{
    nativeFunctions[id](vm);
}

const AsebaNativeFunctionDescription * const * AsebaGetNativeFunctionsDescriptions(AsebaVMState *vm)
{
    return nativeFunctionsDescription;
}


const AsebaVMDescription* AsebaGetVMDescription(AsebaVMState *vm)
{
    return &vmDescription;
}

const AsebaLocalEventDescription * AsebaGetLocalEventsDescriptions(AsebaVMState *vm)
{
    return localEvents;
}

uint16 AsebaShouldDropPacket(uint16 source, const uint8* data)
{
    return AsebaVMShouldDropPacket(&vmState, source, data);
}

void AsebaWriteBytecode(AsebaVMState *vm)
{

}
