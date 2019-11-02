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
extern volatile unsigned short comms_send_timeout;
       
/* Globals --------------------------------------------------------------------*/
char buffMessages [100];
unsigned char comms_in_binary = 0;
unsigned short last_crc = 0;


//--- Manager
const char s_mem_reset [] = {"mem reset"};
const char s_read_all [] = {"read all"};
const char s_read_address [] = {"read addr"};
const char s_read_crc [] = {"read crc"};
const char s_get_mid [] = {"read mid"};
const char s_get_sid [] = {"read sid"};
const char s_get_prot [] = {"read prot"};
const char s_blank_check [] = {"blank check"};
const char s_write_all [] = {"write all"};
const char s_write_addr [] = {"write addr"};
const char s_erase_all [] = {"erase all"};
const char s_last_crc [] = {"last crc"};

/* Module Private Types -------------------------------------------------------*/
typedef struct {
    unsigned int address;
    unsigned char data;
} mem_data_t;

/* Module Private Functions ---------------------------------------------------*/
resp_t COMM_ReadAllMem (void);
resp_t COMM_WriteAllMem (void);
void COMM_ReadAddress (unsigned int, unsigned int);
unsigned short COMM_GetCRC (unsigned short);
resp_t COMM_BlankCheck (mem_data_t *);
resp_t COMM_EraseAllMem (void);

/* Module Functions -----------------------------------------------------------*/
void UpdateCommunications (void)
{
    // unsigned char bytes_readed = 0;
    
    if (SerialProcess() > 2)	//si tiene algun dato significativo
    {
#ifdef USE_LED_IN_USART_ANSWERS
        LED_ON;
#endif
        InterpretarMsg();
#ifdef USE_LED_IN_USART_ANSWERS        
        LED_OFF;
#endif
    }

    // if (comms_in_binary)
    // {
    //     if ((usart1_have_data) && (!usart1_timeout))
    //     {
    //         //leo un buffer terminado en 0, que en binario no sirve, ajusto el tamanio
    //         bytes_readed = ReadUsart1Buffer((unsigned char *) buffMessages, sizeof(buffMessages));
    //         if (bytes_readed)
    //             bytes_readed--;

    //         usart1_have_data = 0;
    //         comms_in_binary_timeout = 3000;

    //         //TODO: algo hago con esto, llamo para grabar o para leer (leer lo hago por otro lado)
    //     }
    // }

    // if (!comms_in_binary_timeout)
    //     comms_in_binary = 0;
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
        resp = COMM_ReadAllMem();
        if (resp == resp_timeout)
            Usart1Send("Timeout\n");

    }

    else if (strncmp(pStr, s_read_crc, sizeof(s_read_crc) - 1) == 0)
    {
        MEM_Reset();
        Wait_ms(1);
        
        last_crc = 0;
        last_crc = COMM_GetCRC(last_crc);
        Usart1Send("CRC: ");
        sprintf((char *)b_vect, "0x%04x\n", last_crc);
        Usart1Send((char *)b_vect);
        resp = resp_ok;
    }

    else if (strncmp(pStr, s_blank_check, sizeof(s_blank_check) - 1) == 0)
    {
        mem_data_t mem_data;
        
        MEM_Reset();
        Wait_ms(1);
        
        resp = COMM_BlankCheck(&mem_data);
        
        if (resp == resp_ok)
            Usart1Send("Blank Check ok\n");
        else
        {
            Usart1Send("Blank Check Fail! in address: ");
            // sprintf(b_vect, "%d data: %02x\n", mem_data.address, mem_data.data);
            sprintf(b_vect, "%d data: %d\n", mem_data.address, mem_data.data);
            Usart1Send(b_vect);
        }
        resp = resp_ok;
    }

    else if (strncmp(pStr, s_mem_reset, sizeof(s_mem_reset) - 1) == 0)
    {
        MEM_Reset();
        Wait_ms(1);
        
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
        {
            COMM_ReadAddress(address, bytes_to_read);
            resp = resp_ok;
        }
    }

    else if (strncmp(pStr, s_last_crc, sizeof(s_last_crc) - 1) == 0)
    {
        //envio CRC como texto
        Usart1Send("CRC: ");
        sprintf((char *)b_vect, "0x%04x\n", last_crc);
        Usart1Send((char *)b_vect);
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
        Usart1Send("go to binary\n");
        resp = COMM_WriteAllMem();
        if (resp == resp_timeout)
            Usart1Send("Timeout\n");
        
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

        //aca llega el valor a escribir
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
        // sprintf(b_vect, "a: %x ", bytes_to_read);
        // Usart1Send(b_vect);

        if ((bytes_to_read >= 0) &&
            (bytes_to_read <= 255))
        {
            if ((address) <= 0x0080000)
            {
                MEM_WriteByte(address, (unsigned char) bytes_to_read);
                resp = resp_ok;
            }
        }
    }

    else if (strncmp(pStr, s_erase_all, sizeof(s_erase_all) - 1) == 0)
    {
        MEM_Reset();
        Wait_ms(1);

        resp = COMM_EraseAllMem();
        if (resp == resp_ok)
            Usart1Send("Erased\n");

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



 //////////////////////////////////////////////////////////
 // Funciones que usan interaccion entre Binario y Texto //
 //////////////////////////////////////////////////////////
unsigned short COMM_SendBinary (unsigned int, unsigned short, unsigned short);
void COMM_UpdateCommsInBinaryTxRx (unsigned char *);

typedef enum {
    SEND_STATE_INIT,
    SEND_STATE_WAITING_NEXT,
    SEND_STATE_SENDING,
    SEND_STATE_CALC_NEXT

} send_state_t;

#define NEXT 0x01
#define STOP 0x02
#define ENDED 0x04
#define SIZEOF_MEM_BUFFER    16
// #define CHUNK_SIZE    1024
#define CHUNK_SIZE    SIZEOF_MEM_BUFFER
#define TT_BINARY    10000
resp_t COMM_ReadAllMem (void)
{
    resp_t resp = resp_continue;
    unsigned int addr = 0;
    unsigned char comms_send_answer = 0;
    unsigned short to_send = 0;
    char next_s [20] = { 0 };
    send_state_t comms_send_state = SEND_STATE_INIT;

    //wait for send_next or timeout
    while (resp == resp_continue)
    {
        switch (comms_send_state)
        {
        case SEND_STATE_INIT:
            last_crc = 0;
            addr = 0;
            comms_send_state = SEND_STATE_CALC_NEXT;
            break;

        case SEND_STATE_WAITING_NEXT:
            if (comms_send_answer & NEXT)
                comms_send_state = SEND_STATE_SENDING;

            if (!comms_send_timeout)
                resp = resp_timeout;

            break;

        case SEND_STATE_SENDING:
            last_crc = COMM_SendBinary (addr, CHUNK_SIZE, last_crc);
            addr += CHUNK_SIZE;
            comms_send_state = SEND_STATE_CALC_NEXT;
            break;

        case SEND_STATE_CALC_NEXT:
            if ((addr + CHUNK_SIZE) <= (SECTORS_LENGHT * SECTORS_QTTY))
                to_send = CHUNK_SIZE;
            else
                to_send = 0;    //TODO: revisar esto para memorias de otros valores

            if (to_send)
            {
                sprintf(next_s, "next %d\n", to_send);
                Usart1Send(next_s);
                comms_send_timeout = TT_BINARY;
                comms_send_state = SEND_STATE_WAITING_NEXT;
            }
            else
            {
                sprintf(next_s, "CRC: 0x%04x\n", last_crc);
                Usart1Send(next_s);
                // Usart1Send("end\n");
                resp = resp_ok;
            }
            break;

        default:
            comms_send_state = SEND_STATE_INIT;
            break;
        }

        comms_send_answer = 0;
        COMM_UpdateCommsInBinaryTxRx(&comms_send_answer);

        if (comms_send_answer & STOP)
            resp = resp_error;

    }    //end while resp == resp_continue

    return resp;
}


void COMM_UpdateCommsInBinaryTxRx (unsigned char * answer)
{
    if (SerialProcess() > 2)	//si tiene algun dato significativo
    {
        //espero solo un next ended o un stop
        char * pStr = buffMessages;        
        if (strncmp(pStr, "next", sizeof("next") - 1) == 0)
        {
            *answer |= NEXT;
        }

        else if (strncmp(pStr, "stop", sizeof("stop") - 1) == 0)
        {
            *answer |= STOP;
        }

        else if (strncmp(pStr, "ended", sizeof("ended") - 1) == 0)
        {
            *answer |= ENDED;
        }
        
    }
}


unsigned short COMM_SendBinary (unsigned int addr, unsigned short qtty, unsigned short crc)
{
    unsigned char data [SIZEOF_MEM_BUFFER] = { 0 };
    unsigned int orig_addr = 0;

    orig_addr = addr;

    while ((orig_addr + qtty) > addr)
    {
        for (unsigned char i = 0; i < SIZEOF_MEM_BUFFER; i++)
        {
#if defined MEMORY_TEST_SEND_KNOW_PATTERN
            data[i] = i;
#elif defined MEMORY_TEST_SEND_KNOW_PATTERN_0X55
            data[i] = 0x55;
#elif defined MEMORY_TEST_SEND_KNOW_PATTERN_0XAA
            data[i] = 0xAA;
#else
            data[i] = MEM_ReadByte(addr + i);
#endif
        }

        Usart1SendUnsigned(data, SIZEOF_MEM_BUFFER);
        crc = Compute_CRC16_Simple (data, SIZEOF_MEM_BUFFER, crc);
        addr += SIZEOF_MEM_BUFFER;

        Wait_ms(10);    //1.38ms to send
    }

    return crc;
}


unsigned short COMM_GetCRC (unsigned short crc)
{
    unsigned char data [SIZEOF_MEM_BUFFER] = { 0 };
    unsigned int addr = 0;
    
    while (addr < (SECTORS_LENGHT * SECTORS_QTTY))
    {
        for (unsigned char i = 0; i < SIZEOF_MEM_BUFFER; i++)
            data[i] = MEM_ReadByte(addr + i);

        crc = Compute_CRC16_Simple (data, SIZEOF_MEM_BUFFER, crc);
        addr += SIZEOF_MEM_BUFFER;
    }

    return crc;
}


resp_t COMM_BlankCheck (mem_data_t * mem)
{
    resp_t resp = resp_ok;
    unsigned char data [SIZEOF_MEM_BUFFER] = { 0 };
    unsigned int addr = 0;
    
    while (addr < (SECTORS_LENGHT * SECTORS_QTTY))
    {
        for (unsigned char i = 0; i < SIZEOF_MEM_BUFFER; i++)
        {
            data[i] = MEM_ReadByte(addr + i);
            if (data[i] != 0xFF)
            {
                mem->address = addr + i;
                mem->data = data[i];

                i = SIZEOF_MEM_BUFFER;
                addr = SECTORS_LENGHT * SECTORS_QTTY;
                resp = resp_error;
            }
        }
        addr += SIZEOF_MEM_BUFFER;
    }

    return resp;
}


resp_t COMM_EraseAllMem (void)
{
    resp_t resp = resp_ok;

    for (unsigned int i = SA0_ADDR;
         i < (SA7_ADDR + SECTORS_LENGHT);
         i += SECTORS_LENGHT)
    {
        MEM_SectorErase(i);
        Wait_ms(10);
    }
    
    return resp;
}


typedef enum {
    RECEIV_STATE_INIT,
    RECEIV_STATE_WAITING_NEXT,
    RECEIV_STATE_GETTING,
    RECEIV_STATE_SAVING

} receiv_state_t;

#define comms_receiv_timeout comms_send_timeout
resp_t COMM_WriteAllMem (void)
{
    resp_t resp = resp_continue;
    unsigned int addr = 0;
    unsigned int getted = 0;
    unsigned char comms_receiv_answer = 0;
    receiv_state_t comms_receiv_state = RECEIV_STATE_INIT;
    char s_ended [40] = { 0 };
    unsigned char data [SIZEOF_MEM_BUFFER] = { 0 };

    //wait for receiv_next or timeout
    while (resp == resp_continue)
    {
        switch (comms_receiv_state)
        {
        case RECEIV_STATE_INIT:
            addr = 0;
            last_crc = 0;
            comms_receiv_timeout = TT_BINARY;
            comms_receiv_state = RECEIV_STATE_WAITING_NEXT;
            break;

        case RECEIV_STATE_WAITING_NEXT:
            
            comms_receiv_answer = 0;
            COMM_UpdateCommsInBinaryTxRx(&comms_receiv_answer);

            if (comms_receiv_answer & NEXT)
            {
                Usart1Send("n\n");
                Usart1ToBinary(SIZEOF_MEM_BUFFER);
                comms_receiv_timeout = TT_BINARY;
                comms_receiv_state = RECEIV_STATE_GETTING;
            }

            if (comms_receiv_answer & ENDED)
                resp = resp_ok;

            if (!comms_receiv_timeout)
                resp = resp_timeout;

            break;

        case RECEIV_STATE_GETTING:
            if (usart1_have_data)
            {
                //tengo los 16 bytes
                usart1_have_data = 0;
                unsigned char bytes_readed = 0;
                bytes_readed = ReadUsart1Buffer((unsigned char *) data, SIZEOF_MEM_BUFFER);
                if (bytes_readed != SIZEOF_MEM_BUFFER)
                    resp = resp_error;
                else
                {
                    getted += bytes_readed;
                    last_crc = Compute_CRC16_Simple (data, SIZEOF_MEM_BUFFER, last_crc);
                    comms_receiv_state = RECEIV_STATE_SAVING;
                }
            }

            if (!comms_receiv_timeout)
                resp = resp_timeout;

            break;

        case RECEIV_STATE_SAVING:
            for (unsigned char i = 0; i < SIZEOF_MEM_BUFFER; i++)
                MEM_WriteByte(addr + i, data[i]);
            
            addr += SIZEOF_MEM_BUFFER;
            Usart1ToText();
            Usart1Send(".\n");
            comms_receiv_timeout = TT_BINARY;
            comms_receiv_state = RECEIV_STATE_WAITING_NEXT;
            break;
            
        default:
            comms_receiv_state = RECEIV_STATE_INIT;
            break;
        }

    }    //end while resp == resp_continue

    Usart1ToText();
    sprintf(s_ended, "Rx: %d Svd: %d CRC: 0x%04x\n", getted, addr, last_crc);
    Usart1Send(s_ended);
    return resp;
}


void COMM_ReadAddress (unsigned int addr, unsigned int bytes)
{
    unsigned char data [SIZEOF_MEM_BUFFER] = { 0 };
    char string_data [100] = { 0 };
    unsigned int orig_addr = addr;
    
    Wait_ms(300);
    last_crc = 0;

    do {
        for (unsigned char i = 0; i < SIZEOF_MEM_BUFFER; i++)
        {
            data[i] = MEM_ReadByte(addr + i);
        }

        sprintf(string_data, "addr: %d data: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
                addr,
                data[0],
                data[1],
                data[2],
                data[3],
                data[4],
                data[5],
                data[6],
                data[7],
                data[8],
                data[9],
                data[10],
                data[11],
                data[12],
                data[13],
                data[14],
                data[15]);
                
        Usart1Send(string_data);
        last_crc = Compute_CRC16_Simple (data, SIZEOF_MEM_BUFFER, last_crc);
        addr += SIZEOF_MEM_BUFFER;
        Wait_ms(10);

    } while (addr < (orig_addr + bytes));

    sprintf(string_data, "CRC: 0x%x\n", last_crc);
    Usart1Send(string_data);
}

//--- end of file ---//
