#ifndef _GUARD_H_ESPUSH_AT_H_
#define _GUARD_H_ESPUSH_AT_H_

#include <osapi.h>
#include <at_custom.h>
#include <user_interface.h>

enum ESPUSH_MSG_TYPE {
	MSG_CLOUD_ATCMD_REQ = 0x14,
	MSG_CLOUD_ATCMD_RSP = 0x15,
};

void ICACHE_FLASH_ATTR at_queryEspushStatus(uint8_t id);
void ICACHE_FLASH_ATTR at_setupEspushDef(uint8_t id, char *pPara);
void ICACHE_FLASH_ATTR at_setupCmdPushMessage(uint8_t id, char* pPara);
void ICACHE_FLASH_ATTR at_execUnPushRegist(uint8_t id);
void ICACHE_FLASH_ATTR at_exec_NetworkCfgTouch(uint8_t id);
void ICACHE_FLASH_ATTR at_query_ADCU(uint8_t id);
void ICACHE_FLASH_ATTR at_queryAllInfo(uint8_t id);
void ICACHE_FLASH_ATTR at_setupInterval(uint8_t id, char *pPara);
void ICACHE_FLASH_ATTR at_exec_UartTrans(uint8_t id);
void ICACHE_FLASH_ATTR at_setupCmdHexUpMsg(uint8_t id, char* pPara);

void ICACHE_FLASH_ATTR at_query_gpio(uint8_t id);
void ICACHE_FLASH_ATTR at_setupGPIOEdgeLow(uint8_t id, char *pPara);
void ICACHE_FLASH_ATTR at_setupGPIOEdgeHigh(uint8_t id, char *pPara);
void ICACHE_FLASH_ATTR at_setupServer(uint8_t id, char *pPara);
void ICACHE_FLASH_ATTR at_execUpgrade(uint8_t id);

void ICACHE_FLASH_ATTR espush_at_init();

#endif

