#ifndef __EECFG_H__
#define __EECFG_H__


#define RTDRUID_CONFIGURATOR_NUMBER 1273



/***************************************************************************
 *
 * Common defines ( CPU 0 )
 *
 **************************************************************************/

    /* TASK definition */
    #define EE_MAX_TASK 4
    #define task1 0
    #define task2 1
    #define task3 2
    #define task4 3

    /* MUTEX definition */
    #define EE_MAX_RESOURCE 1U
    #define RES_SCHEDULER 0U

    /* ALARM definition */
    #define EE_MAX_ALARM 4
    #define alarmTask1 0
    #define alarmTask2 1
    #define alarmTask3 2
    #define alarmTask4 3

    /* COUNTER definition */
    #define EE_MAX_COUNTER 5
    #define myCounter 0
    #define counterTask1 1
    #define counterTask2 2
    #define counterTask3 3
    #define counterTask4 4

    /* APPMODE definition */
    #define EE_MAX_APPMODE 0U

    /* CPUs */
    #define EE_MAX_CPU 1
    #define EE_CURRENTCPU 0

#ifndef __DISABLE_EEOPT_DEFINES__


/***************************************************************************
 *
 * User options
 *
 **************************************************************************/


/***************************************************************************
 *
 * Automatic options
 *
 **************************************************************************/
#define __RTD_LINUX__
#define __PIC30__
#define __33FJ256MC710__
#define __PIC33FJ256MC710__
#define __EE_FLEX__
#define __USE_LEDS__
#define __MICROCHIP_DSPIC30__
#define __PIC30_SPLIM__
#define __FP__
#define __MONO__
#define __ALARMS__

#endif

#endif

