#define	MAKE_BODY	1

#include "defined.h"

uint64_t	TIME_MILLISECOND				=	((uint64_t)1000UL);
uint64_t	TIME_SECOND						=	((uint64_t)1000000UL);
uint32_t	TIME_MINUTE						=	(60 * TIME_SECOND);

double		ACTIVE_OBJECT_LOOP_INTERVAL		=	0.001;

uint32_t	OBJECT_KEEP_ALIVE_INTERVAL_SEC	=	(60);
uint32_t	OBJECT_KEEP_ALIVE_INTERVAL_SEC_MIN=	(10);
uint32_t	OBJECT_KEEP_ALIVE_INTERVAL_SEC_MAX=	(60 * 60);

const char* OBJECT_TYPE_NODE				=	"node";

const char* OBJECT_TYPE_GATEWAY				=	"gateway";
const char*	OBJECT_TYPE_GW_GEN				=	"gw_gen";

const char* OBJECT_TYPE_DEVICE				=	"device";
const char*	OBJECT_TYPE_DEV_SNMP			=	"dev_snmp";
const char*	OBJECT_TYPE_DEV_FTE				=	"dev_fte";
const char*	OBJECT_TYPE_DEV_SIM				=	"dev_sim";

const char*	OBJECT_TYPE_DEV_MODBUS			=	"dev_modbus";
const char*	OBJECT_TYPE_DEV_GTC_520A		=	"dev_gtc_520a";
const char*	OBJECT_TYPE_DEV_HS_1000M		=	"dev_hs_1000m";
const char*	OBJECT_TYPE_DEV_MB7092			=	"dev_mb7092";

const char*	OBJECT_TYPE_DEV_MODBUS_TCP		=	"dev_modbus_tcp";
const char*	OBJECT_TYPE_DEV_ADAM_6051		=	"dev_adam_6051";
const char*	OBJECT_TYPE_DEV_TURBOMAX		=	"dev_turbomax";

const char* OBJECT_TYPE_ENDPOINT			=	"endpoint";
const char* OBJECT_TYPE_ENDPOINT_SENSOR		=	"en_sensor";
const char*	OBJECT_TYPE_EP_S_DISCRETE		=	"ep_s_discrete";
const char*	OBJECT_TYPE_EP_S_LINEAR			=	"ep_s_linear";
const char*	OBJECT_TYPE_EP_S_TEMPERATURE	=	"ep_s_temperature";
const char*	OBJECT_TYPE_EP_S_HUMIDITY		=	"ep_s_humidity";
const char*	OBJECT_TYPE_EP_S_VOLTAGE		=	"ep_s_voltage";
const char*	OBJECT_TYPE_EP_S_WAVE		=	"ep_s_wave";
const char*	OBJECT_TYPE_EP_S_CURRENT		=	"ep_s_current";
const char*	OBJECT_TYPE_EP_S_DI				=	"ep_s_di";
const char*	OBJECT_TYPE_EP_S_PRESSURE		=	"ep_s_pressure";
const char*	OBJECT_TYPE_EP_S_WIND_SPEED		=	"ep_s_wind_speed";
const char*	OBJECT_TYPE_EP_S_SOIL_MOISTURE	=	"ep_s_soil_moisture";
const char*	OBJECT_TYPE_EP_S_RAINFALL		=	"ep_s_rainfall";
const char*	OBJECT_TYPE_EP_S_GAS			=	"ep_s_gas";
const char*	OBJECT_TYPE_EP_S_MB_DATA		=	"ep_s_mb_data";
const char* OBJECT_TYPE_EP_S_SOIL_ACIDITY	=	"ep_s_soil_acidity";

const char* OBJECT_TYPE_ENDPOINT_ACUTATOR	=	"ep_actuator";
const char*	OBJECT_TYPE_EP_A_TEMPCONTROL		=	"ep_a_tempcontrol";
const char*	OBJECT_TYPE_EP_A_DISCRETE			=	"ep_a_discrete";
const char*	OBJECT_TYPE_EP_A_DO				=	"ep_a_do";
const char*	OBJECT_TYPE_EP_A_FX3DTEMPCTR			=	"ep_a_fx3dtempctr";

uint32_t	GATEWAY_START_INITIALIZATION_TIMEOUT=	1000;

uint32_t	DEVICE_START_INITIALIZATION_TIMEOUT=	1000;	//ms

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

double		ENDPOINT_SENSOR_WAVE_MAX		= 	(800);
double		ENDPOINT_SENSOR_WAVE_MIN		= 	(0);
const char*	ENDPOINT_SENSOR_WAVE_UNIT	= 	"cm";

double		ENDPOINT_SENSOR_CURRENT_MAX		= 	(100);
double		ENDPOINT_SENSOR_CURRENT_MIN		= 	(0);
const char*	ENDPOINT_SENSOR_CURRENT_UNIT	= 	"A";

double		ENDPOINT_SENSOR_WIND_SPEED_MAX	=	10000;
double		ENDPOINT_SENSOR_WIND_SPEED_MIN	=	0;
const char*	ENDPOINT_SENSOR_WIND_SPEED_UNIT	=	"m/s";

double		ENDPOINT_SENSOR_RAINFALL_MAX	=	10000;
double		ENDPOINT_SENSOR_RAINFALL_MIN	=	0;
const char*	ENDPOINT_SENSOR_RAINFALL_UNIT	=	"mm";

double		ENDPOINT_SENSOR_SOIL_MOISTURE_MAX	= 100;
double		ENDPOINT_SENSOR_SOIL_MOISTURE_MIN	= 0;
const char*	ENDPOINT_SENSOR_SOIL_MOISTURE_UNIT	= "%";

double		ENDPOINT_SENSOR_SOIL_ACIDITY_MAX	= 10;
double		ENDPOINT_SENSOR_SOIL_ACIDITY_MIN	= 0;
const char*	ENDPOINT_SENSOR_SOIL_ACIDITY_UNIT	= "";

double		ENDPOINT_SENSOR_GAS_MAX			= 100;
double		ENDPOINT_SENSOR_GAS_MIN			= 0;
const char*	ENDPOINT_SENSOR_GAS_UNIT		= "%";

uint32_t	ENDPOINT_SENSOR_MB_DATA_MAX		= 0xffff;
uint32_t	ENDPOINT_SENSOR_MB_DATA_MIN		= 0x0000;
const char*	ENDPOINT_SENSOR_MB_DATA_UNIT		= "";


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
uint32_t	SERVER_LINKER_REQUEST_TIMEOUT_SEC		= (5);
bool		SERVER_LINKER_REPORT_LATE_ARRIVE_MESSAGE	= false;
uint32_t	SERVER_LINKER_RETRANSMISSION_COUNT_MAX	= 5;

const char*	SERVER_LINKER_TOPIC_VERSION			=	"v1";
const char*	SERVER_LINKER_TOPIC_ID 				=	"1";
const char*	SERVER_LINKER_TOPIC_GLOBAL_UP_NAME 	=	"server";
const char*	SERVER_LINKER_TOPIC_GLOBAL_DOWN_NAME=	"client";

//add SY.KANG
uint32_t	GATEWAY_INFO_INTERVAL_SEC		= (60*5);
//
