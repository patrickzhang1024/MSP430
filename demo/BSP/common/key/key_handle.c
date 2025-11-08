#include "key_handle.h"

/*
struct KEY_STATE key;
void exampleInit()
{
    key.shortpress_threshold = 10; //10ms
    key.longpress_threshold = 1000;  //1000ms
    key.release_threshold = 10; //10ms
    key.(*readKeyFunc)() = readKeyStatus();  //1 press 0 release

    key.key_press_count = 0;
    key.key_release_count = 0;
    key.key_usage = 1;
    key.key_state = KEY_RELEASE;
}
*/

//put this function into 1ms loop
void updateKeyStaus(keyStructure* target)
{
    if(target->readKeyFunc())
        (target->key_press_count)++;
    else if(target->key_press_count)
        (target->key_release_count)++;

    if((target->key_release_count) > (target->release_threhold))
    {
        if((target->key_press_count) > (target->longpress_threhold))
        {
            if(target->key_usage)
                target->key_state = KEY_LONG_PRESS;
            else {
                target->key_state = KEY_RELEASE;
                target->key_usage = 1;
            }
        }
        else if((target->key_press_count) > (target->shortpress_threshold))
            target->key_state = KEY_SHORT_PRESS;
        else
            target->key_state = KEY_RELEASE;

        target->key_press_count = 0;
        target->key_release_count = 0;
    }
    else if (!(target->key_usage))
        target->key_state = KEY_RELEASE;
    else if ((target->key_press_count) > (target->longpress_threhold))
        target->key_state = KEY_LONG_HOLD;
}

uint8_t getKeyStatus(keyStructure* target)
{
    return target->key_state;
}

void useKeyStatus(keyStructure* target)
{
    if(target->key_state == KEY_LONG_HOLD)
        target->key_usage = 0;
    else
        target->key_state = KEY_RELEASE;
}

