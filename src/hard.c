//--------------------------------------------------
// #### PROYECTO LIGHT TREATMENT - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### HARD.C #####################################
//--------------------------------------------------

#include "hard.h"
#include "stm32f0xx.h"
#include "uart.h"
#include "tim.h"

#include <stdio.h>



/* Externals variables ---------------------------------------------------------*/
extern volatile unsigned short timer_led;
extern volatile unsigned short timer_buzzer;
#define buzzer_timeout timer_buzzer

/* Global variables ------------------------------------------------------------*/
//para el led
led_state_t led_state = START_BLINKING;
unsigned char blink = 0;
unsigned char how_many_blinks = 0;



/* Module Functions ------------------------------------------------------------*/

//cambia configuracion de bips del LED
void ChangeLed (unsigned char how_many)
{
    how_many_blinks = how_many;
    led_state = START_BLINKING;
}

//mueve el LED segun el estado del Pote
void UpdateLed (void)
{
    switch (led_state)
    {
        case START_BLINKING:
            blink = how_many_blinks;
            
            if (blink)
            {
                LED_ON;
                timer_led = 200;
                led_state++;
                blink--;
            }
            break;

        case WAIT_TO_OFF:
            if (!timer_led)
            {
                LED_OFF;
                timer_led = 200;
                led_state++;
            }
            break;

        case WAIT_TO_ON:
            if (!timer_led)
            {
                if (blink)
                {
                    blink--;
                    timer_led = 200;
                    led_state = WAIT_TO_OFF;
                    LED_ON;
                }
                else
                {
                    led_state = WAIT_NEW_CYCLE;
                    timer_led = 2000;
                }
            }
            break;

        case WAIT_NEW_CYCLE:
            if (!timer_led)
                led_state = START_BLINKING;

            break;

        default:
            led_state = START_BLINKING;
            break;
    }
}


void WelcomeCodeFeatures (void)
{
    char str[128] = {};
    
    // Features mostly on hardware
#ifdef USE_LED_ON_EACH_CYCLE
    sprintf(str,"[%s] %s\n", __FILE__, str_macro(USE_LED_ON_EACH_CYCLE));
    Usart1Send(str);
    Wait_ms(30);    
#endif
    
#ifdef USE_LED_IN_PROT
    sprintf(str,"[%s] %s\n", __FILE__, str_macro(USE_LED_IN_PROT));
    Usart1Send(str);
    Wait_ms(30);    
#endif    
}

