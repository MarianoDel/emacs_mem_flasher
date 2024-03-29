//--------------------------------------------------
// #### MEMORY FLASHER 29LV040   - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### COMM.H #####################################
//--------------------------------------------------
#ifndef _COMM_H_
#define _COMM_H_


//--- Defines For Configuration ---//
//--- Exported types ---//
//--- Exported constants ---//
typedef enum {
	resp_ok = 0,
	resp_continue,
	resp_error,
        resp_timeout

} resp_t;

//--- Exported macro ---//

//--- Exported functions ---//
void UpdateCommunications (void);
unsigned char SerialProcess (void);
unsigned char InterpretarMsg (void);


#endif
//--- end of file ---//
