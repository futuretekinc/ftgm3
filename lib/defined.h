#ifndef	DEFINED_H_
#define	DEFINED_H_

#include <cstdint>

#ifdef	MAKE_BODY
#define	DEFAULT_CONST(name, value)			const char* 	DEFAULT_CONST_##name=value
#define	TITLE_NAME(name, value)				const char* 	TITLE_NAME_##name=value
#define	MSG_CMD(name, value)				const char* 	MSG_CMD_##name=value
#define	MSG_EVENT(name, value)				const char* 	MSG_EVENT_##name=value
#define	MSG_REQ(name, value)				const char* 	MSG_REQ_##name=value
#define	MSG_RES(name, value)				const char* 	MSG_RES_##name=value
#define	MSG_STR(name, value)				const char* 	MSG_STR_##name=value
#define	RET_CONST(name, value)				const char* 	RET_CONST_##name=value
#else
#define	DEFAULT_CONST(name, value)			extern	const char* 	DEFAULT_CONST_##name
#define	TITLE_NAME(name, value)				extern	const char* 	TITLE_NAME_##name
#define	MSG_CMD(name, value)				extern	const char* 	MSG_CMD_##name
#define	MSG_EVENT(name, value)				extern	const char* 	MSG_EVENT_##name
#define	MSG_REQ(name, value)				extern	const char* 	MSG_REQ_##name
#define	MSG_RES(name, value)				extern	const char* 	MSG_RES_##name
#define	MSG_STR(name, value)				extern	const char* 	MSG_STR_##name
#define	RET_CONST(name, value)				extern	const char* 	RET_CONST_##name
#endif

#define	ID_LENGTH_MAX		32
#define	NAME_LENGTH_MAX		64
#define	IP_LENGTH_MAX		16

extern	uint64_t	TIME_MILLISECOND;
extern	uint64_t	TIME_SECOND;
extern	uint32_t	TIME_MINUTE;

extern	uint32_t	ACTIVE_OBJECT_LOOP_INTERVAL;

extern	uint32_t	OBJECT_KEEP_ALIVE_INTERVAL_SEC;
extern	uint32_t	OBJECT_KEEP_ALIVE_INTERVAL_SEC_MIN;
extern	uint32_t	OBJECT_KEEP_ALIVE_INTERVAL_SEC_MAX;

extern	uint32_t	ENDPOINT_REPORT_INTERVAL;
extern	uint32_t	ENDPOINT_UPDATE_INTERVAL;
extern	uint32_t	ENDPOINT_VALUE_COUNT_MAX;
extern	double		ENDPOINT_VALUE_SCALE;

extern	int			ENDPOINT_SENSOR_TEMPERATURE_MAX;
extern	int			ENDPOINT_SENSOR_TEMPERATURE_MIN;	
extern	const char*	ENDPOINT_SENSOR_TEMPERATURE_UNIT;

extern	uint32_t	ENDPOINT_SENSOR_HUMIDITY_MAX;
extern	uint32_t	ENDPOINT_SENSOR_HUMIDITY_MIN;
extern	const char*	ENDPOINT_SENSOR_HUMIDITY_UNIT;

DEFAULT_CONST(LOCAL_IP, "127.0.0.1");

DEFAULT_CONST(RCS_SERVER_IP, "127.0.0.1");
#define	DEFAULT_CONST_RCS_SERVER_PORT	 8888

extern	int			SNMP_COMMUNITY_LENGTH_MAX;

#define	DEFAULT_CONST_LOG_FILE_SIZE	 (1024*1024)
DEFAULT_CONST(LOG_FILE_PATH, "/var/log/");

enum	RetValue
{
	RET_VALUE_OK,
	RET_VALUE_ERROR,
	RET_VALUE_INVALID_ARGUMENTS,
	RET_VALUE_INVALID_TYPE,
	RET_VALUE_INVALID_NAME,
};

DEFAULT_CONST(SERVER_LINKER_BROKER,"localhost:9092");

DEFAULT_CONST(DB_FILE, "./ftgm.db");
DEFAULT_CONST(DB_TABLE_NAME_DEVICE, "devices");
DEFAULT_CONST(DB_TABLE_NAME_ENDPOINT, "endpoints");

RET_CONST(OK, "ok");
RET_CONST(FAILED, "failed");

TITLE_NAME(AUTO_START, "auto_start");
TITLE_NAME(BROKER, "broker");
TITLE_NAME(RCS_SERVER, "rcs_server");
TITLE_NAME(CMD, "cmd");
TITLE_NAME(COMMAND, "command");
TITLE_NAME(COMMUNITY, "community");
TITLE_NAME(COUNT, "count");
TITLE_NAME(DATA, "data");
TITLE_NAME(DATA_FILE, "data file");
TITLE_NAME(DATA_MANAGER, "database");
TITLE_NAME(DATE, "date");
TITLE_NAME(DEV_NAME, "dev_name");
TITLE_NAME(DEVICE, "device");
TITLE_NAME(DEVICE_ID, "device_id");
TITLE_NAME(ENABLE, "enable");
TITLE_NAME(ENDPOINT, "endpoint");
TITLE_NAME(ENDPOINT_ID, "ep_id");
TITLE_NAME(ENDPOINT_REPORT_INTERVAL,"endpoint_report_interval");
TITLE_NAME(ERROR, "error");
TITLE_NAME(ID, "id");
TITLE_NAME(IP, "ip");
TITLE_NAME(KEEP_ALIVE_INTERVAL, "keep_alive_interval");
TITLE_NAME(LOOP_INTERVAL, "loop_interval");
TITLE_NAME(MODULE, "module");
TITLE_NAME(MSG_ID, "msg_id");
TITLE_NAME(NAME, "name");
TITLE_NAME(OBJECT_MANAGER, "object_manager");
TITLE_NAME(PATH, "path");
TITLE_NAME(PORT, "port");
TITLE_NAME(RESULT,  "result");
TITLE_NAME(SCALE, "scale");
TITLE_NAME(SECTION, "section");
TITLE_NAME(SENSOR_ID, "sensor_id");
TITLE_NAME(SERVER_LINKER, "server_linker");
TITLE_NAME(SIZE, "size");
TITLE_NAME(TCP_SERVER, "tcp_server");
TITLE_NAME(TIME, "time");
TITLE_NAME(TIMEOUT, "timeout");
TITLE_NAME(TRACE, "trace");
TITLE_NAME(TYPE, "type");
TITLE_NAME(UNIT, "unit");
TITLE_NAME(UPDATE_INTERVAL, "update_interval");
TITLE_NAME(VALUE, "value");
TITLE_NAME(VALUE_MAX, "value_max");
TITLE_NAME(VALUE_MIN, "value_min");

DEFAULT_CONST(MSG_VERSION, "v1");
#define	DEFAULT_CONST_MSG_PARTITION	0
//#define	DEFAULT_CONST_MSG_PARTITION RdKafka::Topic::PARTITION_UA
MSG_STR(KEEP_ALIVE,"keep_alive");
MSG_STR(ENDPOINT_REPORT, "endpoint_report");
#endif

