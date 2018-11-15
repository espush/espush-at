/*
 * espush exported headers.
 */

#ifndef _GUARD_H_ESPUSH_HEADERS_H_
#define _GUARD_H_ESPUSH_HEADERS_H_

#include <c_types.h>


typedef void(*text_handler_t)(const uint8* data, uint32 inlen);

struct datacb_args_t {
	uint8_t* data;
	uint32_t inLength;
	uint16_t msgType;
	uint8_t* out;
	uint32_t outLength;
};

typedef int(*data_handler_t)(struct datacb_args_t *p);

void ICACHE_FLASH_ATTR espush_init();
void ICACHE_FLASH_ATTR espush_set_auth(uint32_t appid, uint8_t *devkey);
void ICACHE_FLASH_ATTR espush_set_firmware(char firmware[8]);
void ICACHE_FLASH_ATTR espush_set_server_domain(const char* domain, uint16_t port);
void ICACHE_FLASH_ATTR espush_set_server_ipaddr(const char* ipaddr, uint16_t port);

void ICACHE_FLASH_ATTR espush_set_text_cb(text_handler_t t);
void ICACHE_FLASH_ATTR espush_set_data_cb(data_handler_t t);


void ICACHE_FLASH_ATTR espush_upload_data(const char* data, uint16_t length);


enum ConnectStatus {
	ECS_UNCONNECTED = 0,
	ECS_CONNECTING=1,
	ECS_CONNECTED=2,
	ECS_DNS_LOOKUP=3,
	ECS_REGIST_FAILED=4,
	ECS_UNREGISTED=5
};

enum ConnectStatus ICACHE_FLASH_ATTR espush_connect_status();

void ICACHE_FLASH_ATTR espush_set_autoreconnect(int flag);
void ICACHE_FLASH_ATTR espush_connect();
void ICACHE_FLASH_ATTR espush_disconnect();
void ICACHE_FLASH_ATTR espush_debug_output(const char* out);

uint32_t ICACHE_FLASH_ATTR get_current_timestamp();
uint32 ICACHE_FLASH_ATTR ef_calc_crc32(const void *buf, size_t size);

const char* ICACHE_FLASH_ATTR espush_sdk_version(void);

#ifndef _ESPUSH_LOG_H_
#define _ESPUSH_LOG_H_

void ICACHE_FLASH_ATTR espush_disable_log();
void ICACHE_FLASH_ATTR espush_enable_log();
int ICACHE_FLASH_ATTR espush_log_is_enabled();

#endif

#endif

