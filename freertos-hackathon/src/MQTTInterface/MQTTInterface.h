/*
 * MQTTInterface.h
 *
 *  Created on: 13.12.2022
 *      Author: Mikael
 */

#ifndef MQTTINTERFACE_H_
#define MQTTINTERFACE_H_

#include <string>
#include "../networking/esp8266_socket.h"
#include "app_mqtt_config.h"
#include "FreeRTOS.h"
#include "transport_interface.h"

class MQTTInterface {
public:
	MQTTInterface(char * ssid, char * password, char * brokerIp, int brokerPort);
	virtual ~MQTTInterface();
	void ConnectToMQTTBroker();
	void DisconnectFromMQTTBroker();
	void ChangeAPCredentials(char * SSID, char * Password);
	void ChangeBrokerIPAndPort(char * brokerIP, int brokerPort);
private:
	std::string SSID;
	std::string PASSWORD;
	std::string BROKER_IP;
	int BROKERPORT;
	bool isConnected();
};

#endif /* MQTTINTERFACE_H_ */
