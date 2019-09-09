//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F030
// ##
// #### SPI.H #################################
//---------------------------------------------
#ifndef SPI_H
#define SPI_H

//Clock Peripherals
#define RCC_SPI1_CLK 			(RCC->APB2ENR & 0x00001000)
#define RCC_SPI1_CLK_ON 		RCC->APB2ENR |= 0x00001000
#define RCC_SPI1_CLK_OFF		RCC->APB2ENR &= ~0x00001000


/* Uncomment the line below if you will use the SPI peripheral as a Master */
#define SPI_MASTER
// #define SPI_SLAVE

//When to validate the Data sended
#define SPI_DATA_VALID_ON_RISING_CLK
// #define SPI_DATA_VALID_ON_FALLING_CLK


//-------- Functions -------------
void SPI_Config(void);
unsigned char SPI_Send_Receive (unsigned char);
void SPI_Busy_Wait (void);
void SPI_Send_Multiple (unsigned char);
void SPI_Send_Single (unsigned char);
unsigned char SPI_Receive_Single (void);
#endif
