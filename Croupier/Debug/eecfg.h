#ifndef __EECFG_H__
#define __EECFG_H__


#define RTDRUID_CONFIGURATOR_NUMBER 1273



/***************************************************************************
 *
 * Common defines ( CPU 0 )
 *
 **************************************************************************/

    /* TASK definition */
    #define EE_MAX_TASK 1
    #define RxTxTask 0

    /* MUTEX definition */
    #define EE_MAX_RESOURCE 0U

    /* ALARM definition */
    #define EE_MAX_ALARM 1
    #define AlarmReceiver 0

    /* COUNTER definition */
    #define EE_MAX_COUNTER 1
    #define myCounter 0

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
#define __EDF__
#define __ALARMS__
#define __MONO__
#define __ALLOW_NESTED_IRQ__

#endif

#endif

