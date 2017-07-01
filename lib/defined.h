#ifndef	DEFINED_H_
#define	DEFINED_H_

//#include <cstdint>
#include <stdint.h>

#define	PROPERTY_VALUE_STRING_ONLY			1

#ifdef	MAKE_BODY
#define	DEFAULT_CONST(name, value)			const char* 	DEFAULT_CONST_##name=value
#define	TITLE_NAME(name, value)				const char* 	TITLE_NAME_##name=value
#define	MSG_CMD(name, value)				const char* 	MSG_CMD_##name=value
#define	MSG_EVENT(name, value)				const char* 	MSG_EVENT_##name=value
#define	MSG_REQ(name, value)				const char* 	MSG_REQ_##name=value
#define	MSG_RES(name, value)				const char* 	MSG_RES_##name=value
#define	MSG_STR(name, value)				const char* 	MSG_STR_##name=value
#define	MSG_TYPE_RCS(name, value)			const char* 	MSG_TYPE_RCS_##name=value
#define	RET_CONST(name, value)				const char* 	RET_CONST_##name=value
#else
#define	DEFAULT_CONST(name, value)			extern	const char* 	DEFAULT_CONST_##name
#define	TITLE_NAME(name, value)				extern	const char* 	TITLE_NAME_##name
#define	MSG_CMD(name, value)				extern	const char* 	MSG_CMD_##name
#define	MSG_EVENT(name, value)				extern	const char* 	MSG_EVENT_##name
#define	MSG_REQ(name, value)				extern	const char* 	MSG_REQ_##name
#define	MSG_RES(name, value)				extern	const char* 	MSG_RES_##name
#define	MSG_STR(name, value)				extern	const char* 	MSG_STR_##name
#define	MSG_TYPE_RCS(name, value)			extern	const char* 	MSG_TYPE_RCS_##name
#define	RET_CONST(name, value)				extern	const char* 	RET_CONST_##name
#endif

#define	ID_LENGTH_MAX		32
#define	NAME_LENGTH_MAX		64
#define	CLASS_LENGTH_MAX	32
#define	IP_LENGTH_MAX		16

extern	uint64_t	TIME_MILLISECOND;
extern	uint64_t	TIME_SECOND;
extern	uint32_t	TIME_MINUTE;

extern	uint32_t	ACTIVE_OBJECT_LOOP_INTERVAL;

extern	uint32_t	OBJECT_KEEP_ALIVE_INTERVAL_SEC;
extern	uint32_t	OBJECT_KEEP_ALIVE_INTERVAL_SEC_MIN;
extern	uint32_t	OBJECT_KEEP_ALIVE_INTERVAL_SEC_MAX;

extern	const char* OBJECT_TYPE_NODE;

extern	const char* OBJECT_TYPE_GATEWAY;
extern	const char*	OBJECT_TYPE_GW_GEN;

extern	const char* OBJECT_TYPE_DEVICE;
extern	const char*	OBJECT_TYPE_DEV_SNMP;
extern	const char*	OBJECT_TYPE_DEV_FTE;
extern	const char*	OBJECT_TYPE_DEV_SIM;

extern	const char*	OBJECT_TYPE_DEV_MODBUS;
extern	const char*	OBJECT_TYPE_DEV_GTC_520A;

extern	const char*	OBJECT_TYPE_DEV_MODBUS_TCP;
extern	const char*	OBJECT_TYPE_DEV_ADAM_6051;

extern	const char* OBJECT_TYPE_ENDPOINT;
extern	const char*	OBJECT_TYPE_EP_S_DISCRETE;
extern	const char*	OBJECT_TYPE_EP_S_LINEAR;
extern	const char*	OBJECT_TYPE_EP_S_TEMPERATURE;
extern	const char*	OBJECT_TYPE_EP_S_HUMIDITY;
extern	const char*	OBJECT_TYPE_EP_S_VOLTAGE;
extern	const char*	OBJECT_TYPE_EP_S_CURRENT;
extern	const char*	OBJECT_TYPE_EP_S_DI;
extern	const char*	OBJECT_TYPE_EP_S_PRESSURE;
extern	const char*	OBJECT_TYPE_EP_S_WIND_SPEED;
extern	const char*	OBJECT_TYPE_EP_S_SOIL_MOISTURE;
extern	const char*	OBJECT_TYPE_EP_S_RAINFALL;
extern	const char*	OBJECT_TYPE_EP_S_GAS;
extern	const char* OBJECT_TYPE_EP_S_SOIL_ACIDITY;

extern	const char*	OBJECT_TYPE_EP_A_LINEAR;
extern	const char*	OBJECT_TYPE_EP_A_DISCRETE;
extern	const char*	OBJECT_TYPE_EP_A_DO;

extern	uint32_t	ENDPOINT_REPORT_INTERVAL;
extern	uint32_t	ENDPOINT_UPDATE_INTERVAL;
extern	uint32_t	ENDPOINT_VALUE_COUNT_MAX;
extern	double		ENDPOINT_VALUE_SCALE;

extern	double		ENDPOINT_SENSOR_TEMPERATURE_MAX;
extern	double		ENDPOINT_SENSOR_TEMPERATURE_MIN;	
extern	const char*	ENDPOINT_SENSOR_TEMPERATURE_UNIT;

extern	double		ENDPOINT_SENSOR_HUMIDITY_MAX;
extern	double		ENDPOINT_SENSOR_HUMIDITY_MIN;
extern	const char*	ENDPOINT_SENSOR_HUMIDITY_UNIT;

extern	double		ENDPOINT_SENSOR_VOLTAGE_MAX;
extern 	double		ENDPOINT_SENSOR_VOLTAGE_MIN;
extern 	const char*	ENDPOINT_SENSOR_VOLTAGE_UNIT;

extern 	double		ENDPOINT_SENSOR_CURRENT_MAX;
extern 	double		ENDPOINT_SENSOR_CURRENT_MIN;
extern 	const char*	ENDPOINT_SENSOR_CURRENT_UNIT;

extern 	double		ENDPOINT_SENSOR_WIND_SPEED_MAX;
extern 	double		ENDPOINT_SENSOR_WIND_SPEED_MIN;
extern 	const char*	ENDPOINT_SENSOR_WIND_SPEED_UNIT;

extern 	double		ENDPOINT_SENSOR_RAINFALL_MAX;
extern 	double		ENDPOINT_SENSOR_RAINFALL_MIN;
extern 	const char*	ENDPOINT_SENSOR_RAINFALL_UNIT;

extern 	double		ENDPOINT_SENSOR_SOIL_MOISTURE_MAX;
extern 	double		ENDPOINT_SENSOR_SOIL_MOISTURE_MIN;
extern 	const char*	ENDPOINT_SENSOR_SOIL_MOISTURE_UNIT;

extern 	double		ENDPOINT_SENSOR_SOIL_ACIDITY_MAX;
extern 	double		ENDPOINT_SENSOR_SOIL_ACIDITY_MIN;
extern 	const char*	ENDPOINT_SENSOR_SOIL_ACIDITY_UNIT;

extern 	uint32_t	ENDPOINT_SENSOR_DI_MAX;
extern 	uint32_t	ENDPOINT_SENSOR_DI_MIN;
extern 	const char*	ENDPOINT_SENSOR_DI_UNIT;

extern 	uint32_t	ENDPOINT_ACTUATOR_DO_MAX;
extern 	uint32_t	ENDPOINT_ACTUATOR_DO_MIN;
extern 	const char*	ENDPOINT_ACTUATOR_DO_UNIT;

extern	const char*	ENDPOINT_ACTUATOR_DO_ON;
extern	const char*	ENDPOINT_ACTUATOR_DO_OFF;

extern 	double		ENDPOINT_SENSOR_GAS_MAX;
extern 	double		ENDPOINT_SENSOR_GAS_MIN;
extern 	const char*	ENDPOINT_SENSOR_GAS_UNIT;

extern	const char*	SERVER_LINKER_TOPIC_VERSION;
extern	const char*	SERVER_LINKER_TOPIC_ID;
extern	const char*	SERVER_LINKER_TOPIC_GLOBAL_UP_NAME;
extern	const char*	SERVER_LINKER_TOPIC_GLOBAL_DOWN_NAME;

extern	uint32_t	SERVER_LINKER_CONNECTION_RETRY_INTERVAL_SEC;
extern	uint32_t	SERVER_LINKER_KEEP_ALIVE_INTERVAL_SEC;
extern	uint32_t	SERVER_LINKER_REQUEST_TIMEOUT_SEC;
extern	bool		SERVER_LINKER_REPORT_LATE_ARRIVE_MESSAGE;
extern	uint32_t	SERVER_LINKER_RETRANSMISSION_COUNT_MAX;

#define	DEFAULT_CONST_MAX_DATA_GET_COUNT	50

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
DEFAULT_CONST(DB_TABLE_NAME_GATEWAY, "gateways");
DEFAULT_CONST(DB_TABLE_NAME_DEVICE, "devices");
DEFAULT_CONST(DB_TABLE_NAME_ENDPOINT, "endpoints");
DEFAULT_CONST(GLOBAL_UP_TOPIC, "v1_server_1");
DEFAULT_CONST(GLOBAL_DOWN_TOPIC, "v1_client_1");
DEFAULT_CONST(SERVER_LINKER_HASH, "sha1");

RET_CONST(OK, "ok");
RET_CONST(FAILED, "failed");
RET_CONST(INVALID_ARGUMENTS, "Invalid arguments");
RET_CONST(OBJECT_NOT_FOUND, "Object not found.");
RET_CONST(PERMISSION_DENY, "Permission deny.");
RET_CONST(NOT_ACTUATOR, "Not actuator.");
RET_CONST(TIME_EXPIRED, "Time expired.");

TITLE_NAME(AUTO_START, "auto_start");
TITLE_NAME(BAUDRATE, "baudrate");
TITLE_NAME(BROKER, "broker");
TITLE_NAME(RCS_SERVER, "rcs_server");
TITLE_NAME(CMD, "cmd");
TITLE_NAME(COMMAND, "command");
TITLE_NAME(COMMUNITY, "community");
TITLE_NAME(CORRECTION_INTERVAL, "correction_interval");
TITLE_NAME(COUNT, "count");
TITLE_NAME(CONTROL, "control");
TITLE_NAME(DATA, "data");
TITLE_NAME(DATA_FILE, "data_file");
TITLE_NAME(DATA_MANAGER, "database");
TITLE_NAME(DATE, "date");
TITLE_NAME(DEV_NAME, "dev_name");
TITLE_NAME(DEVICE, "device");
TITLE_NAME(DEVICE_FIELDS, "device_fields");
TITLE_NAME(DEV_ID, "dev_id");
TITLE_NAME(DEVICE_ID, "dev_id");
TITLE_NAME(ENABLE, "enable");
TITLE_NAME(END_TIME, "end_time");
TITLE_NAME(ENDPOINT, "endpoint");
TITLE_NAME(ENDPOINT_FIELDS, "endpoint_fields");
TITLE_NAME(EP_ID, "ep_id");
TITLE_NAME(ENDPOINT_ID, "ep_id");
TITLE_NAME(ENDPOINT_REPORT_INTERVAL,"endpoint_report_interval");
TITLE_NAME(ERROR, "error");
TITLE_NAME(ERROR_MSG, "error_msg");
TITLE_NAME(FIRST_TIME, "first_time");
TITLE_NAME(FILE_NAME, "file_name");
TITLE_NAME(FILE_PATH, "file_path");
TITLE_NAME(FILE_SIZE, "file_size");
TITLE_NAME(GLOBAL_UP_NAME, "global_up");
TITLE_NAME(GLOBAL_DOWN_NAME, "global_down");
TITLE_NAME(GW_ID, "gw_id");
TITLE_NAME(GATEWAY, "gateway");
TITLE_NAME(GATEWAY_FIELDS, "gateway_fields");
TITLE_NAME(GATEWAY_ID, "gw_id");
TITLE_NAME(HASH, "hash");
TITLE_NAME(HASH_ALG, "hash_alg");
TITLE_NAME(ID, "id");
TITLE_NAME(IP, "ip");
TITLE_NAME(KEEP_ALIVE_INTERVAL, "keep_alive_interval");
TITLE_NAME(LAST_TIME, "last_time");
TITLE_NAME(LAST_REPORT_TIME, "last_report_time");
TITLE_NAME(LAST_CONFIRM_TIME, "last_confirm_time");
TITLE_NAME(LOCATION, "location");
TITLE_NAME(LOG, "log");
TITLE_NAME(LOOP_INTERVAL, "loop_interval");
TITLE_NAME(MAX_SESSION, "max_session");
TITLE_NAME(MODEL, "model");
TITLE_NAME(MODULE, "module");
TITLE_NAME(MODE, "mode");
TITLE_NAME(MQTT, "mqtt");
TITLE_NAME(MSG_ID, "msg_id");
TITLE_NAME(MSG_TYPE, "msg_type");
TITLE_NAME(NAME, "name");
TITLE_NAME(OBJECT_MANAGER, "object_manager");
TITLE_NAME(OPTIONS, "options");
TITLE_NAME(PARENT_ID, "parent_id");
TITLE_NAME(PARITY_BIT, "parity_bit");
TITLE_NAME(DATA_BIT, "data_bit");
TITLE_NAME(PATH, "path");
TITLE_NAME(PORT, "port");
TITLE_NAME(PORT_NAME, "port_name");
TITLE_NAME(PORT_BAUDRATE, "port_baudrate");
TITLE_NAME(PORT_PARITY_BIT, "port_parity_bit");
TITLE_NAME(PORT_DATA_BIT, "port_data_bit");
TITLE_NAME(REGISTERED,  "registered");
TITLE_NAME(REQ_ID,  "req_id");
TITLE_NAME(RESULT,  "result");
TITLE_NAME(SCALE, "scale");
TITLE_NAME(SECTION, "section");
TITLE_NAME(SECRET, "secret_code");
TITLE_NAME(SECRET_KEY, "secret_key");
TITLE_NAME(SENSOR_ID, "sensor_id");
TITLE_NAME(SERVER_LINKER, "server_linker");
TITLE_NAME(SIZE, "size");
TITLE_NAME(START_TIME, "start_time");
TITLE_NAME(STAT, "stat");
TITLE_NAME(TCP_SERVER, "tcp_server");
TITLE_NAME(TIME, "time");
TITLE_NAME(TIME_OF_EXPIRE, "time_of_expire");
TITLE_NAME(TIMEOUT, "timeout");
TITLE_NAME(TOPIC, "topic");
TITLE_NAME(TRACE, "trace");
TITLE_NAME(TYPE, "type");
TITLE_NAME(UNIT, "unit");
TITLE_NAME(UPDATE_INTERVAL, "update_interval");
TITLE_NAME(VALUE, "value");
TITLE_NAME(VALUE_MAX, "value_max");
TITLE_NAME(VALUE_MIN, "value_min");
TITLE_NAME(VERSION, "version");
TITLE_NAME(WITH_DEVICE, "width_device");
TITLE_NAME(WITH_ENDPOINT, "width_endpoint");

DEFAULT_CONST(MSG_VERSION, "v1");
#define	DEFAULT_CONST_MSG_PARTITION	0
//#define	DEFAULT_CONST_MSG_PARTITION RdKafka::Topic::PARTITION_UA

MSG_TYPE_RCS(KEEP_ALIVE,"keep_alive");
MSG_TYPE_RCS(REPORT, 	"report");
MSG_TYPE_RCS(ADD, 		"add");
MSG_TYPE_RCS(DEL, 		"del");
MSG_TYPE_RCS(GET, 		"get");
MSG_TYPE_RCS(SET, 		"set");
MSG_TYPE_RCS(LIST, 		"list");
MSG_TYPE_RCS(CONFIRM,	"confirm");
MSG_TYPE_RCS(ERROR, 	"error");

#endif

