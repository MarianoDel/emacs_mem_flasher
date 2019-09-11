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

#include "memory.h"
#include "utils.h"
#include "tim.h"
#include "dsp.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>



/* Externals ------------------------------------------------------------------*/
// ------- Externals del Puerto serie  -------
extern volatile unsigned char usart1_have_data;
extern volatile unsigned char usart1_timeout;
extern volatile unsigned short comms_in_binary_timeout;
       
/* Globals --------------------------------------------------------------------*/
char buffMessages [100];
unsigned char comms_in_binary = 0;

//strings de comienzo o lineas intermedias

//--- Manager
const char s_read_all [] = {"read all"};
const char s_read_address [] = {"read addr"};
const char s_get_mid [] = {"read mid"};
const char s_get_sid [] = {"read sid"};
const char s_get_prot [] = {"read prot"};
const char s_write_all [] = {"write all"};
const char s_write_addr [] = {"write addr"};

/* Module Private Functions ---------------------------------------------------*/
void COMM_ReadAllMem (void);

    
/* Module Functions -----------------------------------------------------------*/
void UpdateCommunications (void)
{
    unsigned char bytes_readed = 0;
    
    if (SerialProcess() > 2)	//si tiene algun dato significativo
    {
        InterpretarMsg();
    }

    if (comms_in_binary)
    {
        if ((usart1_have_data) && (!usart1_timeout))
        {
            //leo un buffer terminado en 0, que en binario no sirve, ajusto el tamanio
            bytes_readed = ReadUsart1Buffer((unsigned char *) buffMessages, sizeof(buffMessages));
            if (bytes_readed)
                bytes_readed--;

            usart1_have_data = 0;
            comms_in_binary_timeout = 3000;

            //TODO: algo hago con esto, llamo para grabar o para leer (leer lo hago por otro lado)
        }
    }

    if (!comms_in_binary_timeout)
        comms_in_binary = 0;
}

//Procesa consultas desde la raspberry
//carga el buffer buffMessages
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
    silicon_t mem_data;
    char b_vect [20] = { 0 };

    
    //-- Read Actions
    if (strncmp(pStr, s_read_all, sizeof(s_read_all) - 1) == 0)
    {
        Usart1Send("go to binary\n");
        COMM_ReadAllMem();
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
        resp = MEM_GetSilicon(&mem_data);
        if (resp == resp_ok)
        {
            sprintf(b_vect, "sid: %x\n", mem_data.silicon);
            Usart1Send(b_vect);
        }
    }

    else if (strncmp(pStr, s_get_mid, sizeof(s_get_mid) - 1) == 0)
    {
        resp = MEM_GetManufacturer(&mem_data);
        if (resp == resp_ok)
        {
            sprintf(b_vect, "mid: %x\n", mem_data.manufacturer);
            Usart1Send(b_vect);
        }
    }

    else if (strncmp(pStr, s_get_prot, sizeof(s_get_prot) - 1) == 0)
    {
        resp = MEM_GetProtectedSectors(&mem_data);
        if (resp == resp_ok)
        {
            sprintf(b_vect, "prot: %x\n", mem_data.protected);
            Usart1Send(b_vect);
        }
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

//revisar SIZEOF_TXDATA en Usart1SendUnsigned()
#define SIZEOF_MEM_BUFFER    16
void COMM_ReadAllMem (void)
{    
    unsigned int addr = 0;
    unsigned char data [SIZEOF_MEM_BUFFER] = { 0 };

    unsigned short last_crc = 0;
    
    Wait_ms(100);

    do {
        for (unsigned char i = 0; i < SIZEOF_MEM_BUFFER; i++)
        {
            data[i] = MEM_ReadByte(addr + i);
        }

        Usart1SendUnsigned(data, SIZEOF_MEM_BUFFER);
        last_crc = Compute_CRC16_Simple (data, SIZEOF_MEM_BUFFER, last_crc);
        Wait_ms(20);
        addr += SIZEOF_MEM_BUFFER;

    } while (addr < 0x080000);

    //envio CRC como texto
    Usart1Send("\r\nCRC: ");
    sprintf((char *)data, "0x%04x\r\n", last_crc);
    Usart1Send((char *)data);
    Wait_ms(10);
}
//--- end of file ---//
