#include "ee.h"





/***************************************************************************
 *
 * Kernel ( CPU 0 )
 *
 **************************************************************************/
    /* Definition of task's body */
    DeclareTask(task1);
    DeclareTask(task2);
    DeclareTask(task3);
    DeclareTask(task4);

    const EE_FADDR EE_hal_thread_body[EE_MAX_TASK] = {
        (EE_FADDR)Functask1,		/* thread task1 */
        (EE_FADDR)Functask2,		/* thread task2 */
        (EE_FADDR)Functask3,		/* thread task3 */
        (EE_FADDR)Functask4 		/* thread task4 */

    };

    /* ready priority */
    const EE_TYPEPRIO EE_th_ready_prio[EE_MAX_TASK] = {
        0x2U,		/* thread task1 */
        0x1U,		/* thread task2 */
        0x4U,		/* thread task3 */
        0x8U 		/* thread task4 */
    };

    /* dispatch priority */
    const EE_TYPEPRIO EE_th_dispatch_prio[EE_MAX_TASK] = {
        0x2U,		/* thread task1 */
        0x1U,		/* thread task2 */
        0x4U,		/* thread task3 */
        0x8U 		/* thread task4 */
    };

    /* thread status */
    #if defined(__MULTI__) || defined(__WITH_STATUS__)
        EE_TYPESTATUS EE_th_status[EE_MAX_TASK] = {
            EE_READY,
            EE_READY,
            EE_READY,
            EE_READY
        };
    #endif

    /* next thread */
    EE_TID EE_th_next[EE_MAX_TASK] = {
        EE_NIL,
        EE_NIL,
        EE_NIL,
        EE_NIL
    };

    EE_TYPEPRIO EE_th_nact[EE_MAX_TASK];
    /* The first stacked task */
    EE_TID EE_stkfirst = EE_NIL;

    /* The first task into the ready queue */
    EE_TID EE_rqfirst  = EE_NIL;

    /* system ceiling */
    EE_TYPEPRIO EE_sys_ceiling= 0x0000U;



/***************************************************************************
 *
 * Mutex
 *
 **************************************************************************/
    const EE_TYPEPRIO EE_resource_ceiling[EE_MAX_RESOURCE]= {
        0x8U 		/* resource RES_SCHEDULER */
    };

    EE_TYPEPRIO EE_resource_oldceiling[EE_MAX_RESOURCE];



/***************************************************************************
 *
 * Counters
 *
 **************************************************************************/
    EE_counter_RAM_type       EE_counter_RAM[EE_MAX_COUNTER] = {
        {0, -1},        /* myCounter */
        {0, -1},        /* counterTask1 */
        {0, -1},        /* counterTask2 */
        {0, -1},        /* counterTask3 */
        {0, -1}         /* counterTask4 */
    };



/***************************************************************************
 *
 * Alarms
 *
 **************************************************************************/
    const EE_alarm_ROM_type   EE_alarm_ROM[] = {
        {1, EE_ALARM_ACTION_TASK    , task1, NULL},
        {2, EE_ALARM_ACTION_TASK    , task2, NULL},
        {3, EE_ALARM_ACTION_TASK    , task3, NULL},
        {4, EE_ALARM_ACTION_TASK    , task4, NULL}
    };

    EE_alarm_RAM_type         EE_alarm_RAM[EE_MAX_ALARM];

