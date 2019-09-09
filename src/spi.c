//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F030
// ##
// #### SPI.C #################################
//---------------------------------------------
#include "spi.h"
#include "stm32f0xx.h"


/* Externals ------------------------------------------------------------------*/
extern volatile unsigned char TxBuffer_SPI [];
extern volatile unsigned char RxBuffer_SPI [];
extern volatile unsigned char *pspi_tx;
extern volatile unsigned char *pspi_rx;
extern volatile unsigned char spi_bytes_left;

/* Globals --------------------------------------------------------------------*/


/* Module Functions -----------------------------------------------------------*/
void SPI_Config(void)
{
    unsigned int temp;
    
    if (!RCC_SPI1_CLK)
        RCC_SPI1_CLK_ON;

    //Configuracion SPI
    SPI1->CR1 = 0;

#if defined SPI_MASTER
    //SPI speed; clk / 256; master
    SPI1->CR1 |= SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2;
#elif defined SPI_SLAVE
    //SPI speed; clk / 256; master
    //TODO: chequear esta configuracion, la hice de memoria
    SPI1->CR1 |= SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2 |SPI_CR1_SSM;
#else
#error "Select Peripheral use on spi.h"
#endif

#ifdef SPI_DATA_VALID_ON_FALLING_CLK
    //CPOL High; CPHA first clock
    SPI1->CR1 |= SPI_CR1_MSTR | SPI_CR1_CPOL | SPI_CR1_SSM | SPI_CR1_SSI;
#endif
#ifdef SPI_DATA_VALID_ON_RISING_CLK
    //CPOL High; CPHA second clock
    SPI1->CR1 |= SPI_CR1_MSTR | SPI_CR1_CPOL | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_CPHA;
#endif
    //thresh 8 bits; data 8 bits;
    SPI1->CR2 = SPI_CR2_FRXTH | SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0;

    //Activo en GPIOB PB3 clk y PB5 mosi
    temp = GPIOB->AFR[0];
    temp &= 0xFF0F0FFF;
    temp |= 0x00000000;	//PB5 -> AF0 PB3 -> AF0
    GPIOB->AFR[0] = temp;

    //habilito periferico
    SPI1->CR1 |= SPI_CR1_SPE;    
}


unsigned char SPI_Send_Receive (unsigned char a)
{
    unsigned char rx;
    unsigned char j;

    //primero limpio buffer rx spi
    while ((SPI1->SR & SPI_SR_RXNE) == 1)
    {
        rx = SPI1->DR & 0x0F;
    }

    //espero que haya lugar en el buffer
    while ((SPI1->SR & SPI_SR_TXE) == 0);

    *(__IO uint8_t *) ((uint32_t)SPI1 + (uint32_t)0x0C) = a; //evito enviar 16bits problemas de compilador

    //espero tener el dato en RX
    for (j = 0; j < 150; j++)
    {
    	asm("nop");
    }

    return (SPI1->DR & 0x0F);
}


void SPI_Busy_Wait (void)
{
    //espero que se transfiera el dato
    while ((SPI1->SR & SPI_SR_BSY) != 0);
}


void SPI_Send_Multiple (unsigned char a)
{
    //espero que haya lugar en el buffer
    while ((SPI1->SR & SPI_SR_TXE) == 0);

    //*(__IO uint8_t *) SPI1->DR = a;
    *(__IO uint8_t *) ((uint32_t)SPI1 + (uint32_t)0x0C) = a; //evito enviar 16bits problemas de compilador

}


void SPI_Send_Single (unsigned char a)
{
    //espero que se libere el buffer
    while ((SPI1->SR & SPI_SR_TXE) == 0);

    //tengo espacio
    //SPI1->DR = a;
    //SPI1->DR = a;
    *(__IO uint8_t *) ((uint32_t)SPI1 + (uint32_t)0x0C) = a; //evito enviar 16bits problemas de compilador

    //espero que se transfiera el dato
    while ((SPI1->SR & SPI_SR_BSY) != 0);
}


unsigned char SPI_Receive_Single (void)
{
    unsigned char dummy;

    //espero que se libere el buffer
    while (((SPI1->SR & SPI_SR_TXE) == 0) || ((SPI1->SR & SPI_SR_BSY) != 0));

    //limpio buffer RxFIFO
    while ((SPI1->SR & SPI_SR_RXNE) != 0)
        dummy = SPI1->DR;

    *(__IO uint8_t *) ((uint32_t)SPI1 + (uint32_t)0x0C) = 0xff; //evito enviar 16bits problemas de compilador

    //espero que se transfiera el dato
    while ((SPI1->SR & SPI_SR_BSY) != 0);

    dummy = (unsigned char) SPI1->DR;
    return dummy;
}

//--- end of file ---//
