/*
 * MQTTInterface.cpp
 *
 *  Created on: 13.12.2022
 *      Author: Mikael
 */

#include "MQTTInterface.h"

extern uint32_t prvGetTimeMs( void );

static void prvEventCallback( MQTTContext_t * pxMQTTContext,
                              MQTTPacketInfo_t * pxPacketInfo,
                              MQTTDeserializedInfo_t * pxDeserializedInfo )
{
	//Needs to be NOT NULL but no functionality for publishing

}

uint32_t uxRandMQTT() {
	return rand();
}

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
			xBackoffAlgStatus = BackoffAlgorithm_GetNextBackoff( &xReconnectParams, uxRandMQTT(), &usNextRetryBackOff );

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
void MQTTInterface::ConnectToMQTTBroker(const MQTTFixedBuffer_t * pNetworkBuffer) {
	this->ConnectionState = ConnectedToBroker;

	MQTTStatus_t xResult;
	MQTTConnectInfo_t xConnectInfo;
    bool xSessionPresent;
    TransportInterface_t xTransport;

	xTransport.pNetworkContext = &xNetworkContext;
    xTransport.send = Plaintext_FreeRTOS_send;
    xTransport.recv = Plaintext_FreeRTOS_recv;

	xResult = MQTT_Init(&xMQTTContext, &xTransport, prvGetTimeMs, prvEventCallback, pNetworkBuffer);

	 /* Many fields not used in this demo so start with everything at 0. */
    //( void ) memset( ( void * ) &xConnectInfo, 0x00, sizeof( xConnectInfo ) );

    /* Start with a clean session i.e. direct the MQTT broker to discard any
     * previous session data. Also, establishing a connection with clean session
     * will ensure that the broker does not store any data when this client
     * gets disconnected. */
    xConnectInfo.cleanSession = true;

    /* The client identifier is used to uniquely identify this MQTT client to
     * the MQTT broker. In a production device the identifier can be something
     * unique, such as a device serial number. */
    xConnectInfo.pClientIdentifier = appconfigCLIENT_IDENTIFIER;
    xConnectInfo.clientIdentifierLength = ( uint16_t ) strlen( appconfigCLIENT_IDENTIFIER );

    /* Set MQTT keep-alive period. It is the responsibility of the application to ensure
     * that the interval between Control Packets being sent does not exceed the Keep Alive value.
     * In the absence of sending any other Control Packets, the Client MUST send a PINGREQ Packet. */
    xConnectInfo.keepAliveSeconds = mqttKEEP_ALIVE_TIMEOUT_SECONDS;

    /* Send MQTT CONNECT packet to broker. LWT is not used in this demo, so it
     * is passed as NULL. */
    xResult = MQTT_Connect( &xMQTTContext,
                            &xConnectInfo,
                            NULL,
                            mqttCONNACK_RECV_TIMEOUT_MS,
                            &xSessionPresent );
	if (xResult == MQTTSuccess) {
		this->ConnectionState = ConnectedToBroker;
	}
}
void MQTTInterface::DisconnectFromMQTTServer() {
	MQTT_Disconnect(&xMQTTContext);
	Plaintext_FreeRTOS_Disconnect(&xNetworkContext);
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
bool MQTTInterface::Publish(std::string topic, std::string payload) {
	MQTTStatus_t xResult;
    MQTTPublishInfo_t xMQTTPublishInfo;

	/* Some fields are not used by this demo so start with everything at 0. */
    //( void ) memset( ( void * ) &xMQTTPublishInfo, 0x00, sizeof( xMQTTPublishInfo ) );

	xMQTTPublishInfo.qos = MQTTQoS0;
	xMQTTPublishInfo.retain = false;
	xMQTTPublishInfo.pTopicName = topic.c_str();
	xMQTTPublishInfo.topicNameLength = topic.length();
	xMQTTPublishInfo.pPayload = payload.c_str();
	xMQTTPublishInfo.payloadLength = payload.length();
	
	//Packet ID can be 0 as we are using MQTTQoS0;
	xResult = MQTT_Publish(&xMQTTContext, &xMQTTPublishInfo, 0);

	if(xResult == MQTTSuccess) {
		return true;
	} else {
		return false;
	}
}


