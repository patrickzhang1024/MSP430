#ifndef KEY_HANDLE_H__
#define KEY_HANDLE_H__

#include <stdint.h>
#include <stddef.h>

typedef enum {
  KEY_RELEASE = 0,
  KEY_SHORT_PRESS,
  KEY_LONG_PRESS,
  KEY_LONG_HOLD,
}KEY_STATE;

typedef struct {
    //user defined
    uint32_t shortpress_threshold;
    uint32_t longpress_threhold;
    uint32_t release_threhold;
    uint8_t (*readKeyFunc)();

    //user don't care
    uint32_t key_press_count;
    uint32_t key_release_count;
    uint8_t key_usage;            //if long hold state has been used, long press state won't generate after release.
    uint8_t key_state;
}keyStructure;

//extern void exampleInit();
extern void updateKeyStaus(keyStructure* target);

extern uint8_t getKeyStatus(keyStructure* target);
extern void useKeyStatus(keyStructure* target);

#endif
