#define	MAKE_BODY	1

#include "defined.h"

uint64_t	TIME_MILLISECOND				=	((uint64_t)1000UL);
uint64_t	TIME_SECOND						=	((uint64_t)1000000UL);
uint32_t	TIME_MINUTE						=	(60 * TIME_SECOND);

uint32_t	ACTIVE_OBJECT_LOOP_INTERVAL		=	(1 * TIME_MILLISECOND);

uint32_t	OBJECT_KEEP_ALIVE_INTERVAL_SEC	=	(10);
uint32_t	OBJECT_KEEP_ALIVE_INTERVAL_SEC_MIN=	(10);
uint32_t	OBJECT_KEEP_ALIVE_INTERVAL_SEC_MAX=	(60 * 60);

const char*	NODE_TYPE_GW_GEN				=	"gw_gen";

const char*	NODE_TYPE_DEV_SNMP				=	"dev_snmp";
const char*	NODE_TYPE_DEV_FTE				=	"dev_fte";

const char*	NODE_TYPE_EP_S_DISCRETE			=	"ep_s_discrete";
const char*	NODE_TYPE_EP_S_LINEAR			=	"ep_s_linear";
const char*	NODE_TYPE_EP_S_TEMPERATURE		=	"ep_s_temperature";
const char*	NODE_TYPE_EP_S_HUMIDITY			=	"ep_s_humidity";
const char*	NODE_TYPE_EP_S_VOLTAGE			=	"ep_s_voltage";
const char*	NODE_TYPE_EP_S_CURRENT			=	"ep_s_current";
const char*	NODE_TYPE_EP_S_DI				=	"ep_s_di";
const char*	NODE_TYPE_EP_S_PRESSURE			=	"ep_s_pressure";
const char*	NODE_TYPE_EP_S_WIND_SPEED		=	"ep_s_wind_speed";
const char*	NODE_TYPE_EP_S_SOIL_MOISTURE	=	"ep_s_soil_moisture";
const char*	NODE_TYPE_EP_S_RAINFALL			=	"ep_s_rainfall";

const char*	NODE_TYPE_EP_A_DISCRETE			=	"ep_a_discrete";
const char*	NODE_TYPE_EP_A_LINEAR			=	"ep_a_linear";
const char*	NODE_TYPE_EP_A_DO				=	"ep_a_do";

uint32_t	ENDPOINT_REPORT_INTERVAL		=	(30);
uint32_t	ENDPOINT_UPDATE_INTERVAL		=	(5);
uint32_t	ENDPOINT_VALUE_COUNT			=	(24*60*10);
uint32_t	ENDPOINT_VALUE_COUNT_MAX		=	(7*24*60*10);
double		ENDPOINT_VALUE_SCALE			=	(1);

double		ENDPOINT_SENSOR_TEMPERATURE_MAX	=	(100);
double		ENDPOINT_SENSOR_TEMPERATURE_MIN	=	(-20);
const char*	ENDPOINT_SENSOR_TEMPERATURE_UNIT=	"C";

double		ENDPOINT_SENSOR_HUMIDITY_MAX	= 	(100);
double		ENDPOINT_SENSOR_HUMIDITY_MIN	= 	(0);
const char*	ENDPOINT_SENSOR_HUMIDITY_UNIT	= 	"%RH";

double		ENDPOINT_SENSOR_VOLTAGE_MAX		= 	(380);
double		ENDPOINT_SENSOR_VOLTAGE_MIN		= 	(0);
const char*	ENDPOINT_SENSOR_VOLTAGE_UNIT	= 	"V";

double		ENDPOINT_SENSOR_CURRENT_MAX		= 	(100);
double		ENDPOINT_SENSOR_CURRENT_MIN		= 	(0);
const char*	ENDPOINT_SENSOR_CURRENT_UNIT	= 	"A";

double		ENDPOINT_SENSOR_WIND_SPEED_MAX	=	100;
double		ENDPOINT_SENSOR_WIND_SPEED_MIN	=	0;
const char*	ENDPOINT_SENSOR_WIND_SPEED_UNIT	=	"m/s";

double		ENDPOINT_SENSOR_RAINFALL_MAX	=	10000;
double		ENDPOINT_SENSOR_RAINFALL_MIN	=	0;
const char*	ENDPOINT_SENSOR_RAINFALL_UNIT	=	"mm";

double		ENDPOINT_SENSOR_SOIL_MOISTURE_MAX	= 100;
double		ENDPOINT_SENSOR_SOIL_MOISTURE_MIN	= 0;
const char*	ENDPOINT_SENSOR_SOIL_MOISTURE_UNIT	= "%";

uint32_t	ENDPOINT_SENSOR_DI_MAX			= 	(1);
uint32_t	ENDPOINT_SENSOR_DI_MIN			= 	(0);
const char*	ENDPOINT_SENSOR_DI_UNIT			= 	"";

uint32_t	ENDPOINT_ACTUATOR_DO_MAX		= 	(1);
uint32_t	ENDPOINT_ACTUATOR_DO_MIN		= 	(0);
const char*	ENDPOINT_ACTUATOR_DO_UNIT		= 	"";

const char*	ENDPOINT_ACTUATOR_DO_ON			=	"1";
const char*	ENDPOINT_ACTUATOR_DO_OFF		=	"0";

int			SNMP_COMMUNITY_LENGTH_MAX		=	128;

uint32_t	SERVER_LINKER_CONNECTION_RETRY_INTERVAL_SEC = (10);
uint32_t	SERVER_LINKER_KEEP_ALIVE_INTERVAL_SEC 	= (60);
uint32_t	SERVER_LINKER_REQUEST_TIMEOUT_SEC		= (30);
bool		SERVER_LINKER_REPORT_LATE_ARRIVE_MESSAGE	= false;
