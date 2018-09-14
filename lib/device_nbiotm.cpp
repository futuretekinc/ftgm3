#include "defined.h"
#include "exception.h"
#include "utils.h"
#include "object_manager.h"
#include "device_nbiotm.h"
#include "server_linker.h"

static const char*	class_name = "Devicenbiotm";

DeviceNBIOTM::DeviceNBIOTM(ObjectManager& _manager)
: DeviceModbus(_manager, OBJECT_TYPE_DEV_NB_IOT_M, true),process_state(0), correction_interval_(1),response_timeout_(5)
{
	serial_.SetBaudrate(115200);
	serial_.SetParityBit("none");
	serial_.SetDataBit(8);
}

DeviceNBIOTM::DeviceNBIOTM(ObjectManager& _manager, JSONNode const& _properties)
: DeviceModbus(_manager, OBJECT_TYPE_DEV_NB_IOT_M, true), process_state(0), correction_interval_(1), response_timeout_(5)
{
	TRACE_INFO("NB_IOT_M Created");
	serial_.SetBaudrate(115200);
	serial_.SetParityBit("none");
	serial_.SetDataBit(8);
	SetProperties(_properties, false, true);
}

const char*	DeviceNBIOTM::GetClassName()
{
	return	class_name;
}

void	DeviceNBIOTM::nbiot_strsplit(std::string* strResult, std::string strOrigin, std::string strTok, int size)
{
	int cutAt;
	int index = 0;
//	std::string* strResult = new std::string[size];

	while((cutAt = strOrigin.find_first_of(strTok)) != strOrigin.npos)
	{
		if( cutAt > 0)
		{
			strResult[index++] = strOrigin.substr(0,cutAt);
		}
		strOrigin = strOrigin.substr(cutAt + 1);
	}
	if(strOrigin.length() > 0 )
	{
		strResult[index++] = strOrigin.substr(0, cutAt);
	}
//	return strResult;
}

bool	DeviceNBIOTM::reset_status()
{
	std::string at_command = "AT\r\n";
	std::string at_reset_command = "AT+CFUN=6\r\n";
	uint8_t response[100];
	uint32_t response_len = 0;
	std::string	etc = "\r\n+:? ";
	std::string	s_ok = "OK";
	std::string	s_error = "ERROR";
	std::string response_msg;
	bool	result = false;
	
	memset(response, 0x00, sizeof(response));
	TRACE_INFO("RESET_STATUS");
	RequestAndResponse((uint8_t*)at_command.c_str(), at_command.length(), response, sizeof(response), response_len, response_timeout_);
	if(response_len == 0)
	{
		return false;
	}
	else
	{
		response_msg = (char*)response;
		
		TRACE_INFO("RECEIVE MESSAGE : " << response_msg);	
		std::string* rev_value = new std::string[10];
		nbiot_strsplit(rev_value, response_msg, etc, 10);
		
		for(uint32_t i = 0; i < 10 ; i++)
		{
			if(s_ok.compare(rev_value[i]) == 0)
			{
				result = true;				
			}
		}
		delete[] rev_value;
		if(result == false)
		{
			return false;
		}
	}
	
	response_len = 0;
	memset(response, 0x00, sizeof(response));
	TRACE_INFO("AT COMMAND : " << at_reset_command);
	RequestAndResponse((uint8_t*)at_reset_command.c_str(), at_reset_command.length(), response, sizeof(response), response_len, response_timeout_);
	
/*	if(response_len == 0)
	{
		return false;
	}
	else
	{
		
		response_msg = (char*) response;
		
		TRACE_INFO("RECEIVE MESSAGE : " << response_msg);	
		std::string* rev_value = new std::string[10];
	       	nbiot_strsplit(rev_value, response_msg, etc, 10);
		for(uint32_t j = 0 ; j < 10 ; j++)
		{
			if(s_error.compare(rev_value[j]) == 0)
			{
				delete[] rev_value;
				return false;
			}
		}	
		delete[] rev_value;
	}*/
	return true;
}

bool	DeviceNBIOTM::nbiot_status() // nb-iot lte status check
{
	std::string at_command = "AT\r\n";
	std::string at_net_status_command = "AT+CEREG?\r\n";
	uint8_t	response[100];
	uint32_t response_len;
	bool	result = false;
	std::string response_msg;
	std::string	etc = "\r\n+:? ";
	std::string	s_ok = "OK";
	std::string	s_error = "ERROR";
	
	memset(response, 0x00, sizeof(response));
	TRACE_INFO("NET_STATUS_STATE");
	TRACE_INFO("AT COMMAND : " << at_command);
	RequestAndResponse((uint8_t*)at_command.c_str(), at_command.length(), response, sizeof(response), response_len, response_timeout_ );
	if(response_len == 0)
	{
		return false;
	}
	else
	{
		response_msg = (char*)response;
		
		TRACE_INFO("RECEIVE MESSAGE : " << response_msg);	
		std::string* rev_value = new std::string[10];
		nbiot_strsplit(rev_value, response_msg, etc, 10);
		
		for(uint32_t i = 0; i < 2 ; i++)
		{
			if(s_ok.compare(rev_value[i]) == 0)
			{
				result = true;				
			}
		}
		delete[] rev_value;
		if(result == false)
		{
			return false;
		}
	}

	response_len = 0;
	memset(response, 0x00, sizeof(response));
	TRACE_INFO("AT COMMAND : " << at_net_status_command);
	RequestAndResponse((uint8_t*)at_net_status_command.c_str(), at_net_status_command.length(), response, sizeof(response), response_len, response_timeout_);
	
	if(response_len == 0)
	{
		return false;
	}
	else
	{
		
		response_msg = (char*) response;
		
		TRACE_INFO("RECEIVE MESSAGE : " << response_msg);	
		std::string* rev_value = new std::string[10];
	       	nbiot_strsplit(rev_value, response_msg, etc, 10);
		for(uint32_t j = 0 ; j < 4 ; j++)
		{
			if(s_error.compare(rev_value[j]) == 0)
			{
				delete[] rev_value;
				return false;
			}
			else if(rev_value[j] == "0,0")
			{
				delete[] rev_value;
				return false;
			}
		}	
		delete[] rev_value;
	}

	return true;
}

bool	DeviceNBIOTM::tcp_connect()
{
	std::string	at_tcp_conn_cmd;
	
	std::string	etc = "\r\n+:? ";
	std::string	s_error = "ERROR";
	uint32_t	response_len;
	uint8_t		response[100];
	std::string	response_msg;	

	//std::string  svc_port = ServerLinker::GetPort();
	//std::string  svc_port = serial_.GetDevPort();
	std::string  svc_port =manager_.GetMqttInfo(1);
	at_tcp_conn_cmd = "AT+NSOCR=STREAM,6," + svc_port + ",0\r\n";

	response_len = 0;
	memset(response, 0x00, sizeof(response));
	TRACE_INFO("TCP_CONNTECT_STATE");
	TRACE_INFO("AT COMMAND : " << at_tcp_conn_cmd);
	uint8_t retry = 0;	
	for( ;  retry < 3 ; retry++)
	{
		RequestAndResponse((uint8_t*)at_tcp_conn_cmd.c_str(), at_tcp_conn_cmd.length(), response, sizeof(response), response_len, response_timeout_);

		if(response_len == 0)
		{
			continue;
		}
		else
		{
		
			response_msg = (char*) response;
		
			TRACE_INFO("RECEIVE MESSAGE : " << response_msg);	
		
			std::string*	rev_value = new std::string[10];
			nbiot_strsplit(rev_value, response_msg, etc, 10);
			for(uint32_t j = 0; j < 10 ; j++)
			{
				if(s_error.compare(rev_value[j]) == 0)
				{
					delete[] rev_value;
					return false;
				}
			}
			socket_fd_ = rev_value[0];
			delete[] rev_value;
			TRACE_INFO("RECEIVE SOCKET FD : " << socket_fd_);	
			break;	
		}
	}

	if(retry == 3)
	{
		return false;
	}	
	return true;
}

bool	DeviceNBIOTM::tcp_disconnect()
{

	return true;
}

bool    DeviceNBIOTM::mqtt_connect() // nb-iot 
{
	
	std::string at_tcp_mqtt_connect = "AT+NSOST=";
	//std::string nbiot_broker_ip = serial_.GetDevIP();
	std::string nbiot_broker_ip = manager_.GetMqttInfo(0);
	//std::string nbiot_broker_port = serial_.GetDevPort();
	std::string nbiot_broker_port = manager_.GetMqttInfo(1);
	std::string nbiot_broker_conn_msg ;
	uint32_t nbiot_broker_conn_msg_len = 0;
	uint8_t	response[50];
	uint32_t response_len = 0;
	std::string s_error = "ERROR";
	std::string etc = "\r\n+: ";
	TRACE_INFO("MQTT_CONNECT_STATE");	
	TRACE_INFO("AT COMMAND : " << at_tcp_mqtt_connect);
	TRACE_INFO("SOCKET FD : " << socket_fd_);
	TRACE_INFO("BROKER IP : " << nbiot_broker_ip);
	TRACE_INFO("BROKER PORT : " << nbiot_broker_port);

	at_tcp_mqtt_connect += socket_fd_+","+nbiot_broker_ip+","+nbiot_broker_port+",";
	mqtt_connect_msg_gen(nbiot_broker_conn_msg, nbiot_broker_conn_msg_len);

	if(nbiot_broker_conn_msg_len == 0)
	{
		return false;
	}
	
	at_tcp_mqtt_connect += ToString(nbiot_broker_conn_msg_len) + "," + nbiot_broker_conn_msg + "\r\n";
      	TRACE_INFO("AT COMMAND : " << at_tcp_mqtt_connect); 
	uint8_t retry = 0;
	for( ; retry < 3 ; retry++)
	{
		RequestAndResponse((uint8_t*)at_tcp_mqtt_connect.c_str(), at_tcp_mqtt_connect.length(), response, sizeof(response), response_len, response_timeout_);
	
		if(response_len == 0)
		{
			continue;
		}
		else
		{
		//	TRACE_INFO("RECEIVE MESSAGE : " << (char*) response);	
			std::string response_msg = (char*) response;
			std::string* rev_value = new std::string[10];
			nbiot_strsplit(rev_value, response_msg, etc, 10);
			for(uint32_t j = 0; j < 10 ; j++)
			{
				if(s_error.compare(rev_value[j]) == 0)
  				{
					delete[] rev_value;
    					return false;
    				}
   			}
			delete[] rev_value;
			break;
		}
	}
	if(retry == 3)
	{
		return false;
	}
	return true;
}


const std::string&  DeviceNBIOTM::GEN_PUBLISH_PAYLOAD(uint8_t sensor_id)
{
	std::string ep_id[10] = { "10001", "10002", "10003", "10004", "10005", "10006", "10007", "10008", "10009", "10010"};
	RCSMessage	publish_payload(MSG_TYPE_RCS_REPORT);
	JSONNode data;
	time_t time_;
	std::string value_;

	Endpoint*	endpoint = manager_.GetEndpoint(ep_id[sensor_id]);

	data.push_back(JSONNode(TITLE_NAME_TYPE, endpoint->GetType()));
	data.push_back(JSONNode(TITLE_NAME_ID, endpoint->GetID()));
	data.push_back(JSONNode(TITLE_NAME_COUNT, "1"));
	Device* device = manager_.GetDevice("00001");
	device->ReadValue(ep_id[sensor_id], time_, value_);
	JSONNode	array(JSON_ARRAY);
	JSONNode	value;
	value.push_back(JSONNode(TITLE_NAME_TIME, ToString(time_)));
	value.push_back(JSONNode(TITLE_NAME_VALUE, value_));
	array.push_back(value);
	array.set_name(TITLE_NAME_VALUE);
	data.push_back(array);
	publish_payload.AddEPData(data);
	//delete endpoint;
	//delete device;
	//delete[] ep_id;	
	return 	publish_payload.GetPayload().write();
}


bool	DeviceNBIOTM::mqtt_publish(uint8_t sensor_id)
{
	std::string	payload = "TEST MESSAGE";
	std::string	topic = manager_.GetMqttInfo(2);
	std::string at_tcp_mqtt_publish = "AT+NSOST=";
	std::string nbiot_broker_ip = manager_.GetMqttInfo(0);
	std::string nbiot_broker_port = manager_.GetMqttInfo(1);
	std::string nbiot_broker_publish_msg ;
	uint32_t nbiot_broker_publish_msg_len = 0;
	uint8_t response[50];
	uint32_t response_len = 0;
	std::string s_error = "ERROR";
	std::string etc = "\r\n+: ";
#if 1
	//payload gen//
	std::string ep_id[10] = { "10001", "10002", "10003", "10004", "10005", "10006", "10007", "10008", "10009", "10010"};
	RCSMessage	publish_payload(MSG_TYPE_RCS_REPORT);
	JSONNode data;
	time_t time_;
	std::string value_;

	Endpoint*	endpoint = manager_.GetEndpoint(ep_id[sensor_id]);

	data.push_back(JSONNode(TITLE_NAME_TYPE, endpoint->GetType()));
	data.push_back(JSONNode(TITLE_NAME_ID, endpoint->GetID()));
	data.push_back(JSONNode(TITLE_NAME_COUNT, "1"));
	TRACE_INFO("MQTT_PUBLISH_STATE");	
	Device* device = manager_.GetDevice("00001");
	if(!device->ReadValue(endpoint->GetID(), time_, value_))
	{
		return true;
	}
	JSONNode	array(JSON_ARRAY);
	JSONNode	value;
	value.push_back(JSONNode(TITLE_NAME_TIME, ToString(time_)));
	value.push_back(JSONNode(TITLE_NAME_VALUE, value_));
	array.push_back(value);
	array.set_name(TITLE_NAME_VALUE);
	data.push_back(array);
	publish_payload.AddEPData(data);
	
#else	
	///////payload gen//////
	RCSMessage	publish_payload(MSG_TYPE_RCS_REPORT);
	JSONNode	data;
	data.push_back(JSONNode(TITLE_NAME_TYPE,"ep_s_temperature"));
	data.push_back(JSONNode(TITLE_NAME_ID,"10001"));
	data.push_back(JSONNode(TITLE_NAME_COUNT,"1"));
	JSONNode	value;
	JSONNode	array(JSON_ARRAY);
	value.push_back(JSONNode(TITLE_NAME_TIME,ToString(Date::GetCurrent())));
	value.push_back(JSONNode(TITLE_NAME_VALUE,"12.2"));
	array.push_back(value);
	array.set_name(TITLE_NAME_VALUE);
	data.push_back(array);
	publish_payload.AddEPData(data);
	std::string payload_string = GEN_PUBLISH_PAYLOAD(0);
	//std::string payload_string ="TEST MESSAGE LENGTH IS WHAT";

//	mqtt_publish_msg_gen(topic, payload, nbiot_broker_publish_msg, nbiot_broker_publish_msg_len);
#endif	
	
	std::string payload_string = publish_payload.GetPayload().write_formatted();
	at_tcp_mqtt_publish += socket_fd_+","+nbiot_broker_ip+","+nbiot_broker_port+",";
			
	TRACE_INFO(" TOPIC : " << topic);
	TRACE_INFO(" PUBLISH PAYLOAD : " << payload_string);	
	mqtt_publish_msg_gen(topic, payload_string, nbiot_broker_publish_msg, nbiot_broker_publish_msg_len);
	if(nbiot_broker_publish_msg_len == 0)
	{
		return false;
        }
	at_tcp_mqtt_publish += ToString(nbiot_broker_publish_msg_len) + "," + nbiot_broker_publish_msg + "\r\n";
	TRACE_INFO("AT COMMAND : " << at_tcp_mqtt_publish);
	uint8_t retry = 0;	
	for( ; retry < 3 ; retry++)
	{	
		RequestAndResponse((uint8_t*)at_tcp_mqtt_publish.c_str(), at_tcp_mqtt_publish.length(), response, sizeof(response), response_len, response_timeout_); 
		if(response_len == 0)
		{
			continue;
 		}
		else
		{
			std::string response_msg = (char*) response;
 			std::string* rev_value = new std::string[10];
	       		nbiot_strsplit(rev_value, response_msg, etc, 10);
			for(uint32_t j = 0; j < 10 ; j++)
 			{
    				if(s_error.compare(rev_value[j]) == 0)
  				{
					delete[] rev_value;
 					return false;
  				}
                	}
			delete[] rev_value;
			break;
		}
	}
	if(retry == 3)
	{
		return false;
	}	
	return true;
}


bool	DeviceNBIOTM::mqtt_disconnect()
{
	
	return true;
}

bool	DeviceNBIOTM::ReadValue(std::string const& _epid, time_t& _time, std::string& _value)
{

	return true;
}

void	DeviceNBIOTM::Preprocess()
{
	Date date;
	correction_interval_ = GetCorrectionInterval();
	TRACE_INFO("NBIOT DEVICE CORRECTION TIME : " << ToString(correction_interval_));
	date = Date::GetCurrent() + Time(correction_interval_ * TIME_SECOND);
	correction_timer_.Set(date);
	DeviceModbus::Preprocess();
}


void	DeviceNBIOTM::Process()
{
	float p = 0;

		if(process_state == 0)
		{
			if(reset_status())
			{
				process_state = 1;
			}
		}	
		if(process_state == 1)
		{
			if(nbiot_status())
			{
				process_state = 2;
			}
		}
		if(process_state == 2)
		{
			if(tcp_connect())
			{
				process_state = 3;
			}
			else
			{
				process_state = 0;
			}
		}
		if(process_state == 3)
		{
			if(mqtt_connect())
			{
				process_state = 4;
			}
			else
			{
				process_state = 0;
			}
		}
		if(process_state == 4)
		{
			if(correction_timer_.RemainTime() == 0)
			{	
				for(uint8_t i = 0 ; i < 10 ; i++)
				{
					if(mqtt_publish(i))
					{
						continue;
					}
					else
					{
						process_state = 0;
						break;
					}
				}
				correction_timer_.Add(correction_interval_);
			}

		}

	DeviceModbus::Process();
}
////////////////////////////////////////////////////////////////////////////////
//	Define static members
////////////////////////////////////////////////////////////////////////////////
const char*	DeviceNBIOTM::Type()
{
	return	OBJECT_TYPE_DEV_NB_IOT_M;
}


bool	DeviceNBIOTM::nbiotm_msg_gen(uint32_t size, uint8_t* message, std::string& gen_msg)
{
	uint8_t temp;

	for(uint32_t i = 0 ; i < size ; i++)
	{
		temp = message[i];
		convert_string(((temp >> 4) & 0x0f), gen_msg);
		convert_string((temp & 0x0f), gen_msg);
	}
}

bool	DeviceNBIOTM::convert_string(uint8_t hex_, std::string& save_msg)
{
	
	switch(hex_)
	{
		case 0x00 : 
			save_msg = save_msg+"0";
			break;
		case 0x01 : 
			save_msg = save_msg+"1";
			break;
		case 0x02 : 
			save_msg = save_msg+"2";
			break;
		case 0x03 :
			save_msg = save_msg+"3";
			break;
		case 0x04 : 
			save_msg = save_msg+"4";
			break;
		case 0x05 : 
			save_msg = save_msg+"5";
			break;
		case 0x06 :
		        save_msg = save_msg+"6";
			break;
		case 0x07 :
			save_msg = save_msg+"7";
			break;
		case 0x08 : 
			save_msg = save_msg+"8";
			break;
		case 0x09 :
			save_msg = save_msg+"9";
			break;
		case 0x0a : 
			save_msg = save_msg+"A";
			break;
		case 0x0b :
			save_msg = save_msg+"B";
			break;
		case 0x0c :
			save_msg = save_msg+"C";
			break;
		case 0x0d :
			save_msg = save_msg+"D";
			break;
		case 0x0e :
			save_msg = save_msg+"E";
			break;
		case 0x0f :
			save_msg = save_msg+"F";
			break;	
	}
	return true;
}

bool	DeviceNBIOTM::mqtt_connect_msg_gen(std::string& mqtt_conn_msg, uint32_t& msg_size)
{
	std::string dev_name = GetName();
	uint8_t fixedHeaderSize = 0;
	uint8_t fixed_header[3];
	uint8_t remainLen = 0;
	uint8_t flags = 0x00;
	uint8_t var_header[12] =
	{
		0x00, 0x06, 0x4d, 0x51, 0x49, 0x73, 0x64, 0x70, // Protocol name: MQIsdp 
		0x03, // Protocol version 
		0x00, // Connect flags 
		0x00, // Keep alive LSB 
		0x00  // Keep alive MSB 
	};

	uint16_t offset = 0;
	uint16_t packetSize = 0;
	uint8_t *packet = NULL;
	uint16_t clientidlen = dev_name.length(); 
	uint16_t usernamelen = 0; 
	uint16_t passwordlen = 0; 
	uint16_t payload_len = clientidlen + 2;


	// Preparing the flags 
	if (usernamelen)
	{
		payload_len += usernamelen + 2;
		flags |= MQTT_USERNAME_FLAG;
	}

	if (passwordlen)
	{
		payload_len += passwordlen + 2;
		flags |= MQTT_PASSWORD_FLAG;
	}
	if (true)
	{
		flags |= MQTT_CLEAN_SESSION;
	}
	// Variable header 
	var_header[9] = flags; // Connect flags 
	var_header[10] = 60 >> 8;   // Keep alive LSB 
	var_header[11] = 60 & 0xFF; // Keep alive MSB 

	// Fixed header 
	fixedHeaderSize = 2;    // Default size = one byte Message Type + one byte Remaining Length 
	// Message Type 
	fixed_header[0] = MQTT_MSG_CONNECT;
	// Remaining Length 
	remainLen = sizeof(var_header)+payload_len;
	if (remainLen <= 127)
	{		
		fixed_header[1] = remainLen;																							
	}
	else
	{
		fixedHeaderSize++;          // add an additional byte for Remaining Length 
		// first byte is remainder (mod) of 128, then set the MSB to indicate more bytes 
		fixed_header[1] = remainLen % 128;
		fixed_header[1] = fixed_header[1] | 0x80;
		// second byte is number of 128s 
		fixed_header[2] = remainLen / 128;
	}
	offset = 0;
	packetSize = fixedHeaderSize + sizeof(var_header)+payload_len;
	packet = (uint8_t *)malloc(packetSize);
	if (packet == NULL)
	{
	}
	memset(packet, 0, packetSize);
	memcpy(packet, fixed_header, fixedHeaderSize);
	offset += fixedHeaderSize;
	memcpy(packet + offset, var_header, sizeof(var_header));
	offset += sizeof(var_header);
	// Client ID - UTF encoded 
	packet[offset++] = clientidlen >> 8;
	packet[offset++] = clientidlen & 0xFF;
	memcpy(packet + offset, dev_name.c_str(), clientidlen);
	offset += clientidlen;
	if (usernamelen) {
		// Username - UTF encoded 
		packet[offset++] = usernamelen >> 8;
		packet[offset++] = usernamelen & 0xFF;
		//memcpy(packet + offset, broker->username, usernamelen);
		offset += usernamelen;
	}
	if (passwordlen) {
		// Password - UTF encoded 
		packet[offset++] = passwordlen >> 8;
		packet[offset++] = passwordlen & 0xFF;
		//memcpy(packet + offset, broker->password, passwordlen);
		offset += passwordlen;
	}
	msg_size = packetSize;
	nbiotm_msg_gen(packetSize, packet, mqtt_conn_msg);
	free(packet);
	return true;	
}

bool	DeviceNBIOTM::mqtt_publish_msg_gen(std::string const& topic, std::string const& payload, std::string& mqtt_pub_msg, uint32_t& mqtt_pub_msg_len)
{
	uint8_t fixedHeaderSize = 0; 
	uint16_t remainLen = 0; 
    	uint16_t packetLen = 0; 
	uint8_t *packet = NULL; 
	uint16_t topiclen = topic.length();
	uint16_t msglen = payload.length();
	uint16_t var_header_len = 0;  
	uint8_t *var_header = NULL; 
	uint8_t fixed_header[3]; 
	uint8_t qos = 0;
	uint8_t	message_id = 1;
	uint8_t	retain = 0;
	uint8_t qos_flag = MQTT_QOS0_FLAG; 
	uint8_t qos_size = 0; // No QoS included 

	if(qos == 1)  
	{ 
		qos_size = 2; // 2 bytes for QoS 
		qos_flag = MQTT_QOS1_FLAG; 
	} 
	else if(qos == 2)  
	{ 
		qos_size = 2; // 2 bytes for QoS 
		qos_flag = MQTT_QOS2_FLAG; 
	} 
	// Variable header 
	var_header_len = topiclen+2+qos_size; // Topic size (2 bytes), utf-encoded topic 
      	var_header = (uint8_t*)malloc(var_header_len);

	if (var_header == NULL) 
	{
	       	return false;	
	} 

	memset(var_header,0,var_header_len);
	var_header[0] = topiclen>>8; 
	var_header[1] = topiclen&0xFF; 
	memcpy(var_header+2, topic.c_str(), topiclen); 
	if(qos_size)  
	{ 
		var_header[topiclen+2] = 1 >>8; 
		var_header[topiclen+3] = 1 & 0xFF; 
		if(message_id) // Returning message id 
		{  
	//		*message_id = broker->seq; 
		} 
	//	broker->seq++;
	} 
	//broker->seq = broker->seq % 128;

	// Fixed header 
	// the remaining length is one byte for messages up to 127 bytes, then two bytes after that 
	// actually, it can be up to 4 bytes but I'm making the assumption the embedded device will only 
	// need up to two bytes of length (handles up to 16,383 (almost 16k) sized message) 
	fixedHeaderSize = 2;    // Default size = one byte Message Type + one byte Remaining Length 
	// Message Type, DUP flag, QoS level, Retain 
	fixed_header[0] = MQTT_MSG_PUBLISH | qos_flag; 
	if(retain)  
	{ 
		fixed_header[0] |= MQTT_RETAIN_FLAG; 
	} 

	remainLen = var_header_len+msglen; 
	// Remaining Length 
	if (remainLen <= 127)  
	{ 
		fixed_header[1] = remainLen; 
	}  
	else  
	{ 
		fixedHeaderSize++;          // add an additional byte for Remaining Length 

		// first byte is remainder (mod) of 128, then set the MSB to indicate more bytes 
		fixed_header[1] = remainLen % 128; 
		fixed_header[1] = fixed_header[1] | 0x80; 
		// second byte is number of 128s 
		fixed_header[2] = remainLen / 128; 
	} 

	packetLen = fixedHeaderSize+var_header_len+msglen; 

	packet = (uint8_t*)malloc(packetLen);
	if (packet == NULL) 
	{ 
	       return false;	
	} 
	memset(packet,0,packetLen);
	memcpy(packet, fixed_header, fixedHeaderSize); 
	memcpy(packet+fixedHeaderSize, var_header, var_header_len); 
	memcpy(packet+fixedHeaderSize+var_header_len, payload.c_str(), msglen); 

	nbiotm_msg_gen(packetLen, packet, mqtt_pub_msg);
	mqtt_pub_msg_len = packetLen;

	free(var_header);
	free(packet);
	return true;
}
