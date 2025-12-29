#include <iio/iio.h>
#include <sys/time.h>
#include <stdio.h>

#define PLUTO_INTERFACE "ip:pluto.local"
//#define PLUTO_INTERFACE "usb:"

unsigned long long runTimeMs(void);

int plutoFailures;
long long lastClock;
long long elapsedTime;
long progStartTime=0;

struct iio_context *plutoctx;
struct iio_device *plutophy;

struct iio_channel *plutochn;
const struct iio_attr *plutoattr;

void main(void)
{
    FILE *flog = fopen("/tmp/LangstoneIIO_Pluto.log", "w");

    struct iio_context_params plutoparam = {            \
        .out = flog,                                    \
        .err = flog,                                    \
        .log_level = LEVEL_INFO,                        \
        .stderr_level = LEVEL_INFO,                     \
        .timestamp_level = LEVEL_NOLOG,                 \
        .timeout_ms = 0                                 \
    };

    printf("\n\nLangstone Pluto comms test starting\n");
    printf("###################################\n\n");

    plutoctx = iio_create_context(&plutoparam, PLUTO_INTERFACE);
    if (iio_err(plutoctx) != 0)
    {
        printf("ERROR... Pluto did not respond at %s\n",PLUTO_INTERFACE);
        printf("Please check Pluto power, connection and configuration\n");
        exit(-1);
    }
    else
    {
        plutophy = iio_context_find_device(plutoctx, "ad9361-phy"); 
        if (plutophy == NULL)
        {
            printf("Something went wrong\n");
            fclose(flog);
            exit(-2);
        }
    }

    plutochn = iio_device_find_channel(plutophy, "altvoltage0", true);
    if (plutochn == NULL)
    {
        printf("channel not found");
        fclose(flog);
        exit(-3);
    }

    plutoattr = iio_channel_find_attr(plutochn, "frequency");
    if (plutoattr == NULL)
    {
        printf("attribute not found");
        fclose(flog);
        exit(-4);
    }

    printf("Pluto found at %s\n\n",PLUTO_INTERFACE);
    printf("Starting throughput test.....\n");
    plutoFailures = 0;
    lastClock = runTimeMs();

    for(int n = 0; n < 1000; n++)
    {
        int retval = iio_attr_write_longlong(plutoattr, 430000000);
        if( retval != 0 )
        {
            plutoFailures++;
        }
    }

    elapsedTime = runTimeMs() - lastClock;

    iio_context_destroy(plutoctx); 

    printf("Throughput test complete\n\n");
    printf("1000 Calls made in %lld ms with %d failures\n",elapsedTime,plutoFailures);
    printf("Pluto throughput measured at %lld calls per second\n",1000000/elapsedTime);
    printf("Typical value on a working system is 1900 calls per second\n");

    if (((1000000 / elapsedTime) < 1500) | (plutoFailures > 0))
    {
        printf("\nPluto performance is below normal.\n");
        printf("It may still work but performance may be reduced.\n");
        printf("Please check USB Connection and Power supply.\n");
        printf("Ensure Pluto is directly connected to Pi and not through a USB Hub\n\n ");
    }
    else
    {
        printf("Pluto performance appears to be OK\n\n");
    }

    fclose(flog);
}

unsigned long long runTimeMs(void)
{
struct timeval tt;
gettimeofday(&tt,NULL);
if(progStartTime==0)
  {
    progStartTime=tt.tv_sec;
  }
tt.tv_sec=tt.tv_sec - progStartTime;
return ((tt.tv_sec*1000) + (tt.tv_usec/1000));
}

