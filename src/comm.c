//--------------------------------------------------
// #### MEMORY FLASHER 29LV040   - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### COMM.C #####################################
//--------------------------------------------------

/* Includes ------------------------------------------------------------------*/
#include "comm.h"
#include "uart.h"
#include "hard.h"

#include "utils.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>



/* Externals ------------------------------------------------------------------*/
// ------- Externals del Puerto serie  -------
extern unsigned char usart1_have_data;

/* Globals --------------------------------------------------------------------*/
char buffMessages [100];

//strings de comienzo o lineas intermedias

//--- Manager
const char s_read_all [] = {"read all"};
const char s_read_address [] = {"read addr"};
const char s_get_mid [] = {"read mid"};
const char s_get_sid [] = {"read sid"};
const char s_get_prot [] = {"read prot"};
const char s_write_all [] = {"write all"};
const char s_write_addr [] = {"write addr"};



/* Module Functions -----------------------------------------------------------*/
void UpdateCommunications (void)
{
    if (SerialProcess() > 2)	//si tiene algun dato significativo
    {
        InterpretarMsg();
    }
}

//Procesa consultas desde la raspberry
//carga el buffer buffMessages y avisa con el flag msg_ready
unsigned char SerialProcess (void)
{
    unsigned char bytes_readed = 0;

    if (usart1_have_data)
    {
        usart1_have_data = 0;
        bytes_readed = ReadUsart1Buffer((unsigned char *) buffMessages, sizeof(buffMessages));
    }
    return bytes_readed;
}

resp_t InterpretarMsg (void)
{
    resp_t resp = resp_error;
    char * pStr = buffMessages;
    unsigned int address = 0;
    unsigned int bytes_to_read = 0;
    char b_vect [10] = { 0 };

    
    //-- Read Actions
    if (strncmp(pStr, s_read_all, sizeof(s_read_all) - 1) == 0)
    {
        resp = resp_ok;
    }
    
    else if (strncmp(pStr, s_read_address, sizeof(s_read_address) - 1) == 0)
    {
        pStr += sizeof(s_read_address);		//normalizo al payload, hay un espacio

        memset(b_vect, 0, sizeof(b_vect));
        
        //aca llega el address que quieren leer
        for (unsigned char i = 0; i < sizeof(b_vect); i++)
        {
            if (*(pStr + i) != ' ')
                b_vect[i] = *(pStr + i);
            else
            {
                pStr += i + 1;    //hay un espacio
                i = sizeof(b_vect);
            }
        }
        address = atoi(b_vect);
        // sprintf(b_vect, "a: %x ", address);
        // Usart1Send(b_vect);
        
        memset(b_vect, 0, sizeof(b_vect));

        //aca llega la cantidad de bytes a leer
        for (unsigned char i = 0; i < sizeof(b_vect); i++)
        {
            if (*(pStr + i) != ' ')
                b_vect[i] = *(pStr + i);
            else
            {
                pStr += i + 1;    //hay un espacio
                i = sizeof(b_vect);
            }
        }
        bytes_to_read = atoi(b_vect);
        // sprintf(b_vect, "b: %x\n", bytes_to_read);
        // Usart1Send(b_vect);
        
        if ((address + bytes_to_read) <= 0x0080000)
            resp = resp_ok;
    }

    //-- Silicon Actions
    else if (strncmp(pStr, s_get_sid, sizeof(s_get_sid) - 1) == 0)
    {
        Usart1Send("SID\n");
        resp = resp_ok;
    }

    else if (strncmp(pStr, s_get_mid, sizeof(s_get_mid) - 1) == 0)
    {
        Usart1Send("MID\n");        
        resp = resp_ok;
    }

    else if (strncmp(pStr, s_get_prot, sizeof(s_get_prot) - 1) == 0)
    {
        Usart1Send("Protected Sectors\n");
        resp = resp_ok;
    }

    //-- Write Actions
    else if (strncmp(pStr, s_write_all, sizeof(s_write_all) - 1) == 0)
    {
        Usart1Send("Write all\n");
        resp = resp_ok;
    }

    else if (strncmp(pStr, s_write_addr, sizeof(s_write_addr) - 1) == 0)
    {
        pStr += sizeof(s_write_addr);		//normalizo al payload, hay un espacio

        memset(b_vect, 0, sizeof(b_vect));

        //aca llega el address que quieren escribir
        for (unsigned char i = 0; i < sizeof(b_vect); i++)
        {
            if (*(pStr + i) != ' ')
                b_vect[i] = *(pStr + i);
            else
            {
                pStr += i + 1;    //hay un espacio
                i = sizeof(b_vect);
            }
        }
        address = atoi(b_vect);
        // sprintf(b_vect, "a: %x ", address);
        // Usart1Send(b_vect);
        
        memset(b_vect, 0, sizeof(b_vect));

        //aca llega la cantidad de bytes a escribir
        for (unsigned char i = 0; i < sizeof(b_vect); i++)
        {
            if (*(pStr + i) != ' ')
                b_vect[i] = *(pStr + i);
            else
            {
                pStr += i + 1;    //hay un espacio
                i = sizeof(b_vect);
            }
        }
        bytes_to_read = atoi(b_vect);
        // sprintf(b_vect, "b: %x\n", bytes_to_read);
        // Usart1Send(b_vect);
        
        if ((address + bytes_to_read) <= 0x0080000)
            resp = resp_ok;
        
        resp = resp_ok;
    }
        
    //-- Ninguno de los anteriores
    else
        resp = resp_error;

    // Answers for individuals
    if (resp == resp_ok)
        Usart1Send("OK\n");
    else
        Usart1Send("NOK\n");

    return resp;
}

//--- end of file ---//
