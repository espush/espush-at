#ifndef _GUARD_H_SAVE_CONF_H_
#define _GUARD_H_SAVE_CONF_H_

#include <os_type.h>
#include <c_types.h>


struct FlashServerInfo {
	uint32_t appid;
	uint8_t devkey[32+1];

	uint8_t host[32+1];
	uint16_t port;
	uint8_t use_ip;
	// crc
	uint32 crc_val;
};

void ICACHE_FLASH_ATTR flash_info_show(struct FlashServerInfo* info);

uint32 ICACHE_FLASH_ATTR calc_info_crc(struct FlashServerInfo* info);
int ICACHE_FLASH_ATTR check_info_crc(struct FlashServerInfo* info);

int ICACHE_FLASH_ATTR espush_sector_set();
int ICACHE_FLASH_ATTR save_serverinfo_flash(struct FlashServerInfo* info);
int ICACHE_FLASH_ATTR read_serverinfo_flash(struct FlashServerInfo* info);

#endif

