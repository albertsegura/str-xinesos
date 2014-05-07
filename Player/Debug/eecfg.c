#include "ee.h"





/***************************************************************************
 *
 * Kernel ( CPU 0 )
 *
 **************************************************************************/
    /* Definition of task's body */
    DeclareTask(SenderTask);
    DeclareTask(ReceiverTask);

    const EE_FADDR EE_hal_thread_body[EE_MAX_TASK] = {
        (EE_FADDR)FuncSenderTask,		/* thread SenderTask */
        (EE_FADDR)FuncReceiverTask 		/* thread ReceiverTask */

    };

    /* ready priority */
    const EE_TYPEPRIO EE_th_ready_prio[EE_MAX_TASK] = {
        0x1U,		/* thread SenderTask */
        0x1U 		/* thread ReceiverTask */
    };

    /* dispatch priority */
    const EE_TYPEPRIO EE_th_dispatch_prio[EE_MAX_TASK] = {
        0x1U,		/* thread SenderTask */
        0x1U 		/* thread ReceiverTask */
    };

    /* thread status */
    #if defined(__MULTI__) || defined(__WITH_STATUS__)
        EE_TYPESTATUS EE_th_status[EE_MAX_TASK] = {
            EE_READY,
            EE_READY
        };
    #endif

    /* next thread */
    EE_TID EE_th_next[EE_MAX_TASK] = {
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

    /* deadlines */
    const EE_TYPERELDLINE EE_th_reldline[EE_MAX_TASK] = {
        2000000,		/* thread SenderTask */
        2000000 		/* thread ReceiverTask */
    };
    EE_TYPEABSDLINE EE_th_absdline[EE_MAX_TASK];



/***************************************************************************
 *
 * Counters
 *
 **************************************************************************/
    EE_counter_RAM_type       EE_counter_RAM[EE_MAX_COUNTER] = {
        {0, -1}         /* myCounter */
    };



/***************************************************************************
 *
 * Alarms
 *
 **************************************************************************/
    const EE_alarm_ROM_type   EE_alarm_ROM[] = {
        {0, EE_ALARM_ACTION_TASK    , SenderTask, NULL},
        {0, EE_ALARM_ACTION_TASK    , ReceiverTask, NULL}
    };

    EE_alarm_RAM_type         EE_alarm_RAM[EE_MAX_ALARM];

