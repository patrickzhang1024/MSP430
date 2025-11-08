#include "trans_loopbuffer.h"

uint8_t windex_position = 0; //0:windex >= rindex  1:windex < rindex

static void overflowFlagControl(loopStructure* loop)
{
    if(!loop->overflow_flag)
    {
        if(windex_position && loop->write_index >= loop->read_index)
            loop->overflow_flag = WRITE_OVERFLOW;
        else if (!windex_position && (loop->write_index + 1) < loop->read_index)
            loop->overflow_flag = READ_OVERFLOW;
    }
}

static void handledFlagControl(loopStructure* loop)
{
    if((loop->read_index  - loop->write_index) == 1)
        loop->handled_flag = 1;

    else if(loop->read_index == 0 && loop->write_index == (loop->buffersize - 1))
        loop->handled_flag = 1;

    else
        loop->handled_flag = 0;
}

void writeBuffer(loopStructure* loop, uint8_t data)
{
    if(loop->write_index < loop->buffersize)
    {
        loop->write_index++;
        loop->buffer[loop->write_index] = data;
    }
    else {
        loop->write_index = 0;
        loop->buffer[0] = data;
        windex_position = 1;
    }

    handledFlagControl(loop);
    overflowFlagControl(loop);
}

uint8_t readBuffer(loopStructure* loop)
{
    uint8_t data = loop->buffer[loop->read_index];
    loop->read_index++;
    if(loop->read_index >= loop->buffersize)
    {
        loop->read_index = 0;
        windex_position = 0;
    }

    handledFlagControl(loop);
    overflowFlagControl(loop);
    return data;
}

void readBufferConsequent(loopStructure* loop, uint8_t* deposit_buf, uint32_t size)
{
    uint32_t i = 0;
    while(size)
    {
        deposit_buf[i] = readBuffer(loop);
        i++;
        size--;
    }
}

uint32_t getReadIndex(loopStructure* loop)
{
    return (loop->read_index);
}

void addReadIndex(loopStructure* loop, uint32_t add)
{
    loop->read_index += add;
    if(loop->read_index >= loop->buffersize)
        loop->read_index = loop->read_index - loop->buffersize - 1;
}

uint32_t subtractReadIndex(loopStructure* loop, uint32_t index)
{
    if(loop->write_index > index)
        return (loop->write_index - index);
    else
        return (loop->write_index + loop->buffersize - 1 - index);
}

void getBufferFragment(loopStructure* loop, uint8_t* deposit_buf, uint32_t size)
{
    uint32_t i = 0, j = loop->read_index;
    while(size)
    {
        deposit_buf[i] = loop->buffer[j];
        i++;
        j++;
        size--;

        if(j >= loop->buffersize)
            j = 0;
    }
}
