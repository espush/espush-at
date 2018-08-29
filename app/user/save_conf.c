

#include "save_conf.h"
#include "utils.h"
#include <spi_flash.h>


int ICACHE_FLASH_ATTR espush_sector_set()
{
	return 200;
}

int ICACHE_FLASH_ATTR save_serverinfo_flash(struct FlashServerInfo* info)
{
	// ÏÈ ²Á³ý
	SpiFlashOpResult res;
	int sector = espush_sector_set();

	info->crc_val = calc_info_crc(info);
	res = spi_flash_erase_sector(sector);
	if(res != SPI_FLASH_RESULT_OK) {
		AT_DBG("spi flash erase sector %d failed.\r\n", sector);
		return -1;
	}

	res = spi_flash_write(sector * 4096, (uint32*)info, sizeof(struct FlashServerInfo));
	if(res != SPI_FLASH_RESULT_OK) {
		AT_DBG("spi_flash_write sector %d failed.\r\n", sector);
		return -1;
	}

	return 0;
}

int ICACHE_FLASH_ATTR read_serverinfo_flash(struct FlashServerInfo* info)
{
	SpiFlashOpResult res;
	int sector = espush_sector_set();

	res = spi_flash_read(sector * 4096, (uint32*)info, sizeof(struct FlashServerInfo));
	if(res != SPI_FLASH_RESULT_OK) {
		AT_DBG("spi_flash_read sector %d failed.\r\n", sector);
		return -1;
	}

	if(!check_info_crc(info)) {
		AT_DBG("flash crc check failed. %d\r\n", info->crc_val);
		return -2;
	}

	return 0;
}

uint32 ICACHE_FLASH_ATTR calc_info_crc(struct FlashServerInfo* info)
{
	return ef_calc_crc32(info, sizeof(struct FlashServerInfo) - sizeof(uint32));
}

int ICACHE_FLASH_ATTR check_info_crc(struct FlashServerInfo* info)
{
	return calc_info_crc(info) == info->crc_val;
}

/*
	uint32_t appid;
	uint8_t devkey[32+1];

	uint8_t host[32+1];
	uint16_t port;
	uint8_t use_ip;
	// crc
	uint32 crc_val;
 */
void ICACHE_FLASH_ATTR flash_info_show(struct FlashServerInfo* info)
{
	AT_DBG("appid: [%d]\r\n", info->appid);
	AT_DBG("devkey: [%s], [%d]\r\n", info->devkey, os_strlen(info->devkey));
	AT_DBG("host: [%s], [%d]\r\n", info->host, os_strlen(info->host));
	AT_DBG("port: [%d]\r\n", info->port);
	AT_DBG("use ip: [%d]\r\n", info->use_ip);
	AT_DBG("crc32: [%d]\r\n", info->crc_val);
}
