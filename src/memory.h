//--------------------------------------------------
// #### MEMORY FLASHER 29LV040   - Custom Board ####
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F030
// ##
// #### MEMORY.H ###################################
//--------------------------------------------------
#ifndef _MEMORY_H_
#define _MEMORY_H_

//--- Exported types ---//
typedef struct {
    unsigned char manufacturer;
    unsigned char silicon;
    unsigned char protected;
} silicon_t;

//--- Exported constants ---//
#define SECTORS_QTTY    8
#define SECTORS_LENGHT    0x10000


#define SA7_PROTECTED    0x80
#define SA6_PROTECTED    0x40
#define SA5_PROTECTED    0x20
#define SA4_PROTECTED    0x10
#define SA3_PROTECTED    0x08
#define SA2_PROTECTED    0x04
#define SA1_PROTECTED    0x02
#define SA0_PROTECTED    0x01

#define SA7_ADDR    0x70000
#define SA6_ADDR    0x60000
#define SA5_ADDR    0x50000
#define SA4_ADDR    0x40000
#define SA3_ADDR    0x30000
#define SA2_ADDR    0x20000
#define SA1_ADDR    0x10000
#define SA0_ADDR    0x00000

#define SA_LENGHT    SECTORS_LENGHT

//--- Defines for Configuration ---------------------------------
//--- End of Defines for Configuration --------------------------

//--- Exported functions ---//
void MEM_Reset (void);
void MEM_SetAddress (unsigned int);
unsigned char MEM_GetManufacturer (silicon_t *);
unsigned char MEM_GetSilicon (silicon_t *);
unsigned char MEM_GetProtectedSectors (silicon_t *);

unsigned char MEM_ReadByte (unsigned int);
void MEM_WriteByte (unsigned int, unsigned char);
void MEM_SectorErase (unsigned int);
void MEM_ChipErase (void);

#endif    /* _MEMORY_H_ */

//--- end of file ---//
