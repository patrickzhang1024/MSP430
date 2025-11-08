#ifndef __TRANS_LOOPBUFFER_H__
#define __TRANS_LOOPBUFFER_H__

#include <stdint.h>
#include <stddef.h>
//#define MALLOC_ABLE                  //use malloc buffer or static buffer

typedef enum
{
    NO_OVERFLOW       = 0,
    WRITE_OVERFLOW,
    READ_OVERFLOW,
} LOOP_OVERFLOW;


typedef struct loopStructure{
    uint32_t buffersize;
    uint8_t *buffer;

    uint32_t write_index;
    uint32_t read_index;

    uint8_t overflow_flag;           //handle speed to slow, Windex data is recover Rindex data
    uint8_t handled_flag;            //Windex == Rindex handle finished
}loopStructure;

void writeBuffer(loopStructure* loop, uint8_t data);                                     //填充buf wirteIndex增加
uint8_t readBuffer(loopStructure* loop);                                                 //读取buf readIndex增加
void readBufferConsequent(loopStructure* loop, uint8_t* deposit_buf, uint32_t size);     //读取buf片段 readIndex增加

uint32_t getReadIndex(loopStructure* loop);
void addReadIndex(loopStructure* loop, uint32_t add);                                   //增加readIndex
uint32_t subtractReadIndex(loopStructure* loop, uint32_t index);                        //得到readIndex和index的差值

void getBufferFragment(loopStructure* loop, uint8_t* deposit_buf, uint32_t size);       //从readIndex处复制一个片段buf

#endif
