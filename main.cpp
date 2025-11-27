#include "mbed.h"

#if MBED_MAJOR_VERSION < 6
#define UnbufferedSerial RawSerial
#endif

#define HOST_COM_SEL            HOST_COM_NULINK
#define ESP8266_AT_SEL          ESP8266_AT_ONBOARD

#define ESP8266_AT_ONBOARD      1   // On-board ESP8266
#define ESP8266_AT_EXTERN       2   // External ESP8266 through UNO, target dependent

#define HOST_COM_NULINK         1   // Host COM port through Nu-Link
#define HOST_COM_NUBRIDGE       2   // Host COM port through Nu-Bridge

#if HOST_COM_SEL == HOST_COM_NULINK
UnbufferedSerial    pc(USBTX, USBRX);

#elif HOST_COM_SEL == HOST_COM_NUBRIDGE
#if defined(TARGET_NUMAKER_IOT_M467)
UnbufferedSerial    pc(D3, D2);

#elif defined(TARGET_NUMAKER_IOT_M487)
UnbufferedSerial    pc(D3, D2);

#elif defined(TARGET_NU_PFM_M2351)
UnbufferedSerial    pc(D4, D5);

#elif defined(TARGET_M2354)
UnbufferedSerial    pc(D4, D5);

#endif
#endif

#if ESP8266_AT_SEL == ESP8266_AT_ONBOARD

#if defined(TARGET_NUMAKER_IOT_M467)
UnbufferedSerial    dev(PC_1, PC_0);

#elif defined(TARGET_NUMAKER_IOT_M487)
UnbufferedSerial    dev(PH_8, PH_9);

#elif defined(TARGET_NU_PFM_M2351)
DigitalIn esp_gpio0(PD_6);          // Go boot mode by default
                                    // User can change to F/W update mode by short'ing ESP8266 GPIO0/GND
                                    // before power-on
DigitalOut esp_pwr_off(PD_7, 1);    // Disable power to on-board ESP8266
UnbufferedSerial    dev(PD_1, PD_0);

#elif defined(TARGET_NU_M2354)
UnbufferedSerial    dev(PC_7, PC_6);

#endif

#elif ESP8266_AT_SEL == ESP8266_AT_EXTERN

UnbufferedSerial    dev(D1, D0);

#endif


DigitalOut led1(LED1);
DigitalOut led2(LED2);

void dev_recv()
{
    led1 = !led1;
    while(dev.readable()) {
#if MBED_MAJOR_VERSION >= 6
        uint8_t c;
        dev.read(&c, 1);
        pc.write(&c, 1);
#else
        pc.putc(dev.getc());
#endif
    }
}

void pc_recv()
{
    led2 = !led2;
    while(pc.readable()) {
#if MBED_MAJOR_VERSION >= 6
        uint8_t c;
        pc.read(&c, 1);
        dev.write(&c, 1);
#else
        dev.putc(pc.getc());
#endif
    }
}

int main()
{
#if ESP8266_AT_SEL == ESP8266_AT_ONBOARD
#if defined(TARGET_NU_PFM_M2351)
    wait_ms(50);
    esp_pwr_off = 0;                // Turn on on-board ESP8266
    wait_ms(50);
#endif
#endif

    pc.baud(115200);
    dev.baud(115200);
    
    pc.attach(&pc_recv, SerialBase::RxIrq);
    dev.attach(&dev_recv, SerialBase::RxIrq);
    
    while(1) {
        sleep();
    }
}

