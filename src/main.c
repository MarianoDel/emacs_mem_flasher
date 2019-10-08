//----------------------------------------------------------
// #### PROJECT: MEMORY FLASHER 29LV040 - Custom Board #####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### MAIN.C #############################################
//----------------------------------------------------------

// Includes ------------------------------------------
#include "hard.h"
#include "stm32f0xx.h"
#include "gpio.h"
#include "uart.h"

#include "core_cm0.h"
#include "tim.h"
#include "spi.h"
#include "comm.h"
#include "memory.h"

#include <stdio.h>
// #include <string.h>


// External Variables --------------------------------

//-- Timers externals --------------------------------
volatile unsigned char timer_1seg = 0;
volatile unsigned short timer_led = 0;


//-- Usart externals ---------------------------------
volatile unsigned char usart1_have_data;
volatile unsigned char usart1_timeout;

//-- Comms externals ---------------------------------
volatile unsigned short comms_in_binary_timeout = 0;

// Global Variables ----------------------------------
//-- Timers globals ----------------------------------
volatile unsigned short timer_standby;
volatile unsigned short wait_ms_var = 0;


// Module Functions Declaration ----------------------
void TimingDelay_Decrement(void);


// Module Functions Definition -----------------------
int main(void)
{
    unsigned short i = 0;

    //GPIO Configuration.
    GPIO_Config();

    HV_A9_OFF;
    HV_OE_OFF;
    //Systick Timer
    if (SysTick_Config(48000))
    {
        while (1)	/* Capture error */
        {
            if (LED)
                LED_OFF;
            else
                LED_ON;

            for (i = 0; i < 255; i++)
            {
                asm (	"nop \n\t"
                        "nop \n\t"
                        "nop \n\t" );
            }
        }
    }

#ifdef HARD_TEST_MODE_LED_CYCLE
    //Hard Test LED
    while (1)
    {
        LED_TOGGLE;
        Wait_ms(1000);
    }
#endif

#ifdef HARD_TEST_MODE_MEMORY_SIGNALS
    //Hard Test LED
    while (1)
    {
        LED_ON;
        WE_ON;
        OE_ON;
        CE_ON;
        Wait_ms(1000);
        LED_OFF;
        WE_OFF;
        OE_OFF;
        CE_OFF;
        Wait_ms(1000);
    }
#endif

#ifdef HARD_TEST_MODE_HIGH_VOLTAGE_LINES
    //Hard Test LED
    while (1)
    {
        LED_ON;
        HV_A9_ON;
        HV_OE_ON;
        Wait_ms(1000);
        LED_OFF;
        HV_A9_OFF;
        HV_OE_OFF;
        Wait_ms(1000);
    }
#endif
    
#ifdef HARD_TEST_MODE_SPI_HC595
    //Hard Test SPI
    SPI_Config();
    
    // while (1)
    // {
    //     if (!timer_standby)
    //     {
    //         timer_standby = 1000;
    //         MEM_SetAddress(0x00040003);
    //     }
    // }

    // MEM_ReadByte(0x00040003);
    // MEM_SetByte(0x00040003, 0xFF);
    MEM_SetByte(0x00040003, 0x55);    
    while (1);
#endif
    
#ifdef HARD_TEST_MODE_USART
    //Activate the USART
    USART1Config();
    char buff_local [128] = { 0 };
    unsigned char readed = 0;

    Usart1Send("\nKirno Technology -- Usart Test\n");
    while(1)
    {
        Wait_ms(3000);
        if (usart1_have_data)
        {
            readed = ReadUsart1Buffer((unsigned char *) buff_local, 127);
            *(buff_local + readed) = '\n';    //cambio el '\0' por '\n' antes de enviar
            *(buff_local + readed + 1) = '\0';    //ajusto el '\0'
            Usart1Send(buff_local);
        }
    }    
#endif

#ifdef HARD_TEST_MODE_MEMORY_READ
    //Hard Test SPI
    SPI_Config();

    //Activate the USART    
    USART1Config();
    char buff_local [128] = { 0 };
    unsigned char readed[16] = { 0 };

    Usart1Send("\nKirno Technology -- Read Memory Test\n");
    
    while (1)
    {
        if (!timer_standby)
        {
            timer_standby = 1000;

            // Leer varios
            for (unsigned char i = 0; i < 16; i++)
                readed[i] = MEM_ReadByte(i);

            for (unsigned char i = 0; i < 16; i++)
            {
                sprintf(buff_local,"0x%02x ", readed[i]);
                Usart1Send(buff_local);
            }
            Usart1Send("\n");

            // Leer siempre el mismo 0x00
            // readed = MEM_ReadByte(0x00);
            // sprintf(buff_local,"0x%02x\n", readed);
            // Usart1Send(buff_local);

            // Leer siempre el mismo 0x0F
            // readed = MEM_ReadByte(0x0F);
            // sprintf(buff_local,"0x%02x\n", readed);
            // Usart1Send(buff_local);
            
        }
    }
#endif

#ifdef HARD_TEST_MODE_MEMORY_SID_MID
    //Hard Test SPI
    SPI_Config();

    //Activate the USART    
    USART1Config();
    char buff_local [128] = { 0 };
    unsigned char readed = 0;

    Usart1Send("\nKirno Technology -- SID & MID Read Memory Test\n");
    MEM_Reset();

    
    while (1)
    {
        if (!timer_standby)
        {
            timer_standby = 1000;

            // Leer SID
            MEM_SetByte(0x555, 0xAA);
            MEM_SetByte(0x2AA, 0x55);
            MEM_SetByte(0x555, 0x90);

            readed = MEM_ReadByte(0x01);
            sprintf(buff_local,"SID: 0x%02x\n", readed);
            Usart1Send(buff_local);

            // Leer MID
            MEM_SetByte(0x555, 0xAA);
            MEM_SetByte(0x2AA, 0x55);
            MEM_SetByte(0x555, 0x90);

            readed = MEM_ReadByte(0x00);
            sprintf(buff_local,"MID: 0x%02x\n", readed);
            Usart1Send(buff_local);
            
            
        }
    }
#endif
    
    //---------- END OF HARD TEST ----------//

    
    //---------- PRODUCTION PROGRAM ----------//
#ifdef PRODUCTION_PROGRAM
    // Activate SPI and Pointers
    SPI_Config();
    
    // Activate the Usart
    USART1Config();

    //--- Welcome Messages ---//
    //---- Defines from hard.h -----//
    Wait_ms(1000);
    Usart1Send("\nMemory Flasher -- Kirno Technology\n");
    
#ifdef HARD
    Usart1Send(HARD);
    Wait_ms(100);    
#else
#error	"No Hardware defined in hard.h file"
#endif

#ifdef SOFT
    Usart1Send(SOFT);
    Wait_ms(100);    
#else
#error	"No Soft Version defined in hard.h file"
#endif

    WelcomeCodeFeatures();

    MEM_Reset();
    while (1)
    {
        UpdateCommunications ();
    }

#endif    //PRODUCTION_PROGRAM
    return 0;
}
//--- End of Main ---//

void TimingDelay_Decrement(void)
{
    if (wait_ms_var)
        wait_ms_var--;

    if (timer_standby)
        timer_standby--;

    if (timer_led)
        timer_led--;

    if (usart1_timeout)
        usart1_timeout--;

    if (comms_in_binary_timeout)
        comms_in_binary_timeout--;
    
}

//--- end of file ---//

