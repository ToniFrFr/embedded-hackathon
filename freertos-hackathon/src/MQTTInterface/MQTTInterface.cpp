/*
 * MQTTInterface.cpp
 *
 *  Created on: 13.12.2022
 *      Author: Mikael
 */

#include <MQTTInterface.h>

MQTTInterface::MQTTInterface(char * ssid, char * password, char * brokerIp, int brokerPort) : SSID(ssid), PASSWORD(password), BROKER_IP(brokerIp), BROKERPORT(brokerPort) {
	// TODO Auto-generated constructor stub

}

MQTTInterface::~MQTTInterface() {
	// TODO Auto-generated destructor stub
}

