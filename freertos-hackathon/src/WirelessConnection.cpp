/*
 * WirelessConnection.cpp
 *
 *  Created on: 13.12.2022
 *      Author: Mikael
 */

#include <WirelessConnection.h>

WirelessConnection::WirelessConnection(char * ssid, char * password, int connectionId) : SSID(ssid), PASSWORD(password), ConnectionId(connectionId), ConnectedStatus(false) {
	// TODO Auto-generated constructor stub

}

WirelessConnection::~WirelessConnection() {
	// TODO Auto-generated destructor stub
}
bool WirelessConnection::connectToAP() {
	int errorCode = esp_socket(SSID.c_str(), PASSWORD.c_str());

	if(errorCode == 0) {
		ConnectedStatus = true;
	} else {
		ConnectedStatus = false;
	}

	return ConnectedStatus;
}
void WirelessConnection::disconnectFromAP() {
	esp_shutdown(ConnectionId, 1);
}
void WirelessConnection::changeSSID(char * ssid) {
	this->SSID = std::string(ssid);
}
void WirelessConnection::changePASSWORD(char * password) {
	this->PASSWORD = std::string(password);
}
bool WirelessConnection::connectionStatus() {
	return this->ConnectedStatus;
}