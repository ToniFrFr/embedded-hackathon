/*
 * WirelessConnection.cpp
 *
 *  Created on: 13.12.2022
 *      Author: Mikael
 */

#include <WirelessConnection.h>

WirelessConnection::WirelessConnection(char * ssid, char * password, int connectionId) : SSID(ssid), PASSWORD(password), ConnectionId(connectionId), Connected(false) {
	// TODO Auto-generated constructor stub

}

WirelessConnection::~WirelessConnection() {
	// TODO Auto-generated destructor stub
}
bool WirelessConnection::connectToAP() {

}
void WirelessConnection::disconnectFromAP() {
	
}
