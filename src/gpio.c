//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### GPIO.C ################################
//---------------------------------------------

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"
#include "stm32f0xx.h"
#include "hard.h"



//--- Private typedef ---//
//--- Private define ---//
//--- Private macro ---//
//--- Private variables ---//
//--- Private function prototypes ---//
//--- Private functions ---//

//-------------------------------------------//
// @brief  GPIO configure.
// @param  None
// @retval None
//------------------------------------------//
void GPIO_Config (void)
{
    unsigned long temp;

    //--- MODER ---//
    //00: Input mode (reset state)
    //01: General purpose output mode
    //10: Alternate function mode
    //11: Analog mode

    //--- OTYPER ---//
    //These bits are written by software to configure the I/O output type.
    //0: Output push-pull (reset state)
    //1: Output open-drain

    //--- ORSPEEDR ---//
    //These bits are written by software to configure the I/O output speed.
    //x0: Low speed.
    //01: Medium speed.
    //11: High speed.
    //Note: Refer to the device datasheet for the frequency.

    //--- PUPDR ---//
    //These bits are written by software to configure the I/O pull-up or pull-down
    //00: No pull-up, pull-down
    //01: Pull-up
    //10: Pull-down
    //11: Reserved


#ifdef GPIOA_ENABLE
    //--- GPIO A ---//
    if (!GPIOA_CLK)
        GPIOA_CLK_ON;

    temp = GPIOA->MODER;	//2 bits por pin
    temp &= 0x3C00FFFF;		//PA0 - PA7 no change
    temp |= 0x41550000;		//PA8 - PA12 & PA15 output
    GPIOA->MODER = temp;

    temp = GPIOA->OTYPER;	//1 bit por pin
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;
    GPIOA->OTYPER = temp;
    
    temp = GPIOA->OSPEEDR;	//2 bits por pin
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;		
    GPIOA->OSPEEDR = temp;

    temp = GPIOA->PUPDR;	//2 bits por pin
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;
    GPIOA->PUPDR = temp;

#ifdef GPIOB_ENABLE

    //--- GPIO B ---//
    if (!GPIOB_CLK)
        GPIOB_CLK_ON;

    temp = GPIOB->MODER;	//2 bits por pin
    temp &= 0xFFFF003C;		//PB0 output, PB3 PB5 (alternative) PB4 output ; PB6 PB7 (alternative)
    temp |= 0x0000A981;
    GPIOB->MODER = temp;

    temp = GPIOB->OTYPER;	//1 bit por pin
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;
    GPIOB->OTYPER = temp;

    temp = GPIOB->OSPEEDR;	//2 bits por pin
    temp &= 0xFFFFC03F;        //PB3 PB4 PB5 PB6
    temp |= 0x00000000;		//low speed
    GPIOB->OSPEEDR = temp;

    temp = GPIOB->PUPDR;	//2 bits por pin
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;
    GPIOB->PUPDR = temp;

#endif

#ifdef GPIOF_ENABLE

    //--- GPIO F ---//
    if (!GPIOF_CLK)
        GPIOF_CLK_ON;

    temp = GPIOF->MODER;
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;
    GPIOF->MODER = temp;

    temp = GPIOF->OTYPER;
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;
    GPIOF->OTYPER = temp;

    temp = GPIOF->OSPEEDR;
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;
    GPIOF->OSPEEDR = temp;

    temp = GPIOF->PUPDR;
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;
    GPIOF->PUPDR = temp;

#endif

#endif    //V1.0
    
#ifdef WITH_EXTI
    //Interrupt en PA8
    if (!SYSCFG_CLK)
        SYSCFG_CLK_ON;

    SYSCFG->EXTICR[0] = 0x00000000; //Select Port A
    SYSCFG->EXTICR[1] = 0x00000000; //Select Port A
    EXTI->IMR |= 0x0100; 			//Corresponding mask bit for interrupts PA8
    EXTI->EMR |= 0x0000; 			//Corresponding mask bit for events
    EXTI->RTSR |= 0x0100; 			//Pin Interrupt line on rising edge PA8
    EXTI->FTSR |= 0x0100; 			//Pin Interrupt line on falling edge PA8

    NVIC_EnableIRQ(EXTI4_15_IRQn);
    NVIC_SetPriority(EXTI4_15_IRQn, 2);
#endif
}

inline void EXTIOff (void)
{
	EXTI->IMR &= ~0x00000100;
}

inline void EXTIOn (void)
{
	EXTI->IMR |= 0x00000100;
}


void GPIO_PortAOutput (void)
{
    unsigned long temp;

    temp = GPIOA->MODER;	//2 bits por pin
    temp &= 0xFFFF0000;		//PA7 - PA0 output
    temp |= 0x00005555;
    GPIOA->MODER = temp;

}


void GPIO_PortAInput (void)
{
    unsigned long temp;

    temp = GPIOA->MODER;	//2 bits por pin
    temp &= 0xFFFF0000;		//PA7 - PA0 input
    temp |= 0x00000000;
    GPIOA->MODER = temp;

}


void GPIO_PortAWrite (unsigned char a)
{
    unsigned short data = 0;
    
    data = GPIOA->ODR;
    data &= 0xFF00;
    data |= a;

    GPIOA->ODR = data;
}


unsigned char GPIO_PortARead (void)
{
    unsigned short data = 0;

    data = GPIOA->IDR & 0x00FF;
    
    return (unsigned char) data;
}


//--- end of file ---//
