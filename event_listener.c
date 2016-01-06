#include "event_listener.h"

#define NUM_EVENTS  5

#define NON_STOP    1

/* error codes */
#define NO_ERROR 		0
#define ERROR			1

#define KEY_CHANNEL_UP             62
#define KEY_CHANNEL_DOWN           61
#define KEY_MUTE_                   60
#define KEY_VOLUME_UP              63
#define KEY_VOLUME_DOWN            64
#define KEY_EXIT_                  102
#define KEY_INFO_                  358

#define ASSERT_TDP_RESULT(x,y)  if(NO_ERROR == x) \
                                    printf("%s success\n", y); \
                                else{ \
                                    printf("%s fail\n", y); \
                                    return -1; \
                                }


static int32_t inputFileDesc;

void initEventListener()
{

}

int32_t getKeys(int32_t count, uint8_t* buf, int32_t* eventsRead)
{
    int32_t ret = 0;
    
    /* read input events and put them in buffer */
    ret = read(inputFileDesc, buf, (size_t)(count * (int)sizeof(struct input_event)));
    if(ret <= 0)
    {
        printf("Error code %d", ret);
        return ERROR;
    }
    /* calculate number of read events */
    *eventsRead = ret / (int)sizeof(struct input_event);
    
    return NO_ERROR;
}