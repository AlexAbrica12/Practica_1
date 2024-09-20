/*
 * Copyright 2017-2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*  Standard C Included Files */
#include <string.h>
/*  SDK Included Files */
#include "pin_mux.h"
#include "board.h"

#include "stdlib.h"

#include "fsl_device_registers.h"
#include "pin_mux.h"
#include "clock_config.h"


//#include "aes.h"
//#include "fsl_crc.h"
#include "capa_de_Red.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
uint8_t g_macAdd[6]           = MAC_ADDRESS;
uint8_t mg_size[8]	= MESSAGE_LENGTH;

char *message[8] = {"elc", "Alex", "Iteso", "Mexico", "practic", "12041997", "Guadalajara", "Hellow world"};

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */
int main(void)
{
    uint32_t txnumber = 0;
    ARM_ETH_LINK_INFO linkInfo;

    /* Hardware Initialization. */
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    /* Disable SYSMPU. */
    SYSMPU_Enable(SYSMPU, false);

    ENET_Init_module();


    /* Build broadcast for sending. */

    while (1)
    {
        // Check the total number of received number.
        if (flags.g_testTxNum && (flags.g_txCheckIdx != flags.g_testTxNum))
        {
        	flags.g_txCheckIdx = flags.g_testTxNum;
            //PRINTF("The %d frame transmitted success!\r\n", flags.g_txCheckIdx);
        }
        if (flags.g_rxCheckIdx != flags.g_rxIndex)
        {
        	flags.g_rxCheckIdx = flags.g_rxIndex;
            //PRINTF("A total of %d frame(s) has been successfully received!\r\n", flags.g_rxCheckIdx);
        }
        // Get the Frame size
        if (flags.txnumber < ENET_EXAMPLE_LOOP_COUNT) //ENET_EXAMPLE_LOOP_COUNT
        {

        	SendFrame(message[flags.txnumber], mg_size[flags.txnumber]);
        	SDK_DelayAtLeastUs(150000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
        	while(ReciveFrame() != true)
        	{

        	}
        	flags.txnumber++;
        }

    }


    PRINTF(" \r\nEnd Transmit frame!\r\n");

}
