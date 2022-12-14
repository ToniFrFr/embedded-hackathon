/*
 * MQTTInterface.cpp
 *
 *  Created on: 13.12.2022
 *      Author: Mikael
 */

#include "MQTTInterface.h"


static void prvEventCallback( MQTTContext_t * pxMQTTContext,
                              MQTTPacketInfo_t * pxPacketInfo,
                              MQTTDeserializedInfo_t * pxDeserializedInfo )
{
	//Needs to be NOT NULL but no functionality for publishing

}

uint32_t uxRandMQTT() {
	return rand();
}

MQTTInterface::MQTTInterface(char * ssid, char * password, char * brokerIp, uint16_t brokerPort) : SSID(ssid), PASSWORD(password), BROKER_IP(brokerIp) {
	// TODO Auto-generated constructor stub
	this->ConnectionState = NotConnected;
	this->BROKERPORT = brokerPort;
}

MQTTInterface::~MQTTInterface() {
	// TODO Auto-generated destructor stub
}
bool MQTTInterface::ConnectToMQTTServer(NetworkContext_t *pxNetworkContext) {
	PlaintextTransportStatus_t xNetworkStatus;
    BackoffAlgorithmStatus_t xBackoffAlgStatus = BackoffAlgorithmSuccess;
    BackoffAlgorithmContext_t xReconnectParams;
    uint16_t usNextRetryBackOff = 0U;

	BackoffAlgorithm_InitializeParams( &xReconnectParams,
                                       mqttRETRY_BACKOFF_BASE_MS,
                                       mqttRETRY_MAX_BACKOFF_DELAY_MS,
                                       mqttRETRY_MAX_ATTEMPTS );

	do {
		xNetworkStatus = Plaintext_FreeRTOS_Connect(pxNetworkContext, BROKER_IP.c_str(), SSID.c_str(), 
		PASSWORD.c_str(), BROKERPORT, mqttTRANSPORT_SEND_RECV_TIMEOUT_MS, mqttTRANSPORT_SEND_RECV_TIMEOUT_MS);

		if(xNetworkStatus != PLAINTEXT_TRANSPORT_SUCCESS) {
			xBackoffAlgStatus = BackoffAlgorithm_GetNextBackoff( &xReconnectParams, uxRandMQTT(), &usNextRetryBackOff );

            if( xBackoffAlgStatus == BackoffAlgorithmRetriesExhausted )
            {
                LogError( ( "Connection to the broker failed, all attempts exhausted." ) );
				printf("Connection to broker failed on all attempts \n");
            }
            else if( xBackoffAlgStatus == BackoffAlgorithmSuccess )
            {
                LogWarn( ( "Connection to the broker failed. "
                           "Retrying connection with backoff and jitter." ) );
				printf("Connection to broker failed, trying again \n");
                vTaskDelay( pdMS_TO_TICKS( usNextRetryBackOff ) );
            }
			this->ConnectionState = NotConnected;
		} else {
			this->ConnectionState = ConnectedToServer;
		}
	} while (( xNetworkStatus != PLAINTEXT_TRANSPORT_SUCCESS ) && ( xBackoffAlgStatus == BackoffAlgorithmSuccess ));

	if(ConnectionState == ConnectedToServer) {
		printf("Connected to server\n");
		return true;
	} else {
		printf("Connection to server failed...\n");
		return false;
	}
}
bool MQTTInterface::ConnectToMQTTBroker(MQTTFixedBuffer_t * pNetworkBuffer, MQTTContext_t *pxMQTTContext, NetworkContext_t * pxNetworkContext) {
	MQTTStatus_t xResult;
	MQTTConnectInfo_t xConnectInfo;
    bool xSessionPresent;
    TransportInterface_t xTransport;

	xTransport.pNetworkContext = pxNetworkContext;
    xTransport.send = Plaintext_FreeRTOS_send;
    xTransport.recv = Plaintext_FreeRTOS_recv;

	printf("Starting MQTT_Init()..\n");

	xResult = MQTT_Init(pxMQTTContext, &xTransport, prvGetTimeMs, prvEventCallback, pNetworkBuffer);

	if(xResult != MQTTSuccess) {
		printf("Failed init, something wrong with init parameters...\n");
		return false;
	}

	printf("MQTT_Init successful...\n");

	 /* Many fields not used in this demo so start with everything at 0. */
    ( void ) memset( ( void * ) &xConnectInfo, 0x00, sizeof( xConnectInfo ) );
    xConnectInfo.cleanSession = true;
    xConnectInfo.pClientIdentifier = appconfigCLIENT_IDENTIFIER;
    xConnectInfo.clientIdentifierLength = ( uint16_t ) strlen( appconfigCLIENT_IDENTIFIER );
    xConnectInfo.keepAliveSeconds = mqttKEEP_ALIVE_TIMEOUT_SECONDS;

	#ifndef appconfigTEST_ENV
	xConnectInfo.pUserName = appconfigSECRET_MQTT_USERNAME;
	xConnectInfo.userNameLength = ( uint16_t ) strlen(appconfigSECRET_MQTT_USERNAME);
	xConnectInfo.pPassword = appconfigSECRET_MQTT_PASSWORD;
	xConnectInfo.passwordLength = ( uint16_t ) strlen(appconfigSECRET_MQTT_PASSWORD); 
	#endif

	printf("Starting MQTT_Connect()....\n");
    xResult = MQTT_Connect( pxMQTTContext,
                            &xConnectInfo,
                            NULL,
                            mqttCONNACK_RECV_TIMEOUT_MS,
                            &xSessionPresent );
	if (xResult == MQTTSuccess) {
		this->ConnectionState = ConnectedToBroker;
		printf("Connection to broker successful\n");
		return true;
	} else if(xResult == MQTTBadParameter) {
		printf("Bad parameters with MQTT connect\n");
		return false;
	} else if(xResult == MQTTSendFailed) {
		printf("MQTT connect send failed\n");
		return false;
	}  else if(xResult == MQTTRecvFailed) {
		printf("MQTT connect receive failed\n");
		return false;
	} else if(xResult == MQTTNoMemory) {
		printf("MQTT connect no memory\n");
		return false;
	} else if(xResult == MQTTNoDataAvailable) {
		printf("MQTT connect no data available\n");
		return false;
	} else {
		printf("Connection to broker failed\n");
		return false;
	}
}
bool MQTTInterface::DisconnectFromMQTTServer(MQTTContext_t *pxMQTTContext, NetworkContext_t * pxNetworkContext) {
	MQTTStatus_t xMQTTStatus;
	PlaintextTransportStatus_t xNetworkStatus;
	xMQTTStatus = MQTT_Disconnect(pxMQTTContext);
	xNetworkStatus = Plaintext_FreeRTOS_Disconnect(pxNetworkContext);
	
	if(xMQTTStatus == MQTTSuccess && xNetworkStatus == PLAINTEXT_TRANSPORT_SUCCESS) {
		printf("Disconnect success \n");
		return true;
	} else {
		printf("Disconnect failed \n");
		return false;
	}
	
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
bool MQTTInterface::Publish(std::string topic, std::string payload, MQTTContext_t *pxMQTTContext) {
	MQTTStatus_t xResult;
    MQTTPublishInfo_t xMQTTPublishInfo;

	/* Some fields are not used by this demo so start with everything at 0. */
    ( void ) memset( ( void * ) &xMQTTPublishInfo, 0x00, sizeof( xMQTTPublishInfo ) );

	xMQTTPublishInfo.qos = MQTTQoS0;
	xMQTTPublishInfo.retain = false;
	xMQTTPublishInfo.pTopicName = topic.c_str();
	xMQTTPublishInfo.topicNameLength = topic.length();
	xMQTTPublishInfo.pPayload = payload.c_str();
	xMQTTPublishInfo.payloadLength = payload.length();
	
	//Packet ID can be 0 as we are using MQTTQoS0;
	xResult = MQTT_Publish(pxMQTTContext, &xMQTTPublishInfo, 0);

	if(xResult == MQTTSuccess) {
		return true;
	} else {
		return false;
	}
}
std::string MQTTInterface::GeneratePublishPayload(int co2, int rh, int temp, bool valveState, uint32_t setpoint) {
	int valveStateValue = valveState ? 100 : 0;
	std::string payload = "field1=" + std::to_string(co2) + "&field2=" + std::to_string(rh) + "&field3=" + std::to_string(temp) + "&field4=" + std::to_string(valveStateValue) + "&field5=" + std::to_string(setpoint);
	return payload;
}

