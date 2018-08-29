#include <string.h>

#include <osapi.h>
#include <gpio.h>

#include "espush_at.h"
#include "espush.h"
#include "save_conf.h"
#include "utils.h"

#define MAX_UP_MSG_LENGTH  512
#define STANDARD_DEVKEY_LENGTH 32
#define DEFAULT_SERVER "gw.espush.cn"
#define DEFAULT_PORT 10001

static uint8 suffix_flag = 1;

void ICACHE_FLASH_ATTR showbuf(uint8* buf, uint32 len) {
	int i;
	for (i = 0; i != len; ++i) {
		char buf[3];
		os_sprintf(buf, "%02x ", buf[i]);
		at_response(buf);
	}

	at_response("\r\n");
}

void ICACHE_FLASH_ATTR at_text_cb(const uint8* pdata, uint32 len) {
	char buf[16] = { 0 };
	if (suffix_flag) {
		os_sprintf(buf, "\r\n+MSG,%d:", len);
		at_response(buf);
	}

	uint8* pdest = (uint8*) pdata;
	uint8_t tmp = pdata[len];
	pdest[len] = 0;
	at_response(pdata);
	pdest[len] = tmp;

	if (suffix_flag) {
		at_response("\r\n");
	}
}

void ICACHE_FLASH_ATTR at_queryEspushStatus(uint8_t id) {
	switch(espush_connect_status()) {
	case ECS_UNCONNECTED:
		at_response("UNCONNECTED");
		break;
	case ECS_CONNECTING:
		at_response("CONNECTING");
		break;
	case ECS_CONNECTED:
		at_response("CONNECTED");
		break;
	case ECS_DNS_LOOKUP:
		at_response("DNSLOOKING");
		break;
	case ECS_REGIST_FAILED:
		at_response("REGFAILED");
		break;
	case ECS_UNREGISTED:
		at_response("UNREGISTED");
		break;
	default:
		at_response("UNKNOWN");
		break;
	}

	at_response_ok();
}

int ICACHE_FLASH_ATTR is_valid_ip(char* ip) {
	int num;
	int flag = 1;
	int counter = 0;
	char* p = strtok(ip, ".");

	while (p && flag) {
		num = atoi(p);

		if (num >= 0 && num <= 255 && (counter++ < 4)) {
			flag = 1;
			p = strtok(NULL, ".");
		} else {
			flag = 0;
			break;
		}
	}

	return flag && (counter == 4);
}

int ICACHE_FLASH_ATTR fake_at_cmd_req(struct datacb_args_t *p)
{
	uint32 ret;
	uint8_t rsp;

	AT_DBG("cmd [%s], length %d\r\n", p->data, p->inLength);
	at_cmdProcess(p->data);
	p->out[0] = 0;

	return 1;
}

int ICACHE_FLASH_ATTR at_data_cb(struct datacb_args_t *p)
{
	int ret;

	ret = 0;
	switch(p->msgType) {
	case MSG_CLOUD_ATCMD_REQ:
		ret = fake_at_cmd_req(p);
		break;
	default:
		break;
	}

	return ret;
}

void ICACHE_FLASH_ATTR ServerInfoInit(struct FlashServerInfo* info)
{
	flash_info_show(info);

	// 如果已经连接上云端，则先断开
	if(espush_connect_status() == ECS_CONNECTED) {
		espush_disconnect();
	}

	espush_set_auth(info->appid, info->devkey);
	if(info->use_ip) {
		espush_set_server_ipaddr(info->host, info->port);
	} else {
		espush_set_server_domain(info->host, info->port);
	}
	espush_set_text_cb(at_text_cb);
	espush_set_data_cb(at_data_cb);
	espush_set_autoreconnect(1);
	espush_set_firmware("AT");
	espush_connect();
}


/*
 * AT+ESPUSH=123,"19231239daowejqo",<"espush.cn">,<1231>
 */
void ICACHE_FLASH_ATTR at_setupEspushDef(uint8_t id, char *pPara)
{
	int appid, port, err, flag;
	char devkey[32+1];
	char host[32+1];
	pPara ++;

	os_memset(devkey, 0, sizeof(devkey));
	os_memset(host, 0, sizeof(host));

	// get appid
	flag = at_get_next_int_dec(&pPara, &appid, &err);
	if(err && flag==FALSE) {
		AT_DBG("at_get_next_int_dec error.\r\n");
		at_response_error();
		return;
	}
	AT_DBG("get appid %d\r\n", appid);

	if(*pPara++ != ',') {
		AT_DBG("skip , first error.\r\n");
		at_response_error();
		return;
	}

	AT_DBG("prepare copy appkey.\r\n");
	flag = at_data_str_copy(devkey, &pPara, 32);
	if(flag < STANDARD_DEVKEY_LENGTH) {
		AT_DBG("at_data_str_copy appkey error.\r\n");
		at_response_error();
		return;
	}

	port = DEFAULT_PORT;
	os_strcpy(host, DEFAULT_SERVER);
	AT_DBG("prepare read host info.\r\n");
	if(*pPara == ',') {
		pPara ++; //skip ,
		flag = at_data_str_copy(host, &pPara, 32);
		// 无论是IP还是域名，小于三个字符肯定是错了
		if(flag < 3) {
			AT_DBG("data copy host failed.\r\n");
			at_response_error();
			return;
		}
		AT_DBG("host info [%s]\r\n", host);
		host[flag] = 0;

		if(*pPara == ',') {
			pPara ++; //skip ,
			flag = at_get_next_int_dec(&pPara, &port, &err);
			if(err && flag == FALSE) {
				AT_DBG("get port failed.\r\n");
				at_response_error();
				return;
			}
			AT_DBG("port %d\r\n", port);
		}
	} else if(*pPara == '\r') {
		// default host info.
	} else {
		AT_DBG("skip appkey error.\r\n");
		at_response_error();
		return;
	}

	// save.
	AT_DBG("prepare save info to flash.\r\n");
	struct FlashServerInfo info;
	os_memset(&info, 0, sizeof(info));
	info.appid = appid;
	os_strcpy(info.devkey, devkey);
	os_strcpy(info.host, host);
	info.port = port;

	AT_DBG("is_valid ip %s\r\n", info.host);
	// 此处 host 被修改， . 符号被删除
	if(is_valid_ip(host)) {
		info.use_ip = 1;
	} else {
		info.use_ip = 0;
	}

	AT_DBG("crc %d, sector\r\n", info.crc_val);
	if(save_serverinfo_flash(&info)) {
		AT_DBG("save to flash failed.\r\n");
		at_response_error();
		return;
	}
	AT_DBG("crc %d, save succ.\r\n", info.crc_val);

	AT_DBG("host info [%s]\r\n", info.host);
	ServerInfoInit(&info);
	at_response_ok();
}

void ICACHE_FLASH_ATTR at_setupCmdPushMessage(uint8_t id, char* pPara)
{
	++pPara;

	// 最后两个是 \r\n
	espush_upload_data(pPara, os_strlen(pPara) - 2);
	at_response_ok();
}

int ICACHE_FLASH_ATTR strlen_spec(const char* s, char c)
{
	const char* pos = s;
	while(*pos++!=c);
	return --pos - s;
}

void ICACHE_FLASH_ATTR at_setupCmdHexUpMsg(uint8_t id, char* pPara)
{
	++pPara;

	int length = strlen_spec(pPara, '\r');
}

void ICACHE_FLASH_ATTR at_execUnPushRegist(uint8_t id)
{
	espush_disconnect();

	at_response_ok();
}

void ICACHE_FLASH_ATTR at_execUpgrade(uint8_t id)
{
	//
}

void ICACHE_FLASH_ATTR flash_guess(uint32 flashid, char* buf, int out_max_length)
{
	uint32 i1, i2;
	i1 = flashid & 0xFF;
	i2 = (flashid >> 8) & 0xFFFF;

	os_memset(buf, 0, sizeof(out_max_length));
	switch(i1) {
	case 0xC8:
		strcat(buf, "GigaDevice ");
		break;
	case 0xEF:
		strcat(buf, "Winbond ");
		break;
	default:
		strcat(buf, "Unknown ");
		break;
	}

	switch(i2) {
	case 0x1640:
		strcat(buf, "4MB QIO");
		break;
	case 0x1540:
		strcat(buf, "2MB QIO");
		break;
	case 0x1440:
		strcat(buf, "1MB QIO");
		break;
	case 0x1340:
		strcat(buf, "0.5MB QIO");
		break;
	default:
		strcat(buf, "UNKNOWN");
		break;
	}
}

void ICACHE_FLASH_ATTR at_queryAllInfo(uint8_t id)
{
	char out[256];
	char flash_info[32];

	uint8 cpuFreq = system_get_cpu_freq();
	uint8 flashMap = system_get_flash_size_map();
	uint8 bootVer = system_get_boot_version();
	uint8 bootMode = system_get_boot_mode();
	uint32 chipid = system_get_chip_id();
	uint32 userbinAddr = system_get_userbin_addr();
	uint32 flashId = spi_flash_get_id();
	uint32 sysTime = system_get_time();
	uint32 rtcTime = system_get_rtc_time();
	uint32 freeHeap = system_get_free_heap_size();
	flash_guess(flashId, flash_info, sizeof(flash_info));

	os_sprintf(out, "CPU freq: [%d]\r\n"
			"Flash map: [%d]\r\n"
			"Flash id: [%d]\r\n"
			"Flash Guess: [%s]\r\n"
			"Boot version: [%d]\r\n"
			"Boot mode: [%d]\r\n"
			"Chip id: [%d]\r\n"
			"App current: [%d]\r\n"
			"Sys time: [%d]\r\n"
			"Rtc time: [%d]\r\n"
			"Free memory: [%d]\r\n", cpuFreq, flashMap, flashId, flash_info, bootVer,
			bootMode, chipid, userbinAddr, sysTime, rtcTime, freeHeap);

	at_response(out);
	at_response_ok();
}

void ICACHE_FLASH_ATTR at_exec_UartTrans(uint8_t id) {
	//
}

void ICACHE_FLASH_ATTR espush_at_init()
{
	wifi_set_opmode(STATION_MODE);

	struct FlashServerInfo info;

	if(read_serverinfo_flash(&info)) {
		AT_DBG("read flash failed.\r\n");
		return;
	}

	ServerInfoInit(&info);
}


void ICACHE_FLASH_ATTR at_setupInterval(uint8_t id, char *pPara) {
	//
}
