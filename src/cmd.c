#include <math.h>
#include <string.h>
#include "hal.h"
#include "test.h"
#include "chprintf.h"
#include "shell.h"
#include "usbcfg.h"
#include "chtm.h"
#include "common/types.h"
#include "vm/natives.h"

#define TEST_WA_SIZE        THD_WORKING_AREA_SIZE(256)

static void cmd_mem(BaseSequentialStream *chp, int argc, char *argv[])
{
    size_t n, size;

    (void)argv;
    if (argc > 0) {
        chprintf(chp, "Usage: mem\r\n");
        return;
    }
    n = chHeapStatus(NULL, &size);
    chprintf(chp, "core free memory : %u bytes\r\n", chCoreGetStatusX());
    chprintf(chp, "heap fragments     : %u\r\n", n);
    chprintf(chp, "heap free total    : %u bytes\r\n", size);
}

static void cmd_threads(BaseSequentialStream *chp, int argc, char *argv[])
{
    static const char *states[] = {CH_STATE_NAMES};
    thread_t *tp;

    (void)argv;
    if (argc > 0) {
        chprintf(chp, "Usage: threads\r\n");
        return;
    }
    chprintf(chp, "        addr        stack prio refs         state\r\n");
    tp = chRegFirstThread();
    do {
        chprintf(chp, "%08lx %08lx %4lu %4lu %9s\r\n",
                         (uint32_t)tp, (uint32_t)tp->p_ctx.r13,
                         (uint32_t)tp->p_prio, (uint32_t)(tp->p_refs - 1),
                         states[tp->p_state]);
        tp = chRegNextThread(tp);
    } while (tp != NULL);
}

static void cmd_test(BaseSequentialStream *chp, int argc, char *argv[])
{
    thread_t *tp;

    (void)argv;
    if (argc > 0) {
        chprintf(chp, "Usage: test\r\n");
        return;
    }
    tp = chThdCreateFromHeap(NULL, TEST_WA_SIZE, chThdGetPriorityX(),
                                                     TestThread, chp);
    if (tp == NULL) {
        chprintf(chp, "out of memory\r\n");
        return;
    }
    chThdWait(tp);
}

static void cmd_readclock(BaseSequentialStream *chp, int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    chprintf(chp, "SYSCLK: %i \n HCLK: %i \n PCLK1  %i \n PCLK2 %i \n",
        STM32_SYSCLK, STM32_HCLK, STM32_PCLK1, STM32_PCLK2);
}


extern sint16 aseba_sqrt(sint16 num);

static void cmd_sqrt(BaseSequentialStream *chp, int argc, char *argv[])
{
    uint16_t input, result;
    float x;
    time_measurement_t tmp;
    chTMObjectInit(&tmp);

    if(argc != 2) {
        chprintf(chp, "Usage: sqrt mode int\r\nModes: a (aseba), b (math), c (assembler) is default mode\r\n");
    } else {
        input =(uint16_t) atoi(argv[1]);

        if(!strcmp(argv[0], "a")) {
            chSysLock();
            chTMStartMeasurementX(&tmp);
            result = aseba_sqrt(input);
            chTMStopMeasurementX(&tmp);
            chSysUnlock();
        } else if(!strcmp(argv[0], "b")) {
            chSysLock();
            chTMStartMeasurementX(&tmp);
            result = sqrtf(input);
            chTMStopMeasurementX(&tmp);
            chSysUnlock();
        } else {
            chSysLock();
            chTMStartMeasurementX(&tmp);
            x = (float) input;
            __asm__ volatile(
                "vsqrt.f32 %[var], %[var]"
                : [var]"+t"(x)
            );
            result =(uint16_t) x;
            chTMStopMeasurementX(&tmp);
            chSysUnlock();
        }

        chprintf(chp, "sqrt(%u) = %u \r\n", input, result);
        chprintf(chp, "time: %u \r\n", tmp.last);
    }
}

extern sint16 aseba_atan2(sint16 y, sint16 x);

static void cmd_atan2(BaseSequentialStream *chp, int argc, char *argv[])
{
    int16_t a, b, result;
    time_measurement_t tmp;
    chTMObjectInit(&tmp);

    if(argc != 3) {
        chprintf(chp, "Usage: atan2 mode a b\r\nModes: a (aseba), b (math) is default mode\r\n");
    } else {
        a =(int16_t) atoi(argv[1]);
        b =(int16_t) atoi(argv[2]);

        if(!strcmp(argv[0], "a")) {
            chSysLock();
            chTMStartMeasurementX(&tmp);
            result = aseba_atan2(a, b);
            chTMStopMeasurementX(&tmp);
            chSysUnlock();
        } else {
            chSysLock();
            chTMStartMeasurementX(&tmp);
            result =(int16_t)(atan2f(a, b) * 32768 / M_PI);
            chTMStopMeasurementX(&tmp);
            chSysUnlock();
        }


        chprintf(chp, "atan2(%d, %d) = %d \r\n", a, b, result);
        chprintf(chp, "time: %u \r\n", tmp.last);
    }
}

const ShellCommand shell_commands[] = {
    {"mem", cmd_mem},
    {"threads", cmd_threads},
    {"test", cmd_test},
    {"clock", cmd_readclock},
    {"sqrt", cmd_sqrt},
    {"atan2", cmd_atan2},
    {NULL, NULL}
};
