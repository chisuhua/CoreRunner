#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "QuarkTS.h"
#define TIMER_TICK   ( 0.001f )   /* 1ms */ 

/*event flags application definitions */
#define SWITCH_CHANGED  QEVENTFLAG_01
#define TIMER_EXPIRED   QEVENTFLAG_02
#define DATA_READY      QEVENTFLAG_03
#define DATA_TXMIT      QEVENTFLAG_04

qTask_t TaskDataProducer; 
qUINT8_t dataToTransmit[ 10 ] = { 0u };

static inline uint64_t read_instret(void) {
    uint32_t lo, hi;
    asm volatile ("csrr %0, instret" : "=r"(lo));  // 读取低32位
    asm volatile ("csrr %0, instreth" : "=r"(hi));  // 读取高32位
    return ((uint64_t)hi << 32) | lo;
}


/*-----------------------------------------------------------------------*/
void TaskDataProducer_Callback( qEvent_t e ) { 
    printf("call task producer data\n");
    qBool_t condition;
    condition = qTask_EventFlags_Check( &TaskDataProducer, 
                                        DATA_TXMIT, qTrue, qTrue );

    printf("TaskData %s", (char*)(e->TaskData));
    if ( qTrue == condition) {
        //GenerateData( dataToTransmit );
        printf("call data generateData\n");
        qTask_EventFlags_Modify( &TaskDataProducer, DATA_READY, 
                                 QEVENTFLAG_SET ); 
    }
    qTask_EventFlags_Check( &TaskDataProducer, 
                            DATA_READY | SWITCH_CHANGED | TIMER_EXPIRED,
                            qTrue, qTrue );
}
/*-----------------------------------------------------------------------*/
void IdleTask_Callback( qEvent_t e ) {
    
    //TransmitData( dataToTransmit );
    printf("call transimite data\n");
    qTask_EventFlags_Modify( &TaskDataProducer, 
                             DATA_TXMIT, QEVENTFLAG_SET ); 
}
/*-----------------------------------------------------------------------*/
int main( void ) {    
    printf("start\n");
    /* next line is used to setup hardware with specific code to fire
     * interrupts at 1ms - timer tick*/
    /*
    Idle task will be responsible to transmit the generate the data after 
    all conditions are meet
    */
    qOS_Setup( NULL, IdleTask_Callback );
    /*
    The task will wait until data is transmitted to generate another set of
    data
    */
    qOS_Add_EventTask( &TaskDataProducer, TaskDataProducer_Callback,
                       qHigh_Priority, "DATAPRODUCER" );
    /*
    Set the flag DATA_TXMIT as initial condition to allow the data 
    generation at startup
    */
    //qTask_EventFlags_Modify( &TaskDataProducer, DATA_TXMIT, QEVENTFLAG_SET ); 
    qOS_Run();
    for(;;){}
    return 0;
}
