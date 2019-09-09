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

/* Externals ------------------------------------------------------------------*/


/* Globals --------------------------------------------------------------------*/


/* Module Private Functions ---------------------------------------------------*/
// void MEM_SetAddress (unsigned int);
void MEM_LowDelay (void);

/* Module Functions -----------------------------------------------------------*/
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


unsigned char MEM_GetManufacturer (silicon_t * m_info)
{
    m_info->manufacturer = 194;
    return resp_ok;
}


unsigned char MEM_GetSilicon (silicon_t * m_info)
{
    m_info->silicon = 79;
    return resp_ok;
}


unsigned char MEM_GetProtectedSectors (silicon_t * m_info)
{
    m_info->protected = 0xFF;
    return resp_ok;
}


void MEM_LowDelay (void)
{
    for (unsigned char j = 0; j < 30; j++)
    {
    	asm("nop");
    }
}

//--- end of file ---//
