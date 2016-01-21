
#include "event_listener.h"

#define dbg printf("\nfunction:%s(%d)\n",__FUNCTION__,__LINE__)
;


/* error codes */
#define NO_ERROR 		0
#define ERROR			1


#define ASSERT_TDP_RESULT(x,y)  if(NO_ERROR == x) \
                                    printf("%s success\n", y); \
                                else{ \
                                    printf("%s fail\n", y); \
                                    return -1; \
                                }


static int32_t inputFileDesc;
    
static const char* dev = "/dev/input/event0";
static char deviceName[20];

void initEventListener()
{
    inputFileDesc = open(dev, O_RDWR);
    if(inputFileDesc == -1)
    {
        printf("Error while opening device (%s) !", strerror(errno));
            return ;
    }
    
    ioctl(inputFileDesc, EVIOCGNAME(sizeof(deviceName)), deviceName);
        printf("RC device opened succesfully [%s]\n", deviceName);
    
    
}

void deintiEventListener()
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
