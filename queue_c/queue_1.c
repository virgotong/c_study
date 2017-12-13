#include <stdio.h>
#include <memory.h>

#include <stdlib.h>
#include <time.h>

#define MAX_SIZE 1024                       // 缓冲区最大容纳数据总和

struct arrQueue                             // 声明队列的头和尾节点，数据存储元素
{
    unsigned char       data[MAX_SIZE+1];   // 定义数组队列元素，能容纳MAX_SIZE字节
    unsigned int        front;              // 含有数据的第一个下标
    unsigned int        rear;               // 含有数据的最后一个元素下标，rear所在的元素不存数据
}queue;

void initQueue()
{
    memset(queue.data, '\0', sizeof(queue.data));
    queue.front = 0;
    queue.rear  = 0;
}

void enQueue(unsigned char *pElement, unsigned int *len)//向queue中存入长度为len字节数据，len返回长度实际存入的长度
{
    // 如果rear在front前面一位，则表明缓冲区已经满，无法存入数据
    // 当rear在最后一位，front在最前一位，也是没有空间
    if ((queue.rear+1)%(MAX_SIZE+1) == queue.front)
    {
        *len = 0;
        return;
    }

    // 如果rear在front前面一位以上，则rear与front之间就是空闲区域，长度为front-rear-1
    if (queue.rear+1<queue.front)
    {
        // 存入数据位len与剩余空间较小者
        *len = *len <= (queue.front-queue.rear-1) ? *len : queue.front-queue.rear-1;
        
        memcpy(&queue.data[queue.rear], pElement, *len);
        queue.rear += *len;
    }
    else    // 如果 rear在front后面，或者相等时，并且有空间
    {
        // 剩余空间大小为 MAX_SIZE-(rear-front), 可写入的数据位长度与剩余空间较小者
        *len = *len <= (MAX_SIZE-(queue.rear-queue.front)) ? *len : MAX_SIZE-(queue.rear-queue.front);
        // 如果front为0，则rear就在小于等于MAX_SIZE的位置，只需要队列尾部保存数据
        if (queue.front==0)
        {
            memcpy(&queue.data[queue.rear], pElement, *len);
            queue.rear += *len;
        }
        else // 如果front不为0，则 队列尾保存了数据，队列头部有可能需要保存数据
        {
            // 如果尾部可以保存数据，则直接保存数据
            if ((MAX_SIZE+1-queue.rear) >= *len)
            {
                memcpy(&queue.data[queue.rear], pElement, *len);
                queue.rear = (queue.rear + *len)%(MAX_SIZE+1);
            }
            else // 如果尾部保存不了数据，头部还需要保存数据，尾部保存的数据长度为：MAX_SIZE+1-queue.rear
            {
                memcpy(&queue.data[queue.rear], pElement, MAX_SIZE+1-queue.rear);
                queue.rear = *len-(MAX_SIZE+1-queue.rear);
                memcpy(&queue.data[0], pElement+(*len-queue.rear), queue.rear);
            }
        }
    }
}

void deQueue(unsigned char *pElement, unsigned int *len)//向queue中取出长度为len字节数据，len返回长度实际取出的长度
{
    // 如果为空数据
    if ((*len==0) || (queue.rear==queue.front))
    {
        *len = 0;
        return;
    }

    // 如果 rear在front后面，则直接取出数据
    if (queue.rear > queue.front)
    {
        *len = *len <= (queue.rear-queue.front) ? *len : (queue.rear-queue.front);
        memcpy(pElement, &queue.data[queue.front], *len);
        queue.front += *len;
    }
    else    // 如果rear在front前面
    {
        // 队列中数据长度为 rear+(MAX_SIZE+1-front)
        *len = *len <= queue.rear+(MAX_SIZE+1-queue.front) ? *len : queue.rear+(MAX_SIZE+1-queue.front);
        
        // 如果尾部可以取出*len的数据则直接取，尾部数据大小为MAX_SIZE+1-queue.front
        if (*len <= MAX_SIZE+1-queue.front)
        {
            memcpy(pElement, &queue.data[queue.front], *len);
            queue.front = (queue.front + *len) % (MAX_SIZE+1);
        }
        else    // 如果尾部取完，还需要取数据，则取头部数据
        {
            memcpy(pElement, &queue.data[queue.front], (MAX_SIZE+1-queue.front));
            queue.front = *len-(MAX_SIZE+1-queue.front);
            memcpy(&pElement[*len-queue.front], &queue.data[0], queue.front);
        }
    }
}

unsigned int lenQueue()     // 队列存了多少数据
{
    return (queue.rear+MAX_SIZE+1-queue.front)%(MAX_SIZE+1);
}


/////////////////////////////////长度数组队列///////////////////////////////////
#define MAX_L_SIZE      128   // 最多存入数据份数，一次存入数据位一份数据
struct LQueue
{
    unsigned int data[MAX_L_SIZE];
    unsigned int front;
    unsigned int rear;
}lqueue;

void initLQueue()
{
    memset(lqueue.data, '\0', sizeof(lqueue.data));
    lqueue.front = 0;
    lqueue.rear = 0;
}

void enLQueue(unsigned int *len)    // 插入长度len, len不能为0，插入正确len不改变，插入错误len为0
{
    if ((lqueue.rear+1)%MAX_L_SIZE == lqueue.front)     // 队列已满
    {
        *len = 0;
    }
    else
    {
        lqueue.data[lqueue.rear] = *len;
        lqueue.rear = (lqueue.rear+1)%MAX_L_SIZE;
    }

}

void deLQueue(unsigned int *len)    // 返回一个长度len数值，len为0则队列为空
{
    if (lqueue.rear == lqueue.front)    // 队列为空
    {
        *len = 0;
    }
    else
    {
        *len = lqueue.data[lqueue.front];
        lqueue.data[lqueue.front] = 0;  // 还原数据为0
        lqueue.front = (lqueue.front+1)%MAX_L_SIZE;
    }
}

unsigned int lenLQueue()            // 获取队列存入多少len数值
{
    return (lqueue.rear+MAX_L_SIZE-lqueue.front)%MAX_L_SIZE;
}
/////////////////////////////////长度队列///////////////////////////////////



// 缓冲区对外两个接口函数
//////////////////////////////////////////////////////////////////////
// 写入数据到缓冲区，若缓冲区存不下需要放入的数据，则删除前面存入的数据
unsigned char dtu_buffer_add_data(const unsigned char *pbuf, unsigned int len)
{
    unsigned int len_t = 0;
    unsigned char pbuf_t[128] = {0x0, };
    if ((pbuf==NULL) || (len==0))
    {
        return 0;
    }
    
    while (MAX_SIZE-lenQueue() < len)   // 如果缓冲区满了，则删除以前的的数据
    {
        deLQueue(&len_t);
        deQueue((unsigned char*)pbuf_t, &len_t);
        printf("【%d-%s】\r\n", len_t, pbuf_t);
    }
    if (MAX_L_SIZE==lenLQueue()+1)      // 如果长度队列满了，则删除一个长度队列
    {
        deLQueue(&len_t);
        deQueue((unsigned char*)pbuf_t, &len_t);
        printf("【%d-%s】\r\n", len_t, pbuf_t);
    }

    enLQueue(&len);
    enQueue((unsigned char*)pbuf, &len);

    return 1;
}

// 从缓冲区获取内容
unsigned char dtu_buffer_get_data(unsigned char* pbuf, unsigned int* len)
{
    unsigned char result = 0;
    if (pbuf==NULL)
        return result;

    deLQueue((unsigned int*)len);
    deQueue((unsigned char*)pbuf, (unsigned int*)len);
    if (*len)
        result = 1;

    return result;
}

/////////////////////////////////////////////////////////////////////////

void main()
{
    int i = 0;
    int len = 1;
    char buf[32];
    initQueue();

    // 测试1，正常情况
    queue.front = 0;
    queue.rear  = 0;
    // 测试2，front在前，rear在后
    //queue.front   = 5;
    //queue.rear    = 7;
    // 测试3，front在后，rear在前
    //queue.front   = 26;
    //queue.rear    = 8;

    // 测试四个存入////////////////////////

    printf("begin==========\r\n");
    for (i=0, len=1; len; i++)
    {
        len = 4;
        buf[0] = 'A'+4*i;
        buf[1] = 'A'+4*i+1;
        buf[2] = 'A'+4*i+2;
        buf[3] = 'A'+4*i+3;
        printf("%d-", lenQueue());
        enQueue(buf, &len);
        printf("%d ", len);
    }
    printf("\r\n");
    for (i=0; i<MAX_SIZE+1; i++)
    {
        printf("%c ", queue.data[i]);
    }
    printf("\r\n");
    // 测试四个存入 end////////////////////

    // 测试四个取出////////////////////////
    for (i=0, len=1; len; i++)
    {
        len = 4;
        printf("%d-", lenQueue());
        deQueue(buf, &len);
        buf[len] = '\0';
        printf("%d-%s\r\n", len, buf);
    }
    printf("end============\r\n");
    // 测试四个取出 end////////////////////
}

    // 采用随机数测试//////////////////////
// {
//     int value_f;
//     int value_r;
//     srand(time(0));
//     value_f = rand() % (MAX_SIZE+1);
//     value_r = rand() % (MAX_SIZE+1);
//     initQueue();
//     queue.front = value_f;
//     queue.rear  = value_r;
//     printf("\r\nbegin========== %d %d\r\n", value_f, value_r);

//     for (i=0, len=1; len; i++)
//     {
//         len = 4;
//         buf[0] = 'A'+4*i;
//         buf[1] = 'A'+4*i+1;
//         buf[2] = 'A'+4*i+2;
//         buf[3] = 'A'+4*i+3;
//         printf("%d-", lenQueue());
//         enQueue(buf, &len);
//         printf("%d ", len);
//     }
//     printf("\r\n");
//     for (i=0; i<MAX_SIZE+1; i++)
//     {
//         printf("%c ", queue.data[i]);
//     }
//     printf("\r\n");

//     for (i=0, len=1; len; i++)
//     {
//         len = 4;
//         printf("%d-", lenQueue());
//         deQueue(buf, &len);
//         buf[len] = '\0';
//         printf("%d-%s\r\n", len, buf);
//     }
//     printf("\r\nend============\r\n");
// }
//     // 采用随机数测试 end//////////////////


//     ////////////////////////////测试lqueue////////////////////////
//     initLQueue();
//     lqueue.front    = 0;
//     lqueue.rear     = 0;

//     // 测试存入////////////////////////
//     printf("\r\nlqueue begin==========\r\n");
//     for (i=0, len=i+1; len; i++)
//     {
//         len = i+1;
//         enLQueue(&len);
//         printf("%d-%d ", lenLQueue(), len);

//         if (!((i+1)%3))     // 测试取出一部分数据
//         {
//             deLQueue(&len);
//             printf("de-%d ", len);
//         }
//     }
//     printf("\r\n");
//     for (i=0; i<MAX_L_SIZE; i++)
//     {
//         printf("%d ", lqueue.data[i]);
//     }
//     printf("\r\n");

//     // 测试取出/////////////////////////////
//     for (len = 1; len; )
//     {
//         printf("%d-", lenLQueue());
//         deLQueue(&len);
//         printf("%d ", len);
//     }
//     printf("\r\n");
//     for (i=0; i<MAX_L_SIZE; i++)
//     {
//         printf("%d ", lqueue.data[i]);
//     }
//     printf("\r\nlqueue end============\r\n");
//     // 测试end////////////////////
//     ////////////////////////////测试lqueue////////////////////////

//     // 测试 dtu_buffer_add_data / dtu_buffer_get_data
//     ////////////////////////////测试 begin////////////////////////
//     printf("add begin============\r\n");
//     len = 4;
//     i = 0;
//     while (i!=10)
//     {
//         sprintf(buf, "abc%d", i);
//         dtu_buffer_add_data(buf, len);
//         i++;
//     }
//     printf("add end=============\r\n");
//     printf("get begin============\r\n");
//     while (dtu_buffer_get_data(buf, &len) )
//     {
//         printf("%d-%s\r\n", len, buf);
//     }
//     printf("get end=============\r\n");
//  
