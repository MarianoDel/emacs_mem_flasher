//--------------------------------------------------
// #### MEMORY FLASHER 29LV040   - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### MEMORY.C ###################################
//--------------------------------------------------

/* Includes -------------------------------------------------------------------*/
#include "memory.h"
#include "spi.h"
#include "hard.h"
#include "comm.h"
#include "gpio.h"

/* Externals ------------------------------------------------------------------*/


/* Globals --------------------------------------------------------------------*/


/* Module Private Functions ---------------------------------------------------*/
// void MEM_SetAddress (unsigned int);
void MEM_LowDelay (void);
void MEM_SetByte (unsigned int, unsigned char);

/* Module Functions -----------------------------------------------------------*/


unsigned char MEM_GetManufacturer (silicon_t * m_info)
{
    MEM_SetByte(0x555, 0xAA);
    MEM_SetByte(0x2AA, 0x55);
    MEM_SetByte(0x555, 0x90);

    m_info->manufacturer = MEM_ReadByte(0x00);
    // m_info->manufacturer = 194;
    return resp_ok;
}


unsigned char MEM_GetSilicon (silicon_t * m_info)
{
    MEM_SetByte(0x555, 0xAA);
    MEM_SetByte(0x2AA, 0x55);
    MEM_SetByte(0x555, 0x90);

    m_info->silicon = MEM_ReadByte(0x01);
    // m_info->silicon = 79;
    return resp_ok;
}


unsigned char MEM_GetProtectedSectors (silicon_t * m_info)
{
    unsigned int sector_addr = 0;
    unsigned char prot = 0;
    
    for (unsigned char i = 0; i < SECTORS_QTTY; i++)
    {
        MEM_SetByte(0x555, 0xAA);
        MEM_SetByte(0x2AA, 0x55);
        MEM_SetByte(0x555, 0x90);

        sector_addr = SECTORS_LENGHT * i;
        sector_addr |= 0x02;

        prot |= MEM_ReadByte(sector_addr);

        prot <<= 1;
    }
    
    m_info->protected = prot;
    return resp_ok;
}


void MEM_Reset (void)
{
    MEM_SetByte(0x00, 0xF0);
}

void MEM_SetAddress (unsigned int addr)
{
    char addr_u8 = 0;

    addr_u8 = (addr & 0xFF0000) >> 16;
    SPI_Send_Multiple (addr_u8);

    addr_u8 = (addr & 0x00FF00) >> 8;
    SPI_Send_Multiple (addr_u8);

    addr_u8 = (addr & 0x0000FF);
    SPI_Send_Multiple (addr_u8);

    SPI_Busy_Wait();
    
    OE_HC595_ON;
    MEM_LowDelay();
    OE_HC595_OFF;
}


void MEM_LowDelay (void)
{
    for (unsigned char j = 0; j < 30; j++)
    {
    	asm("nop");
    }
}


void MEM_FastDelay (void)
{
    asm("nop");
    asm("nop");
    asm("nop");    
}


void MEM_SetByte (unsigned int addr, unsigned char data)
{
    //port to output
    GPIO_PortAOutput();

    OE_OFF;
    MEM_SetAddress(addr);
    CE_ON;
    GPIO_PortAWrite(data);
    WE_ON;
    MEM_FastDelay();
    WE_OFF;
    CE_OFF;
}


unsigned char MEM_ReadByte (unsigned int addr)
{
    unsigned char data = 0;

    //port to input
    GPIO_PortAInput();

    WE_OFF;
    MEM_SetAddress(addr);
    CE_ON;
    OE_ON;
    MEM_FastDelay();
    data = GPIO_PortARead();
    OE_OFF;
    CE_OFF;
    
    return data;
}

//--- end of file ---//
