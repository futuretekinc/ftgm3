#ifndef	DEVICE_NBIOTM_H_
#define	DEVICE_NBIOTM_H_

#include "device_modbus.h"

#define MQTT_DUP_FLAG     1<<3 
#define MQTT_QOS0_FLAG    0<<1 
#define MQTT_QOS1_FLAG    1<<1 
#define MQTT_QOS2_FLAG    2<<1 


#define MQTT_RETAIN_FLAG  1 


#define MQTT_CLEAN_SESSION  1<<1 
#define MQTT_WILL_FLAG      1<<2 
#define MQTT_WILL_RETAIN    1<<5 
#define MQTT_USERNAME_FLAG  1<<7 
#define MQTT_PASSWORD_FLAG  1<<6 
#define MQTT_MSG_CONNECT       1<<4 
#define MQTT_MSG_CONNACK       2<<4 
#define MQTT_MSG_PUBLISH       3<<4 
#define MQTT_MSG_PUBACK        4<<4 
#define MQTT_MSG_PUBREC        5<<4 
#define MQTT_MSG_PUBREL        6<<4 
#define MQTT_MSG_PUBCOMP       7<<4 
#define MQTT_MSG_SUBSCRIBE     8<<4 
#define MQTT_MSG_SUBACK        9<<4 
#define MQTT_MSG_UNSUBSCRIBE  10<<4 
#define MQTT_MSG_UNSUBACK     11<<4 
#define MQTT_MSG_PINGREQ      12<<4 
#define MQTT_MSG_PINGRESP     13<<4 
#define MQTT_MSG_DISCONNECT   14<<4 

class	DeviceNBIOTM : public DeviceModbus
{
public:
	DeviceNBIOTM(ObjectManager& _manager);
	DeviceNBIOTM(ObjectManager& _manager, JSONNode const& _properties);

			const char*	GetClassName();

	bool	ReadValue(std::string const& _id, time_t& _time, std::string& _value);
/*	bool    SetOption(JSONNode const& _option, bool _check);
	bool    SetIP(std::string const _ip, bool _check); 
	bool    SetPort(std::string const _port, bool _check); 
	bool    SetID(std::string const _id, bool _check);
*/
	void    nbiot_strsplit(std::string* strResult, std::string strOrigin, std::string strTok, int size);
	bool    convert_string(uint8_t hex_, std::string& save_msg);
	bool    nbiotm_msg_gen(uint32_t size, uint8_t* message, std::string& gen_msg);
	bool   	mqtt_connect_msg_gen(std::string& mqtt_conn_msg, uint32_t& msg_size);
	bool    mqtt_publish_msg_gen(std::string const& topic, std::string const& payload, std::string& mqtt_pub_msg, uint32_t& mqtt_pub_msg_len);
	bool    nbiot_status();
	bool    tcp_connect();
	bool    tcp_disconnect();
	bool    mqtt_connect(); // nb-iot 
  	bool    mqtt_publish(uint8_t sensor_id);
	bool    mqtt_disconnect();
	bool    reset_status();
	const std::string&  GEN_PUBLISH_PAYLOAD(uint8_t sensor_id);
protected:

	void	Preprocess();
	void	Process();

	Timer		correction_timer_;
	uint32_t	correction_interval_;

	time_t		time_;
	int16_t		registers_[2];
	uint8_t		process_state;
	std::string	ip_;
	std::string	port_;
	std::string	id_;
	std::string	socket_fd_;
	uint32_t	response_timeout_;
	// Static members
public:
	static	const char*	Type();
};
#endif
