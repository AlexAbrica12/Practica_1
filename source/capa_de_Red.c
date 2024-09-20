/*
 * capa_de_Red.c
 *
 *  Created on: 6 sep. 2024
 *      Author: Alexander
 */

#include "capa_de_Red.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/
uint8_t g_macAddr[6] = MAC_ADDRESS;
struct rx_tx_flags flags;

uint8_t *data;
uint8_t buff[300] = {};
char recive[] = {0};

mdio_handle_t mdioHandle = {.ops = &enet_ops};
phy_handle_t phyHandle   = {.phyAddr = RTE_ENET_PHY_ADDRESS, .mdioHandle = &mdioHandle, .ops = &phyksz8081_ops};
_Bool rx_flag = false;


uint32_t ENET0_GetFreq(void)
{
    return CLOCK_GetFreq(kCLOCK_CoreSysClk);
}

void ENET_SignalEvent_t(uint32_t event)
{
    if (event == ARM_ETH_MAC_EVENT_RX_FRAME){
        uint32_t size;
        uint32_t len;

        /* Get the Frame size */
        size = EXAMPLE_ENET.GetRxFrameSize();
        /* Call ENET_ReadFrame when there is a received frame. */
        if (size != 0){
            /* Received valid frame. Deliver the rx buffer with the size equal to length. */
            data = (uint8_t *)malloc(size);
            if (data){
                len = EXAMPLE_ENET.ReadFrame(data, size);
                //memcpy(buff, data, size);
                if (size == len){
                    /* Increase the received frame numbers. */
                    if (flags.g_rxIndex < ENET_EXAMPLE_LOOP_COUNT)
                    {
                    	if(*data == 0xd4)
                    	{
                    		memcpy(buff, data, size);
                    		flags.g_rxIndex++;
                    		rx_flag= true;
                    	}
                    }
                }
                free(data);
            }
        }
    }
    if (event == ARM_ETH_MAC_EVENT_TX_FRAME)
    	flags.g_testTxNum++;
}

/*! @brief Build Frame for transmit. */
void ENET_BuildBroadCastFrame(char* message)
{

}

void ENET_Init_module(void)
{
	flags.g_rxCheckIdx = 0;
	flags.g_rxIndex = 0;
	flags.g_testTxNum = 0;
	flags.g_txCheckIdx = 0;
	flags.txnumber = 0;

	ARM_ETH_LINK_INFO linkInfo;

	mdioHandle.resource.base        = ENET;
	mdioHandle.resource.csrClock_Hz = ENET0_GetFreq();

	PRINTF("\r\nENET example start.\r\n");

    /* Initialize the ENET module. */
    EXAMPLE_ENET.Initialize(ENET_SignalEvent_t);
    EXAMPLE_ENET.PowerControl(ARM_POWER_FULL);
    EXAMPLE_ENET.SetMacAddress((ARM_ETH_MAC_ADDR *)g_macAddr);

    PRINTF("Wait for PHY init...\r\n");
    while (EXAMPLE_ENET_PHY.PowerControl(ARM_POWER_FULL) != ARM_DRIVER_OK)
    {
        PRINTF("PHY Auto-negotiation failed, please check the cable connection and link partner setting.\r\n");
    }

    EXAMPLE_ENET.Control(ARM_ETH_MAC_CONTROL_RX, 1);
    EXAMPLE_ENET.Control(ARM_ETH_MAC_CONTROL_TX, 1);
    PRINTF("Wait for PHY link up...\r\n");
    do
    {
        if (EXAMPLE_ENET_PHY.GetLinkState() == ARM_ETH_LINK_UP)
        {
            linkInfo = EXAMPLE_ENET_PHY.GetLinkInfo();
            EXAMPLE_ENET.Control(ARM_ETH_MAC_CONFIGURE, linkInfo.speed << ARM_ETH_MAC_SPEED_Pos |
                                                            linkInfo.duplex << ARM_ETH_MAC_DUPLEX_Pos |
                                                            ARM_ETH_MAC_ADDRESS_BROADCAST);
            break;
        }
    } while (1);

#if defined(PHY_STABILITY_DELAY_US) && PHY_STABILITY_DELAY_US
    /* Wait a moment for PHY status to be stable. */
    SDK_DelayAtLeastUs(PHY_STABILITY_DELAY_US, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
#endif


}

void SendFrame(char *data, uint32_t length)
{
	uint32_t count = 0, count_messg  = 0, pack_length  = 0;
	uint8_t msg[40] = {0}, mesg_frame[500] = {0};
	unsigned int data_length;

	/* AES data */
	uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
	uint8_t iv[]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	struct AES_ctx ctx;
	size_t test_string_len, padded_len;
	uint8_t  padded_msg[512] = {0};
	/* CRC data */
	CRC_Type *base = CRC0;
	uint32_t checksum32;

	/* Convierte en arreglo el mensaje de entrada */
	pack_length = length;
	data_length = strlen(data);
	memcpy(msg, data, data_length);

	/* Init the AES context structure */
	AES_init_ctx_iv(&ctx, key, iv);
	/* To encrypt an array its lenght must be a multiple of 16 so we add zeros */
	padded_len = data_length + (16 - (data_length%16) );
	memcpy(padded_msg, msg, data_length);
	AES_CBC_encrypt_buffer(&ctx, padded_msg, padded_len);
	PRINTF("Encrypted Message: ");
	/*for(int i=0; i<padded_len; i++) {
		PRINTF("0x%02x,", padded_msg[i]);
	}
	PRINTF("\r\n");*/
	/* CRC32 checksum */
	InitCrc32(base, 0xFFFFFFFFU);
	CRC_WriteData(base, (uint8_t *)&padded_msg[0], padded_len);
	checksum32 = CRC_Get32bitResult(base);
	//PRINTF("CRC-32: 0x%08x\r\n", checksum32);


	/* Build Frame for transmit. */
	for (count = 0; count < 6U; count++){
		mesg_frame[count] = 0xFFU;
	}
	memcpy(&mesg_frame[6], &g_macAddr[0], 6U);
	mesg_frame[12] = (padded_len >> 8) & 0xFFU;	// padded_len <-> data_length
	mesg_frame[13] = padded_len & 0xFFU;			// padded_len <-> data_length
	for(count = 0; count < padded_len; count++){ 	// padded_len <-> data_length
		mesg_frame[count + 14] = padded_msg[count_messg++];
	}
	/* Almacena checksum32 al frame */
	mesg_frame[padded_len + 14] = (checksum32 >> 24) & 0xFFU;
	mesg_frame[padded_len + 15] = (checksum32 >> 16) & 0xFFU;
	mesg_frame[padded_len + 16] = (checksum32 >> 8) & 0xFFU;
	mesg_frame[padded_len + 17] = checksum32 & 0xFFU;


	/* Send a frame when the PHY is link up. */
	if (EXAMPLE_ENET.SendFrame(&mesg_frame[0], pack_length, ARM_ETH_MAC_TX_FRAME_EVENT) == ARM_DRIVER_OK) // padded_len <-> data_length
		SDK_DelayAtLeastUs(1000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
    else
    	PRINTF(" \r\nTransmit frame failed!\r\n");
    // Check the total number of received number.
    if (flags.g_testTxNum && (flags.g_txCheckIdx != flags.g_testTxNum))
    {
    	flags.g_txCheckIdx = flags.g_testTxNum;
        //PRINTF("The %d frame transmitted success!\r\n", g_txCheckIdx);
    }
}

_Bool ReciveFrame()
{
	if(rx_flag)
	{
		/* AES data */
		uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
		uint8_t iv[]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
		struct AES_ctx ctx;
		size_t msg_len, padded_len, buff_len, buff_len_padd;
		uint8_t  recive_msg[512] = {0}, padded_msg[512] = {0}, checksum_result[4] = {0};
		/* CRC data */
		CRC_Type *base = CRC0;
		uint32_t get_checksum32, comp_checksum32;

		rx_flag = false;

		/* Código para paquetes encriptados que son mayores a 16 bits */
		/* TODAVIA NO PROBADO */
		/*buff_len_padd = strlen(buff);
		for (unsigned int count = buff_len_padd; count > 0; count--)
		{
			if(buff[count-1] == 0x00)
				buff_len -= 1;
		}
		// Almacena mensaje encriptado del buffer global //
		for(unsigned int count = 0; count < buff_len; count++){
			padded_msg[count] = buff[count + 14];
		}
		// Obtener checksum del buffer //
		// Como todos los paquetes recibidos son de la misma longitud es 33 (No incluye rellenos 0x00) //
		get_checksum32 = (buff[buff_len-4] << 24) + (buff[buff_len-3] << 16) + (buff[buff_len-2] << 8) + buff[buff_len-1];
		*/

		/* Almacena mensaje encriptado del buffer global */
		for(unsigned int count = 0; count < 16U; count++){ 	// padded_len <-> data_length
			padded_msg[count] = buff[count + 14];
		}

		/* Obtener checksum del buffer */
		/* Como todos los paquetes recibidos son de la misma longitud es 33 (No incluye rellenos 0x00) */
		get_checksum32 = (buff[30] << 24) + (buff[31] << 16) + (buff[32] << 8) + buff[33];

		/* CRC32 checksum */
		InitCrc32(base, 0xFFFFFFFFU);
		CRC_WriteData(base, (uint8_t *)&padded_msg[0], 16U);
		comp_checksum32 = CRC_Get32bitResult(base);

		if(comp_checksum32 != get_checksum32)
		{
			PRINTF("CRC32 is Incorrect\r\n");
		}
		else
		{
			/* Tamaño del mensaje original */
			msg_len = (buff[12] << 8) + buff[13];
			/* Init the AES context structure */
			AES_init_ctx_iv(&ctx, key, iv);
			/* Desencriptar buffer */
			AES_CBC_decrypt_buffer(&ctx, padded_msg, msg_len);

			for(uint32_t mes_count=0; mes_count<msg_len; mes_count++)
			{
				//if(strcmp(buff[mes_count+75],padded_msg[mes_count]) == 0)
				PRINTF("%c", padded_msg[mes_count]);
				//PRINTF("%c", buff[mes_count+75]);
			}
			PRINTF(" \r\n");
		}
		return true;
	}
	else
		return false;
}

/*! @brief Init for CRC-32.
 *  @details Init CRC peripheral module for CRC-32 protocol.
 *          width=32 poly=0x04c11db7 init=0xffffffff refin=true refout=true xorout=0xffffffff check=0xcbf43926
 *          name="CRC-32"
 *          http://reveng.sourceforge.net/crc-catalogue/
 */
static void InitCrc32(CRC_Type *base, uint32_t seed)
{
    crc_config_t config;

    config.polynomial         = 0x04C11DB7U;
    config.seed               = seed;
    config.reflectIn          = true;
    config.reflectOut         = true;
    config.complementChecksum = true;
    config.crcBits            = kCrcBits32;
    config.crcResult          = kCrcFinalChecksum;

    CRC_Init(base, &config);
}
