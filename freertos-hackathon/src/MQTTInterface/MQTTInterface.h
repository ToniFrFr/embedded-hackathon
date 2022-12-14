/*
 * MQTTInterface.h
 *
 *  Created on: 13.12.2022
 *      Author: Mikael
 */

#ifndef MQTTINTERFACE_H_
#define MQTTINTERFACE_H_

#include <string>
#include <string.h>
#include "../networking/esp8266_socket.h"
#include "app_mqtt_config.h"
#include "FreeRTOS.h"
#include "task.h"
#include "chip.h"

extern "C" {
	#include "transport_interface.h"
	#include "core_mqtt.h"
	#include "backoff_algorithm.h"
	#include "library_plaintext.h"
}

/**
 * @brief The maximum number of retries for network operation with server.
 */
#define mqttRETRY_MAX_ATTEMPTS            ( 5U )

/**
 * @brief The maximum back-off delay (in milliseconds) for retrying failed operation
 *  with server.
 */
#define mqttRETRY_MAX_BACKOFF_DELAY_MS    ( 5000U )

/**
 * @brief The base back-off delay (in milliseconds) to use for network operation retry
 * attempts.
 */
#define mqttRETRY_BACKOFF_BASE_MS         ( 500U )


/**
 * @brief Timeout for receiving CONNACK packet in milliseconds.
 */
#define mqttCONNACK_RECV_TIMEOUT_MS           ( 1000U )


/**
 * @brief The number of topic filters to subscribe.
 */
#define mqttTOPIC_COUNT                       ( 1 )

/**
 * @brief Dimensions a file scope buffer currently used to send and receive MQTT data
 * from a socket.
 */
#define mqttSHARED_BUFFER_SIZE                ( 500U )

/**
 * @brief Timeout for MQTT_ProcessLoop in milliseconds.
 */
#define mqttPROCESS_LOOP_TIMEOUT_MS           ( 500U )

/**
 * @brief Keep alive time reported to the broker while establishing an MQTT connection.
 *
 * It is the responsibility of the Client to ensure that the interval between
 * Control Packets being sent does not exceed the this Keep Alive value. In the
 * absence of sending any other Control Packets, the Client MUST send a
 * PINGREQ Packet.
 */
#define mqttKEEP_ALIVE_TIMEOUT_SECONDS        ( 60U )

/**
 * @brief Delay between MQTT publishes. Note that the process loop also has a
 * timeout, so the total time between publishes is the sum of the two delays.
 */
#define mqttDELAY_BETWEEN_PUBLISHES           ( pdMS_TO_TICKS( 2000U ) )

/**
 * @brief Transport timeout in milliseconds for transport send and receive.
 */
#define mqttTRANSPORT_SEND_RECV_TIMEOUT_MS    ( 200U )

#define MILLISECONDS_PER_SECOND                      ( 1000U )                                        /**< @brief Milliseconds per second. */
#define MILLISECONDS_PER_TICK                        ( MILLISECONDS_PER_SECOND / configTICK_RATE_HZ ) /**< Milliseconds per FreeRTOS tick. */

/*-----------------------------------------------------------*/

/**
 * @brief Each compilation unit that consumes the NetworkContext must define it.
 * It should contain a single pointer to the type of your desired transport.
 * When using multiple transports in the same compilation unit, define this pointer as void *.
 *
 * @note Transport stacks are defined in FreeRTOS-Plus/Source/Application-Protocols/network_transport.
 */
struct NetworkContext
{
    PlaintextTransportParams_t * pParams;
};

enum MQTTIntefaceConnectionState {NotConnected, ConnectedToServer, ConnectedToBroker};

extern uint32_t prvGetTimeMs( void );

class MQTTInterface {
public:
	MQTTInterface(char * ssid, char * password, char * brokerIp, uint16_t brokerPort);
	virtual ~MQTTInterface();
	bool ConnectToMQTTServer(NetworkContext_t * pxNetworkContext);
	bool ConnectToMQTTBroker(MQTTFixedBuffer_t * pNetworkBuffer, MQTTContext_t *pxMQTTContext, NetworkContext_t * pxNetworkContext);
	bool DisconnectFromMQTTServer(MQTTContext_t *pxMQTTContext, NetworkContext_t * pxNetworkContext);
	void ChangeAPCredentials(char * ssid, char * password);
	void ChangeBrokerIPAndPort(char * brokerIP, int brokerPort);
	bool Publish(std::string topic, std::string payload, MQTTContext_t *pxMQTTContext);
	std::string GeneratePublishPayload(int co2, int rh, int temp, int valveState, uint32_t setpoint);
private:
	std::string SSID;
	std::string PASSWORD;
	std::string BROKER_IP;
	uint16_t BROKERPORT;
	MQTTIntefaceConnectionState ConnectionState;
};

#endif /* MQTTINTERFACE_H_ */
