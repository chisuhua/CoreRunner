#include "QuarkTS.h"
#include <stdio.h>
#include <string.h>

/* Event flags application definitions */
#define PROD2CONS_NOT_EMPTY   QEVENTFLAG_01    // 生产者缓冲区非空
#define CONS2PROD_NOT_FULL    QEVENTFLAG_02    // 消费者缓冲区未满
#define CONS2PROD_HAS_DATA    QEVENTFLAG_03    // 消费者缓冲区有反馈数据
#define PROD2CONS_NOT_FULL    QEVENTFLAG_04    // 生产者缓冲区未满

// 自定义消息结构体
typedef struct {
    uint32_t id;            // 消息ID
    uint8_t  type;          // 消息类型
    uint16_t length;        // 数据长度
    uint8_t  payload[32];   // 载荷数据
} CustomMessage_t;

#define QUEUE_LENGTH 8

// 队列定义
qQueue_t producerToConsumer; // Producer → Consumer
qQueue_t consumerToProducer; // Consumer → Producer

// 队列存储空间（每个元素是 CustomMessage_t）
CustomMessage_t queueBuffer_Prod2Cons[QUEUE_LENGTH];
CustomMessage_t queueBuffer_Cons2Prod[QUEUE_LENGTH];

// 任务句柄定义
qTask_t producerTask, consumerTask;

void InitializeQueues(void) {
    qQueue_Setup(&producerToConsumer, 
                 (uint8_t*)queueBuffer_Prod2Cons, 
                 sizeof(CustomMessage_t), 
                 QUEUE_LENGTH);

    qQueue_Setup(&consumerToProducer, 
                 (uint8_t*)queueBuffer_Cons2Prod, 
                 sizeof(CustomMessage_t), 
                 QUEUE_LENGTH);

    // 初始状态设置
    qTask_EventFlags_Modify(&consumerTask, PROD2CONS_NOT_EMPTY, QEVENTFLAG_CLEAR); // 清除“非空”
    qTask_EventFlags_Modify(&consumerTask, CONS2PROD_NOT_FULL, QEVENTFLAG_SET);  // 设置“未满”

    qTask_EventFlags_Modify(&producerTask, CONS2PROD_HAS_DATA, QEVENTFLAG_CLEAR);  // 清除“有数据”
    qTask_EventFlags_Modify(&producerTask, PROD2CONS_NOT_FULL, QEVENTFLAG_SET);  // 设置“未满”
}

void Producer_Callback(qEvent_t e) {
    static uint32_t msgId = 0;

    // 检查是否可以发送
    if (qTask_EventFlags_Check(&producerTask, PROD2CONS_NOT_FULL, qTrue, qFalse)) {
        printf("Producer send called\n");
        CustomMessage_t msg = {
            .id     = msgId++,
            .type   = 0x01,
            .length = 16,
        };
        memcpy(msg.payload, "Hello Consumer!", 16);

        if (qFalse == qQueue_Send(&producerToConsumer, &msg, QUEUE_SEND_TO_BACK)) {
            printf("Producer: Queue full!\n");
            qTask_EventFlags_Modify(&producerTask, PROD2CONS_NOT_FULL, QEVENTFLAG_CLEAR);
        } else {
            printf("Producer: Sent message ID %lu\n", msg.id);
        }
    }

    // 如果消费者有反馈数据，读取并处理
    if (qTask_EventFlags_Check(&producerTask, CONS2PROD_HAS_DATA, qTrue, qFalse)) {
        printf("Producer received called\n");
        CustomMessage_t feedback;
        if (qTrue == qQueue_Receive(&consumerToProducer, &feedback)) {
            printf("Producer: Received feedback for ID %lu, Type: 0x%02X\n", feedback.id, feedback.type);
            printf("Feedback Payload: %.*s\n", feedback.length, feedback.payload);

            // 如果队列未满，继续允许生产
            if (!qQueue_IsFull(&producerToConsumer)) {
                qTask_EventFlags_Modify(&producerTask, PROD2CONS_NOT_FULL, QEVENTFLAG_SET);
            }
        }
    }
}

void Consumer_Callback(qEvent_t e) {
    printf("Consumer called\n");

    CustomMessage_t receivedMsg;
    if (qTrue == qQueue_Receive(&producerToConsumer, &receivedMsg)) {
        printf("Consumer: Received message ID %lu, Type: 0x%02X\n", receivedMsg.id, receivedMsg.type);
        printf("Payload: %.*s\n", receivedMsg.length, receivedMsg.payload);

        // 构造反馈消息
        CustomMessage_t feedback = {
            .id     = receivedMsg.id,
            .type   = 0x80 | receivedMsg.type,
            .length = 13,
        };
        memcpy(feedback.payload, "Got your msg", 13);

        if (qFalse == qQueue_Send(&consumerToProducer, &feedback, QUEUE_SEND_TO_BACK)) {
            printf("Consumer: Feedback queue full!\n");
            qTask_EventFlags_Modify(&consumerTask, CONS2PROD_NOT_FULL, QEVENTFLAG_CLEAR);
        } else {
            printf("Consumer: Sent feedback for ID %lu\n", feedback.id);
            qTask_EventFlags_Modify(&producerTask, CONS2PROD_HAS_DATA, QEVENTFLAG_SET);
        }
    }
}

void AttachQueuesToTasks(void) {
    // 绑定消费者队列到生产者任务，当队列中有数据时触发
    qTask_Attach_Queue(&producerTask, &consumerToProducer, qQueueMode_Count, 1);

    // 绑定生产者队列到消费者任务，当队列中有数据时触发
    qTask_Attach_Queue(&consumerTask, &producerToConsumer, qQueueMode_Count, 1);
    //在 AttachQueuesToTasks() 中修改为：
    //qTask_Attach_Queue(&consumerTask, &producerToConsumer, qQueueMode_Empty, 1);
}

void IdleTask_Callback( qEvent_t e ) {
    printf("Idle Called\n");
    
    //qTask_EventFlags_Modify(&producerTask, PROD2CONS_NOT_FULL, QEVENTFLAG_SET);  // 设置“未满”
}

int main(void) {
    // 初始化操作系统
    qOS_Setup( NULL, IdleTask_Callback );     

    // 初始化队列和事件标志
    InitializeQueues();

    // 创建任务
    qOS_Add_EventTask(&consumerTask, Consumer_Callback, qHigh_Priority, "Consumer");
    qOS_Add_EventTask(&producerTask, Producer_Callback, qLowest_Priority, "Producer");

    // 绑定队列到任务
    AttachQueuesToTasks();

    qTask_EventFlags_Modify(&producerTask, PROD2CONS_NOT_FULL, QEVENTFLAG_SET);  // 设置“未满”
    // 启动调度器
    qOS_Run();

    while (1);
}
