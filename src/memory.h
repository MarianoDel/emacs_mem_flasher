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
#define SA7_PROTECTED    0x80
#define SA6_PROTECTED    0x40
#define SA5_PROTECTED    0x20
#define SA4_PROTECTED    0x10
#define SA3_PROTECTED    0x08
#define SA2_PROTECTED    0x04
#define SA1_PROTECTED    0x02
#define SA0_PROTECTED    0x01

//--- Defines for Configuration ---------------------------------
//--- End of Defines for Configuration --------------------------

//--- Exported functions ---//
void MEM_SetAddress (unsigned int);
unsigned char MEM_GetManufacturer (silicon_t *);
unsigned char MEM_GetSilicon (silicon_t *);
unsigned char MEM_GetProtectedSectors (silicon_t *);


#endif    /* _MEMORY_H_ */

//--- end of file ---//
