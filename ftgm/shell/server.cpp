#include <iomanip>
#include <fstream>
#include "shell_ftgm.h"
#include "gateway.h"
#include "endpoint.h"
#include "gateway_gen.h"
#include "object_manager.h"
#include "trace.h"
#include "exception.h"

RetValue	SubCommandKeepAlive
(
	const std::vector<std::string>&	_arguments,
	Shell*			_shell,
	Shell::Command*	_this
)
{
	RetValue	ret_value = RET_VALUE_OK;

	ObjectManager*	object_manager = (ObjectManager*)_shell->GetObject();

	if(_arguments.size() != 2 )
	{
		ThrowInvalidArgument("The keep_alive state is missing.");
	}
	else if(_arguments[1] == "on")
	{
		object_manager->GetServerLinker().KeepAliveEnable(true);
	}
	else if(_arguments[1] == "off")
	{
		object_manager->GetServerLinker().KeepAliveEnable(false);
	}
	else
	{
		ThrowInvalidArgument(_arguments[1]);
	}

	return	ret_value;
}


RetValue	SubCommandLink
(
	const std::vector<std::string>&	_arguments,
	Shell*			_shell,
	Shell::Command*	_this
)
{
	RetValue	ret_value = RET_VALUE_OK;

	ObjectManager*	object_manager = (ObjectManager*)_shell->GetObject();
	ServerLinkerMosq&	linker  = object_manager->GetServerLinker();

	std::vector<ServerLinkerMosq::UpLink*>	up_link_array;

	uint32_t	link_count = linker.GetUpLink(up_link_array);
	std::cout << std::setw(40) << "TOPIC" << std::setw(16) << "Live Time" << std::setw(16) << "OUT GOING" <<  std::setw(16) << "ERROR" << std::endl;
	for(uint32_t i = 0 ; i < link_count ; i++)
	{
		std::cout << std::setw(40) <<  up_link_array[i]->GetTopic();
		std::cout << std::setw(16) <<  up_link_array[i]->GetLiveTime();
		std::cout << std::setw(16) <<  up_link_array[i]->NumberOfOutGoingMessages();
		std::cout << std::setw(16) <<  up_link_array[i]->NumberOfErrorMessages() << std::endl;
	}

	std::vector<ServerLinkerMosq::DownLink*>	down_link_array;

	link_count = linker.GetDownLink(down_link_array);
	std::cout << std::setw(40) << "TOPIC" << std::setw(16) << "Live Time" << std::setw(16) << "INCOMMING" <<  std::setw(16) << "ERROR" << std::endl;
	for(uint32_t i = 0 ; i < link_count ; i++)
	{
		std::cout << std::setw(40) <<  down_link_array[i]->GetTopic();
		std::cout << std::setw(16) <<  down_link_array[i]->GetLiveTime();
		std::cout << std::setw(16) <<  down_link_array[i]->NumberOfIncommingMessages();
		std::cout << std::setw(16) <<  down_link_array[i]->NumberOfErrorMessages() << std::endl;
	}

	return	ret_value;
}


RetValue	SubCommandReport
(
	const std::vector<std::string>&	_arguments,
	Shell*			_shell,
	Shell::Command*	_this
)
{
	RetValue	ret_value = RET_VALUE_OK;

	ObjectManager*	object_manager = (ObjectManager*)_shell->GetObject();

	std::map<std::string, Endpoint*>	endpoint_map;

	for(uint32_t i = 2 ; i < _arguments.size() ; )
	{
		if (_arguments[i] == "--gw")
		{
			for(i++; i < _arguments.size() ; i++)
			{
				if (_arguments[i] == "all")
				{
					std::list<Gateway*>	gateway_list;

				
					object_manager->GetGatewayList(gateway_list);

					for(std::list<Gateway*>::iterator it = gateway_list.begin(); it != gateway_list.end() ; it++)
					{
						std::map<std::string, Device*>	device_map;

						(*it)->GetDeviceMap(device_map);

						for(std::map<std::string, Device*>::iterator device_it = device_map.begin() ; device_it != device_map.end() ; device_it++)
						{
							std::map<std::string, Endpoint*>	endpoint_map;

							device_it->second->GetEndpointMap(endpoint_map);

							for(std::map<std::string, Endpoint*>::iterator endpoint_it = endpoint_map.begin() ; endpoint_it != endpoint_map.end() ; endpoint_it++)
							{
								endpoint_map[endpoint_it->first] = endpoint_it->second;
							}
						}
					}
				}
				else if (_arguments[i].substr(0,2) == "--")
				{
					break;
				}
				else
				{
					Gateway* gateway = object_manager->GetGateway(_arguments[i]);
					if (gateway == NULL)
					{
						std::ostringstream	oss;

						oss << "Gateway[" << _arguments[i] << "] not found!";
						throw std::invalid_argument(oss.str());
					}

					std::map<std::string, Device*>	device_map;

					gateway->GetDeviceMap(device_map);
					for(std::map<std::string, Device*>::iterator device_it = device_map.begin() ; device_it != device_map.end() ; device_it++)
					{
						std::map<std::string, Endpoint*>	endpoint_map;

						device_it->second->GetEndpointMap(endpoint_map);

						for(std::map<std::string, Endpoint*>::iterator endpoint_it = endpoint_map.begin() ; endpoint_it != endpoint_map.end() ; endpoint_it++)
						{
							endpoint_map[endpoint_it->first] = endpoint_it->second;
						}
					}
				}
			}
		}
		else if (_arguments[i] == "--dev")
		{
			for(i++; i < _arguments.size() ; i++)
			{
				if (_arguments[i] == "all")
				{
					std::list<Device*>	device_list;

					object_manager->GetDeviceList(device_list);

					for(std::list<Device*>::iterator device_it = device_list.begin(); device_it != device_list.end() ; device_it++)
					{
						std::map<std::string, Endpoint*>	endpoint_map;

						(*device_it)->GetEndpointMap(endpoint_map);

						for(std::map<std::string, Endpoint*>::iterator endpoint_it = endpoint_map.begin() ; endpoint_it != endpoint_map.end() ; endpoint_it++)
						{
							endpoint_map[endpoint_it->first] = endpoint_it->second;
						}
					}
				}
				else if (_arguments[i].substr(0,2) == "--")
				{
					break;
				}
				else
				{
					Device* device = object_manager->GetDevice(_arguments[i]);
					if (device == NULL)
					{
						std::ostringstream	oss;

						oss << "Device[" << _arguments[i] << "] not found!";
						throw std::invalid_argument(oss.str());
					}

					std::map<std::string, Endpoint*>	endpoint_map;

					device->GetEndpointMap(endpoint_map);

					for(std::map<std::string, Endpoint*>::iterator endpoint_it = endpoint_map.begin() ; endpoint_it != endpoint_map.end() ; endpoint_it++)
					{
						endpoint_map[endpoint_it->first] = endpoint_it->second;
					}
				}
			}
		}
		else if (_arguments[i] == "--ep")
		{
			for(i++; i < _arguments.size() ; i++)
			{
				if (_arguments[i] == "all")
				{
					std::list<Endpoint*>	endpoint_list;

					object_manager->GetEndpointList(endpoint_list);

					for(std::list<Endpoint*>::iterator it = endpoint_list.begin(); it != endpoint_list.end() ; it++)
					{
						endpoint_map[(*it)->GetID()] = *it;
					}
				}
				else if (_arguments[i].substr(0,2) == "--")
				{
					break;
				}
				else
				{
					Endpoint* endpoint = object_manager->GetEndpoint(_arguments[i]);
					if (endpoint == NULL)
					{
						std::ostringstream	oss;

						oss << "Endpoint[" << _arguments[i] << "] not found!";
						throw std::invalid_argument(oss.str());
					}

					endpoint_map[endpoint->GetID()] = endpoint;
				}
			}
		}
		else 
		{
			std::ostringstream	oss;

			oss << "Endpoint[" << _arguments[i] << "] not found!";
			throw std::invalid_argument(oss.str());

		}
	}

	RCSMessage	message(MSG_TYPE_RCS_REPORT);

	//			for(auto it = endpoint_map.begin(); it != endpoint_map.end() ; it++)
	//			{
	//				object_manager->GetServerLinker().AddEPData(payload, it->second);	
	//			}

	object_manager->GetServerLinker().Send(message);

	return	ret_value;
}
 
RetValue	ShellCommandServerLinker
(
	const std::vector<std::string>&	_arguments,
	Shell*			_shell,
	Shell::Command*	_this
)
{
	RetValue	ret_value = RET_VALUE_OK;
	ObjectManager*	object_manager = (ObjectManager*)_shell->GetObject();
	ServerLinkerMosq&	linker  = object_manager->GetServerLinker();

	TRACE_INFO("Check point");
	if (object_manager == NULL)
	{
		std::cout << "Object manager not attached!" << std::endl;	
	}
	else if (_arguments.size() < 2)
	{
		ShowUsage(_this->Usage());
	}
	else
	{
		Shell::Command*	parent = _shell->GetCurrentCommand();

		for(int i = 0 ; i < parent->sub.size() ; i++)
		{
			if (_arguments[1] == parent->sub[i]->name)
			{
				std::vector<std::string> sub_arguments = _arguments;

				sub_arguments.erase(sub_arguments.begin());

				return	parent->sub[i]->Call(sub_arguments, _shell);
			}
		}

		if (_arguments.size() >= 4)
		{
			RCSMessage	payload;
			Fields	fields;

			if ((_arguments[1] == MSG_TYPE_RCS_ADD) || (_arguments[1] == MSG_TYPE_RCS_SET))
			{
				payload.SetMsgType(_arguments[1]);	
				fields.Set(PROPERTY_STATIC_FLAG);
			}
			else if ((_arguments[1] == MSG_TYPE_RCS_DEL) || (_arguments[1] == MSG_TYPE_RCS_GET))
			{
				payload.SetMsgType(_arguments[1]);	
				fields = PROPERTY_ID;
			}
			else
			{
				std::ostringstream	oss;

				oss << "Unknown command[" << _arguments[1] << "]";
				throw std::invalid_argument(oss.str());
			}

			for(uint32_t i = 2 ; i < _arguments.size() ; )
			{
				if (_arguments[i] == "--property")
				{
					fields = PROPERTY_ID;

					for(i++; i < _arguments.size() ; i++)
					{
						if (_arguments[i].substr(0,2) == "--")
						{
							break;
						}
						else
						{
							if (!fields.Set(_arguments[i]))
							{
								std::ostringstream	oss;

								oss << "Invalid property[" << _arguments[i] << "]!";
								throw std::invalid_argument(oss.str());
							}
						}
					}
				}
				else if (_arguments[i] == "--gw")
				{
					for(i++; i < _arguments.size() ; i++)
					{
						if (_arguments[i] == "all")
						{
							std::list<Gateway*>	gateway_list;

							object_manager->GetGatewayList(gateway_list);

							for(std::list<Gateway*>::iterator it = gateway_list.begin(); it != gateway_list.end() ; it++)
							{
								JSONNode	properties;

								(*it)->GetProperties(properties, fields);

								payload.AddGateway(properties);
							}
						}
						else if (_arguments[i].substr(0,2) == "--")
						{
							break;
						}
						else
						{
							Gateway* gateway = object_manager->GetGateway(_arguments[i]);
							if (gateway == NULL)
							{
								std::ostringstream	oss;

								oss << "Gateway[" << _arguments[i] << "] not found!";
								throw std::invalid_argument(oss.str());
							}

							JSONNode	properties;

							gateway->GetProperties(properties, fields);

							payload.AddGateway(properties);
						}
					}
				}
				else if (_arguments[i] == "--dev")
				{
					for(i++; i < _arguments.size() ; i++)
					{
						if (_arguments[i] == "all")
						{
							std::list<Device*>	device_list;

							object_manager->GetDeviceList(device_list);

							for(std::list<Device*>::iterator it = device_list.begin(); it != device_list.end() ; it++)
							{
								JSONNode	properties;

								(*it)->GetProperties(properties, fields);

								payload.AddDevice(properties);
							}
						}
						else if (_arguments[i].substr(0,2) == "--")
						{
							break;
						}
						else
						{
							Device* device = object_manager->GetDevice(_arguments[i]);
							if (device == NULL)
							{
								std::ostringstream	oss;

								oss << "Gateway[" << _arguments[i] << "] not found!";
								throw std::invalid_argument(oss.str());
							}

							JSONNode	properties;

							device->GetProperties(properties, fields);

							payload.AddDevice(properties);
						}
					}
				}
				else if (_arguments[i] == "--ep")
				{
					for(i++; i < _arguments.size() ; i++)
					{
						if (_arguments[i] == "all")
						{
							std::list<Endpoint*>	endpoint_list;

							object_manager->GetEndpointList(endpoint_list);

							for(std::list<Endpoint*>::iterator it = endpoint_list.begin(); it != endpoint_list.end() ; it++)
							{
								JSONNode	properties;

								(*it)->GetProperties(properties, fields);

								payload.AddEndpoint(properties);
							}
						}
						else if (_arguments[i].substr(0,2) == "--")
						{
							break;
						}
						else
						{
							Endpoint* endpoint = object_manager->GetEndpoint(_arguments[i]);
							if (endpoint == NULL)
							{
								std::ostringstream	oss;

								oss << "Endpoint[" << _arguments[i] << "] not found!";
								throw std::invalid_argument(oss.str());
							}

							JSONNode	properties;

							endpoint->GetProperties(properties, fields);

							payload.AddEndpoint(properties);
						}
					}
				}
				else if (_arguments[i] == "--rule")
				{
					for(i++; i < _arguments.size() ; i++)
					{
						if (_arguments[i] == "all")
						{
							std::list<Rule*>	rule_list;

							object_manager->GetRuleList(rule_list);

							for(std::list<Rule*>::iterator it = rule_list.begin(); it != rule_list.end() ; it++)
							{
								payload.AddRule(JSONNode(*(*it)));
							}
						}
						else if (_arguments[i].substr(0,2) == "--")
						{
							break;
						}
						else
						{
							Rule* rule = object_manager->GetRule(_arguments[i]);
							if (rule == NULL)
							{
								std::ostringstream	oss;

								oss << "Endpoint[" << _arguments[i] << "] not found!";
								throw std::invalid_argument(oss.str());
							}

							JSONNode	properties;

							rule->GetProperties(properties, fields);

							payload.AddRule(properties);
						}
					}
				}
				else if (_arguments[i] == "--data")
				{
					for(i++; i < _arguments.size() ; i++)
					{
						if (_arguments[i] == "all")
						{
							std::list<Endpoint*>	endpoint_list;

							object_manager->GetEndpointList(endpoint_list);

							for(std::list<Endpoint*>::iterator it = endpoint_list.begin(); it != endpoint_list.end() ; it++)
							{
								JSONNode	properties;

								(*it)->GetProperties(properties, fields);

								payload.AddGateway(properties);
							}
						}
						else if (_arguments[i].substr(0,2) == "--")
						{
							break;
						}
						else
						{
							Endpoint* endpoint = object_manager->GetEndpoint(_arguments[i]);
							if (endpoint == NULL)
							{
								std::ostringstream	oss;

								oss << "Endpoint[" << _arguments[i] << "] not found!";
								throw std::invalid_argument(oss.str());
							}
							//object_manager->GetServerLinker().AddEPData(payload, endpoint);
						}
					}
				}
				else
				{
					std::ostringstream	oss;

					oss << "Invalid argument[" << _arguments[i] << "]";
					throw std::invalid_argument(oss.str());
				}
			}

			payload.Make();

			object_manager->GetServerLinker().Send(payload);
		}
		else
		{
			throw ShowUsage(_this->Help());
		}
	}

	return	ret_value;
}


Shell::Command	shell_ftgm_command_slc_sub[] = 
{
	Shell::Command("keep_alive", "<command>\n", "", SubCommandKeepAlive),
	Shell::Command("link", "<command>\n", "", SubCommandLink),
	Shell::Command("report", "<command>\n", "", SubCommandReport)
};

Shell::Command	shell_ftgm_command_slc
(
 "slc",
 "<command> \n"
 "  Server linker command.\n"
 "COMMANDS:\n"
 "  add <TYPE> <ID>\n"
 "    Registration gateway\n"
 "  get <TYPE> <ID>\n"
 "    Get get object information.\n"
 "  link\n"
 "    Show produce/subscribe status.\n"
 "  info\n"
 "    Get server linker information.\n"
 "PARAMETERS:\n"
 "  TYPE    Type of object\n"
 "    gw    Gateway\n"
 "    dev   Device\n"
 "    ep    Endpoint\n"
 "    rule  Rule\n"
 "  ID      Id of object\n",
 "Management of gateway",
 ShellCommandServerLinker,
 shell_ftgm_command_slc_sub,
 sizeof(shell_ftgm_command_slc_sub) / sizeof(Shell::Command)

	);
