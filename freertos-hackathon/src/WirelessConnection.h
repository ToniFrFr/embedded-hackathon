/*
 * WirelessConnection.h
 *
 *  Created on: 13.12.2022
 *      Author: Mikael
 */

#ifndef WIRELESSCONNECTION_H_
#define WIRELESSCONNECTION_H_

#include <string>
#include "../networking/esp8266_socket.h"

class WirelessConnection {
public:
	WirelessConnection(char * ssid, char * password, int connectionId);
	virtual ~WirelessConnection();
	bool connectToAP();
	void disconnectFromAP();
	void changeSSID(char * ssid);
	void changePASSWORD(char * password);
	bool connectionStatus();
private:
	std::string SSID;
	std::string PASSWORD;
	int ConnectionId;
	bool ConnectedStatus;
};

#endif /* WIRELESSCONNECTION_H_ */
