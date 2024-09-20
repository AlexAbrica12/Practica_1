/*
 * capa_de_Red_cfg.h
 *
 *  Created on: 6 sep. 2024
 *      Author: Alexander
 */

#ifndef CAPA_DE_RED_CFG_H_
#define CAPA_DE_RED_CFG_H_

#include <stdint.h>


struct rx_tx_flags{
	volatile uint32_t g_testTxNum;
	volatile uint32_t g_rxIndex;
	volatile uint32_t g_rxCheckIdx;
	volatile uint32_t g_txCheckIdx;
	volatile uint32_t txnumber;
};

#endif /* CAPA_DE_RED_CFG_H_ */
