#include "defined.h"

uint64_t	TIME_MILLISECOND				=	((uint64_t)1000UL);
uint64_t	TIME_SECOND						=	((uint64_t)1000000UL);
uint32_t	TIME_MINUTE						=	(60 * TIME_SECOND);

uint32_t	ACTIVE_OBJECT_LOOP_INTERVAL		=	(1 * TIME_MILLISECOND);

uint32_t	OBJECT_LIVE_CHECK_INTERVAL_SEC	=	(10);
uint32_t	OBJECT_LIVE_CHECK_INTERVAL_SEC_MIN=	(10);
uint32_t	OBJECT_LIVE_CHECK_INTERVAL_SEC_MAX=	(60 * 60);

uint32_t	ENDPOINT_REPORT_INTERVAL		=	(10 * TIME_SECOND);
uint32_t	ENDPOINT_UPDATE_INTERVAL		=	(1 * TIME_SECOND);
uint32_t	ENDPOINT_VALUE_COUNT_MAX		=	(7*24*60);
double		ENDPOINT_VALUE_SCALE			=	(1);

int			ENDPOINT_SENSOR_TEMPERATURE_MAX	=	(100);
int			ENDPOINT_SENSOR_TEMPERATURE_MIN	=	(-20);
const char*	ENDPOINT_SENSOR_TEMPERATURE_UNIT=	"C";

uint32_t	ENDPOINT_SENSOR_HUMIDITY_MAX	= (100);
uint32_t	ENDPOINT_SENSOR_HUMIDITY_MIN	= (0);
const char*	ENDPOINT_SENSOR_HUMIDITY_UNIT	= "%RH";

const char*	DEFAULT_LOCAL_IP		=	"127.0.0.1";
const char*	DEFAULT_SERVER_IP		=	"127.0.0.1";
uint16_t	DEFAULT_SERVER_PORT		=	8888;

int			SNMP_COMMUNITY_LENGTH_MAX=	128;

const char*	LOG_FILE_PATH			= "/var/log/";
uint32_t	LOG_FILE_SIZE			= (1024*1024);

const char* DB_TABLE_NAME_DEVICE	= "devices";
const char* DB_TABLE_NAME_ENDPOINT	= "endpoints";

const char* RMC_RESULT_OK			= "ok";
const char* RMC_RESULT_FAILED		= "failed";

const char* RMC_FIELD_SECTION		= "section";
const char* RMC_FIELD_COMMAND		= "command";
const char* RMC_FIELD_RESULT		= "result";
const char* RMC_FIELD_ERROR			= "error";
const char* RMC_FIELD_TYPE			= "type";
const char* RMC_FIELD_DEVICE		= "device";
const char* RMC_FIELD_DEVICE_ARRAY	= "devices";
const char* RMC_FIELD_ENDPOINT		= "endpoint";
const char* RMC_FIELD_ENDPOINT_ARRAY= "endpoints";
const char* RMC_FIELD_ID			= "id";
const char* RMC_FIELD_ID_ARRAY		= "ids";
const char* RMC_FIELD_NAME			= "name";

const char* OBJECT_FIELD_TYPE		= "type";
const char* OBJECT_FIELD_DEVICE		= "device";
const char* OBJECT_FIELD_DEVICE_ARRAY="devices";
const char* OBJECT_FIELD_ID			= "id";
const char* OBJECT_FIELD_ID_ARRAY	= "ids";
const char* OBJECT_FIELD_DATE		= "date";
const char* OBJECT_FIELD_ENABLE		= "enable";
const char* OBJECT_FIELD_SENSOR_ID	= "sensor_id";
const char* OBJECT_FIELD_DEVICE_ID	= "device_id";
const char* OBJECT_FIELD_NAME		= "name";
const char* OBJECT_FIELD_UNIT		= "unit";
const char* OBJECT_FIELD_SCALE		= "scale";
const char* OBJECT_FIELD_UPDATE_INTERVAL= "update_interval";
const char* OBJECT_FIELD_LOOP_INTERVAL= "loop_interval";
const char* OBJECT_FIELD_LIVE_CHECK_INTERVAL	= "live_check_interval";
const char* OBJECT_FIELD_MODULE		= "module";
const char* OBJECT_FIELD_IP			= "ip";
const char* OBJECT_FIELD_COMMUNITY	= "community";
const char* OBJECT_FIELD_TIMEOUT	= "timeout";
const char* OBJECT_FIELD_VALUE_MAX	= "value_max";
const char* OBJECT_FIELD_VALUE_MIN	= "value_min";
const char* OBJECT_FIELD_DEV_NAME	= "dev_name";
const char* OBJECT_FIELD_ENDPOINTS	= "endpoints";

const char* MSG_FIELD_CMD			= "cmd";
const char* MSG_FIELD_TIME			= "time";
const char* MSG_FIELD_DATA			= "data";
const char* MSG_FIELD_COUNT			= "count";

const char* MSG_FIELD_TYPE			= "type";
const char* MSG_FIELD_DEVICE		= "device";
const char* MSG_FIELD_DEVICE_ID		= "device_id";
const char* MSG_FIELD_ENDPOINT_ID	= "ep_id";
const char* MSG_FIELD_DATE			= "date";
const char* MSG_FIELD_ENABLE		= "enable";
const char* MSG_FIELD_SENSOR_ID		= "sensor_id";
const char* MSG_FIELD_NAME			= "name";
const char* MSG_FIELD_UNIT			= "unit";
const char* MSG_FIELD_SCALE			= "scale";
const char* MSG_FIELD_UPDATE_INTERVAL= "update_interval";
const char* MSG_FIELD_LOOP_INTERVAL= "loop_interval";
const char* MSG_FIELD_LIVE_CHECK_INTERVAL	= "live_check_interval";
const char* MSG_FIELD_MODULE		= "module";
const char* MSG_FIELD_IP			= "ip";
const char* MSG_FIELD_COMMUNITY		= "community";
const char* MSG_FIELD_TIMEOUT		= "timeout";
const char* MSG_FIELD_VALUE			= "value";
const char* MSG_FIELD_VALUE_MAX		= "value_max";
const char* MSG_FIELD_VALUE_MIN		= "value_min";
const char* MSG_FIELD_DEV_NAME		= "dev_name";
const char* MSG_FIELD_ENDPOINTS		= "endpoints";

const char* MSG_COMMAND_KEEP_ALIVE	=	"keep_alive";
const char* MSG_COMMAND_ENDPOINT_REPORT="endpoint_report";
