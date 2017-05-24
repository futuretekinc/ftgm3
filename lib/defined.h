#ifndef	DEFINED_H_
#define	DEFINED_H_

#include <cstdint>

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

extern	const char* DB_TABLE_NAME_DEVICE;
extern	const char* DB_TABLE_NAME_ENDPOINT;

extern	const char* RMC_RESULT_OK;
extern	const char* RMC_RESULT_FAILED;
extern	const char* RMC_FIELD_SECTION;
extern	const char* RMC_FIELD_COMMAND;
extern	const char* RMC_FIELD_RESULT;
extern	const char* RMC_FIELD_ERROR;
extern	const char* RMC_FIELD_TYPE;
extern	const char* RMC_FIELD_DEVICE;
extern	const char* RMC_FIELD_DEVICE_ARRAY;
extern	const char* RMC_FIELD_ENDPOINT;
extern	const char* RMC_FIELD_ENDPOINT_ARRAY;
extern	const char* RMC_FIELD_ID;
extern	const char*	RMC_FIELD_ID_ARRAY;
extern	const char*	RMC_FIELD_NAME;

extern	const char* OBJECT_FIELD_TYPE;
extern	const char* OBJECT_FIELD_DEVICE;
extern	const char* OBJECT_FIELD_DEVICE_ARRAY;
extern	const char* OBJECT_FIELD_ID;
extern	const char* OBJECT_FIELD_ID_ARRAY;
extern	const char* OBJECT_FIELD_NAME;
extern	const char* OBJECT_FIELD_DATE;
extern	const char* OBJECT_FIELD_ENABLE;
extern	const char* OBJECT_FIELD_SENSOR_ID;
extern	const char* OBJECT_FIELD_DEVICE_ID;
extern	const char* OBJECT_FIELD_NAME;
extern	const char* OBJECT_FIELD_UNIT;
extern	const char* OBJECT_FIELD_SCALE;
extern	const char* OBJECT_FIELD_UPDATE_INTERVAL;
extern	const char* OBJECT_FIELD_LOOP_INTERVAL;
extern	const char* OBJECT_FIELD_LIVE_CHECK_INTERVAL;
extern	const char* OBJECT_FIELD_MODULE;
extern	const char* OBJECT_FIELD_IP;
extern	const char* OBJECT_FIELD_COMMUNITY;
extern	const char* OBJECT_FIELD_TIMEOUT;
extern	const char* OBJECT_FIELD_DEV_NAME;
extern	const char* OBJECT_FIELD_ENDPOINTS;
extern	const char* OBJECT_FIELD_VALUE_MAX;
extern	const char* OBJECT_FIELD_VALUE_MIN;

extern	const char* MSG_FIELD_CMD;
extern	const char* MSG_FIELD_TIME;
extern	const char* MSG_FIELD_DATA;
extern	const char* MSG_FIELD_COUNT;

extern	const char* MSG_FIELD_TYPE;
extern	const char* MSG_FIELD_DEVICE;
extern	const char* MSG_FIELD_DEVICE_ID;
extern	const char* MSG_FIELD_ENDPOINT_ID;
extern	const char* MSG_FIELD_DATE;
extern	const char* MSG_FIELD_ENABLE;
extern	const char* MSG_FIELD_SENSOR_ID;
extern	const char* MSG_FIELD_NAME;
extern	const char* MSG_FIELD_UNIT;
extern	const char* MSG_FIELD_SCALE;
extern	const char* MSG_FIELD_UPDATE_INTERVAL;
extern	const char* MSG_FIELD_LOOP_INTERVAL;
extern	const char* MSG_FIELD_LIVE_CHECK_INTERVAL;
extern	const char* MSG_FIELD_MODULE;
extern	const char* MSG_FIELD_IP;
extern	const char* MSG_FIELD_COMMUNITY;
extern	const char* MSG_FIELD_TIMEOUT;
extern	const char* MSG_FIELD_VALUE;
extern	const char* MSG_FIELD_VALUE_MAX;
extern	const char* MSG_FIELD_VALUE_MIN;
extern	const char* MSG_FIELD_DEV_NAME;
extern	const char* MSG_FIELD_ENDPOINTS;

extern	const char* MSG_COMMAND_KEEP_ALIVE;
extern	const char* MSG_COMMAND_ENDPOINT_REPORT;
enum	RetValue
{
	RET_VALUE_OK,
	RET_VALUE_ERROR,
	RET_VALUE_INVALID_ARGUMENTS,
	RET_VALUE_INVALID_TYPE,
	RET_VALUE_INVALID_NAME,
};

#endif

