#ifndef	DEFINED_H_
#define	DEFINED_H_

#include <cstdint>

#ifdef	MAKE_BODY
#define	DEFAULT_CONST(name, value)	const char* DEFAULT_CONST_##name=value
#define	TITLE_NAME(name, value)		const char* TITLE_NAME_##name=value
#define	MSG_CMD(name, value)		const char* MSG_CMD_##name=value
#define	RET_CONST(name, value)		const char* RET_CONST_##name=value
#else
#define	DEFAULT_CONST(name, value)	extern	const char* DEFAULT_CONST_##name
#define	TITLE_NAME(name, value)		extern	const char* TITLE_NAME_##name
#define	MSG_CMD(name, value)		extern	const char* MSG_CMD_##name
#define	RET_CONST(name, value)		extern	const char* RET_CONST_##name
#endif

#define	ID_LENGTH_MAX		32
#define	NAME_LENGTH_MAX		64
#define	IP_LENGTH_MAX		16

extern	uint64_t	TIME_MILLISECOND;
extern	uint64_t	TIME_SECOND;
extern	uint32_t	TIME_MINUTE;

extern	uint32_t	ACTIVE_OBJECT_LOOP_INTERVAL;

extern	uint32_t	OBJECT_LIVE_CHECK_INTERVAL_SEC;
extern	uint32_t	OBJECT_LIVE_CHECK_INTERVAL_SEC_MIN;
extern	uint32_t	OBJECT_LIVE_CHECK_INTERVAL_SEC_MAX;

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

extern	const char*	DEFAULT_LOCAL_IP;
extern	const char*	DEFAULT_SERVER_IP;	
extern	uint16_t	DEFAULT_SERVER_PORT;

extern	int			SNMP_COMMUNITY_LENGTH_MAX;

extern	const char*	LOG_FILE_PATH;
extern	uint32_t	LOG_FILE_SIZE;

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

RET_CONST(OK,	 "ok");
RET_CONST(FAILED,	"failed");

TITLE_NAME(AUTO_START, "auto_start");
TITLE_NAME(BROKER, "broker");
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
TITLE_NAME(ENDPOINTS, "endpoints");
TITLE_NAME(ERROR, "error");
TITLE_NAME(ID, "id");
TITLE_NAME(IP, "ip");
TITLE_NAME(LIVE_CHECK_INTERVAL, "live_check_interval");
TITLE_NAME(LOOP_INTERVAL, "loop_interval");
TITLE_NAME(MODULE, "module");
TITLE_NAME(NAME, "name");
TITLE_NAME(RESULT,  "result");
TITLE_NAME(SCALE, "scale");
TITLE_NAME(SECTION, "section");
TITLE_NAME(SENSOR_ID, "sensor_id");
TITLE_NAME(SERVER_LINKER, "server_linker");
TITLE_NAME(TIME, "time");
TITLE_NAME(TIMEOUT, "timeout");
TITLE_NAME(TRACE, "trace");
TITLE_NAME(TYPE, "type");
TITLE_NAME(UNIT, "unit");
TITLE_NAME(UPDATE_INTERVAL, "update_interval");
TITLE_NAME(VALUE, "value");
TITLE_NAME(VALUE_MAX, "value_max");
TITLE_NAME(VALUE_MIN, "value_min");

MSG_CMD(KEEP_ALIVE,"keep_alive");
MSG_CMD(ENDPOINT_REPORT, "endpoint_report");
#endif

