/*
 * MQTTInterface.cpp
 *
 *  Created on: 13.12.2022
 *      Author: Mikael
 */

#include <MQTTInterface.h>

MQTTInterface::MQTTInterface(char * ssid, char * password, char * brokerIp, uint16_t brokerPort) : SSID(ssid), PASSWORD(password), BROKERPORT(brokerPort) {
	// TODO Auto-generated constructor stub
	this->ConnectionState = NotConnected;
	this->xNetworkContext = { 0 };
	this->BROKERPORT = brokerPort;
	this->xMQTTContext = { 0 };
}

MQTTInterface::~MQTTInterface() {
	// TODO Auto-generated destructor stub
}
void MQTTInterface::ConnectToMQTTServer(NetworkContext_t * pxNetworkContext) {
	PlaintextTransportStatus_t xNetworkStatus;
    BackoffAlgorithmStatus_t xBackoffAlgStatus = BackoffAlgorithmSuccess;
    BackoffAlgorithmContext_t xReconnectParams;
    uint16_t usNextRetryBackOff = 0U;

	BackoffAlgorithm_InitializeParams( &xReconnectParams,
                                       mqttRETRY_BACKOFF_BASE_MS,
                                       mqttRETRY_MAX_BACKOFF_DELAY_MS,
                                       mqttRETRY_MAX_ATTEMPTS );

	do {
		xNetworkStatus = Plaintext_FreeRTOS_Connect(&xNetworkContext, BROKER_IP.c_str(), SSID.c_str(), 
		PASSWORD.c_str(), BROKERPORT, mqttTRANSPORT_SEND_RECV_TIMEOUT_MS, mqttTRANSPORT_SEND_RECV_TIMEOUT_MS);

		if(xNetworkStatus != PLAINTEXT_TRANSPORT_SUCCESS) {
			xBackoffAlgStatus = BackoffAlgorithm_GetNextBackoff( &xReconnectParams, uxRand(), &usNextRetryBackOff );

            if( xBackoffAlgStatus == BackoffAlgorithmRetriesExhausted )
            {
                LogError( ( "Connection to the broker failed, all attempts exhausted." ) );
            }
            else if( xBackoffAlgStatus == BackoffAlgorithmSuccess )
            {
                LogWarn( ( "Connection to the broker failed. "
                           "Retrying connection with backoff and jitter." ) );
                vTaskDelay( pdMS_TO_TICKS( usNextRetryBackOff ) );
            }
			this->ConnectionState = NotConnected;
		} else {
			this->ConnectionState = ConnectedToServer;
		}
	} while (( xNetworkStatus != PLAINTEXT_TRANSPORT_SUCCESS ) && ( xBackoffAlgStatus == BackoffAlgorithmSuccess ));
}
void MQTTInterface::ConnectToMQTTBroker() {
	this->ConnectionState = ConnectedToBroker;

	MQTTStatus_t xResult;
	MQTTConnectInfo_t xConnectInfo;
    bool xSessionPresent;
    TransportInterface_t xTransport;

	xTransport.pNetworkContext = &xNetworkContext;
    xTransport.send = Plaintext_FreeRTOS_send;
    xTransport.recv = Plaintext_FreeRTOS_recv;

	
	
}
void MQTTInterface::DisconnectFromMQTTServer() {
	this->ConnectionState = NotConnected;
}
void MQTTInterface::ChangeAPCredentials(char * ssid, char * password) {
	this->SSID = std::string(ssid);
	this->PASSWORD = std::string(password);
}
void MQTTInterface::ChangeBrokerIPAndPort(char * brokerIp, int port) {
	this->BROKER_IP = std::string(brokerIp);
	this->BROKERPORT = port;
}


