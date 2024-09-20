/*
 * capa_de_Red.h
 *
 *  Created on: 6 sep. 2024
 *      Author: Alexander
 */

#ifndef CAPA_DE_RED_H_
#define CAPA_DE_RED_H_

#include <stdint.h>
#include "capa_de_Red_cfg.h"
#include <string.h>
#include "Driver_ETH_MAC.h"

#include "fsl_debug_console.h"
#include "fsl_enet.h"
#include "fsl_enet_cmsis.h"
#include "fsl_enet_phy_cmsis.h"
#include "fsl_phy.h"

#include "fsl_common.h"
#include "fsl_sysmpu.h"
#include "fsl_phyksz8081.h"
#include "fsl_enet_mdio.h"
#include "RTE_Device.h"

#include "aes.h"
#include "fsl_crc.h"
#include "clock_config.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* ENET base address */
#define EXAMPLE_ENET     Driver_ETH_MAC0
#define EXAMPLE_ENET_PHY Driver_ETH_PHY0
#define ENET_DATA_LENGTH        (1000)
#define ENET_EXAMPLE_LOOP_COUNT (8U)

#ifndef MAC_ADDRESS
#define MAC_ADDRESS {0xd4, 0xbe, 0xd9, 0x45, 0x22, 0x61}
#endif

#ifndef MESSAGE_LENGTH
#define MESSAGE_LENGTH {0x3c, 0x41, 0x46, 0x4b, 0x4f, 0x55, 0x5a, 0x5f}
#endif

#define DATA_LENGTH        (1000)

#define KEY	{ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
#define IV	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

extern struct rx_tx_flags flags;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
uint32_t ENET0_GetFreq(void);
void ENET_SignalEvent_t(uint32_t event);
void ENET_Init_module(void);
void SendFrame(char *data, uint32_t length);
_Bool ReciveFrame();
void ENET_BuildBroadCastFrame(char *message);

static void InitCrc32(CRC_Type *base, uint32_t seed);

#endif /* CAPA_DE_RED_H_ */
