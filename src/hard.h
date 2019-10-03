//--------------------------------------------------
// #### PROYECTO MEMORY FLASHER - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### HARD.H #####################################
//--------------------------------------------------
#ifndef _HARD_H_
#define _HARD_H_

#include "stm32f0xx.h"


//-- Defines For Configuration -------------------
//---- Configuration for Hardware Versions -------
// #define HARDWARE_VERSION_1_1
#define HARDWARE_VERSION_1_0


// #define SOFTWARE_VERSION_1_1
#define SOFTWARE_VERSION_1_0


//---- Type of Program -----------------------
// #define HARD_TEST_MODE_LED_CYCLE
// #define HARD_TEST_MODE_MEMORY_SIGNALS
// #define HARD_TEST_MODE_HIGH_VOLTAGE_LINES
// #define HARD_TEST_MODE_SPI_HC595
// #define HARD_TEST_MODE_USART
#define PRODUCTION_PROGRAM

//---- Features Configuration ----------------
#define USE_LED_ON_EACH_CYCLE
// #define USE_LED_IN_PROT
// #define USE_LED_USART1_END_OF_PCKT

#define USE_FREQ_48KHZ     //clean some errors

//---- End of Features Configuration ----------



//--- Hardware Welcome Code ------------------//
#ifdef HARDWARE_VERSION_1_1
#define HARD "Hardware Version: 1.1\n"
#endif
#ifdef HARDWARE_VERSION_1_0
#define HARD "Hardware Version: 1.0\n"
#endif
#ifdef HARDWARE_VERSION_2_0
#define HARD "Hardware Version: 2.0\n"
#endif

//--- Software Welcome Code ------------------//
#ifdef SOFTWARE_VERSION_1_2
#define SOFT "Software Version: 1.2\n"
#endif
#ifdef SOFTWARE_VERSION_1_1
#define SOFT "Software Version: 1.1\n"
#endif
#ifdef SOFTWARE_VERSION_1_0
#define SOFT "Software Version: 1.0\n"
#endif

//-------- Configuration for Outputs-Channels -----


//---- Configuration for Firmware-Programs --------


//-------- Configuration for Outputs-Firmware ------


//-- End Of Defines For Configuration ---------------

//GPIOA pin0    
//GPIOA pin1    
//GPIOA pin2    
//GPIOA pin3    
//GPIOA pin4
//GPIOA pin5    
//GPIOA pin6
//GPIOA pin7    eight bits bidirectional port

//GPIOB pin0
#define LED ((GPIOB->ODR & 0x0001) != 0)
#define LED_ON GPIOB->BSRR = 0x00000001
#define LED_OFF GPIOB->BSRR = 0x00010000

//GPIOB pin1    TIM3 CH1 - CH4

//GPIOA pin8    
#define CE ((GPIOA->ODR & 0x0100) == 0)
#define CE_OFF GPIOA->BSRR = 0x00000100
#define CE_ON GPIOA->BSRR = 0x01000000

//GPIOA pin9
#define WE ((GPIOA->ODR & 0x0200) == 0)
#define WE_OFF GPIOA->BSRR = 0x00000200
#define WE_ON GPIOA->BSRR = 0x02000000

//GPIOA pin10
#define OE ((GPIOA->ODR & 0x0400) == 0)
#define OE_OFF GPIOA->BSRR = 0x00000400
#define OE_ON GPIOA->BSRR = 0x04000000

//GPIOA pin11
#define HV_OE ((GPIOA->ODR & 0x0800) == 0)
#define HV_OE_ON GPIOA->BSRR = 0x00000800
#define HV_OE_OFF GPIOA->BSRR = 0x08000000

//GPIOA pin12
#define HV_A9 ((GPIOA->ODR & 0x1000) == 0)
#define HV_A9_ON GPIOA->BSRR = 0x00001000
#define HV_A9_OFF GPIOA->BSRR = 0x10000000

//GPIOA pin13
//GPIOA pin14    NC

//GPIOA pin15    
#define LC_HC595 ((GPIOA->ODR & 0x0010) == 0)
#define LC_HC595_ON GPIOA->BSRR = 0x00008000
#define LC_HC595_OFF GPIOA->BSRR = 0x80000000

//GPIOB pin3     SPI CLK

//GPIOB pin4     
#define OE_HC595 ((GPIOB->ODR & 0x0010) == 0)
#define OE_HC595_OFF GPIOB->BSRR = 0x00000010
#define OE_HC595_ON GPIOB->BSRR = 0x00100000

//GPIOB pin5     SPI MOSI

//GPIOB pin6
//GPIOB pin7    NC


//ESTADOS DEL LED
typedef enum
{    
    START_BLINKING = 0,
    WAIT_TO_OFF,
    WAIT_TO_ON,
    WAIT_NEW_CYCLE
} led_state_t;

//ESTADOS DEL BUZZER
typedef enum
{    
    BUZZER_INIT = 0,
    BUZZER_TO_STOP,

    BUZZER_MULTIPLE_LONG,
    BUZZER_MULTIPLE_LONGA,
    BUZZER_MULTIPLE_LONGB,

    BUZZER_MULTIPLE_HALF,
    BUZZER_MULTIPLE_HALFA,
    BUZZER_MULTIPLE_HALFB,

    BUZZER_MULTIPLE_SHORT,
    BUZZER_MULTIPLE_SHORTA,
    BUZZER_MULTIPLE_SHORTB
    
} buzzer_state_t;

//COMANDOS DEL BUZZER	(tienen que ser los del estado de arriba)
#define BUZZER_STOP_CMD		BUZZER_TO_STOP
#define BUZZER_LONG_CMD		BUZZER_MULTIPLE_LONG
#define BUZZER_HALF_CMD		BUZZER_MULTIPLE_HALF
#define BUZZER_SHORT_CMD	BUZZER_MULTIPLE_SHORT

#define TIM_BIP_SHORT		50
#define TIM_BIP_SHORT_WAIT	100
#define TIM_BIP_HALF		200
#define TIM_BIP_HALF_WAIT	500
#define TIM_BIP_LONG		1200
#define TIM_BIP_LONG_WAIT	1500


//Estados Externos de LED BLINKING
#define LED_NO_BLINKING    0
#define LED_TREATMENT_STANDBY    1
#define LED_TREATMENT_GENERATING    2
#define LED_TREATMENT_ERROR    6

//--- Stringtify Utils -----------------------
#define STRING_CONCAT(str1,str2) #str1 " " #str2
#define STRING_CONCAT_NEW_LINE(str1,str2) xstr(str1) #str2 "\n"
#define xstr_macro(s) str_macro(s)
#define str_macro(s) #s

#define LED_TOGGLE do { if (LED) \
                            LED_OFF; \
                        else         \
                            LED_ON;  \
                      } while (0)


#define HC595_ENABLE_PARALLEL_OUTPUTS    OE_HC595_ON
#define HC595_DISABLE_PARALLEL_OUTPUTS    OE_HC595_OFF

/* Module Functions ------------------------------------------------------------*/
void ChangeLed (unsigned char);
void UpdateLed (void);
void UpdateBuzzer (void);
void BuzzerCommands(unsigned char, unsigned char);
void WelcomeCodeFeatures (void);

#endif /* _HARD_H_ */

//--- end of file ---//

