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

/**
 * @brief Milliseconds per second.
 * 
 */
#define MILLISECONDS_PER_SECOND                      ( 1000U )
/**
 * @brief Milliseconds per FreeRTOS tick.
 * 
 */
#define MILLISECONDS_PER_TICK                        ( MILLISECONDS_PER_SECOND / configTICK_RATE_HZ )

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
/**
 * @brief Used for status and error reporting from the MQTTInterface class
 * 
 */
enum MQTTIntefaceConnectionState {NotConnected, ConnectedToServer, ConnectedToBroker};

/**
 * @brief Function implemented in main, used for time handling with the network related functions (such as MQTT_Connect(), esp_socket()....)
 * 
 * @return uint32_t return time in ms
 */
extern uint32_t prvGetTimeMs( void );

/**
 * @brief MQTTInterface class used to handle and implement the MQTT functionality, such as connecting to the access point and MQTT broker and
 * publishing to MQTT channel
 * 
 */
class MQTTInterface {
public:
	/**
	 * @brief Construct a new MQTTInterface object
	 * 
	 * @param ssid WiFi AP SSID
	 * @param password WiFi AP password
	 * @param brokerIp MQTT Broker IP address
	 * @param brokerPort MQTT Broker connection port
	 */
	MQTTInterface(char * ssid, char * password, char * brokerIp, uint16_t brokerPort);
	/**
	 * @brief Destroy the MQTTInterface object
	 * 
	 */
	virtual ~MQTTInterface();
	/**
	 * @brief Method to connect to the MQTTServer, connects first to the WiFi Access Point and then creates connection to the server using ip address and port
	 * 
	 * @param pxNetworkContext Struct used to store network settings
	 * @return true if connection successful
	 * @return false if connection unsuccessful 
	 */
	bool ConnectToMQTTServer(NetworkContext_t * pxNetworkContext);
	/**
	 * @brief Method to start connection with the MQTT Broker, method should be only called after the ConnectToMQTTServer() has finished successfully
	 * 
	 * @param pNetworkBuffer Pointer to a buffer to hold the MQTT network traffic and responses etc.
	 * @param pxMQTTContext Sturct to hold information about the MQTT broker, such as IP, port, username, etc, these parameters are set by the method
	 * @param pxNetworkContext Struct that holds the current network connection information
	 * @return true if connection is successful
	 * @return false if connection is unsuccessful
	 */
	bool ConnectToMQTTBroker(MQTTFixedBuffer_t * pNetworkBuffer, MQTTContext_t *pxMQTTContext, NetworkContext_t * pxNetworkContext);
	/**
	 * @brief Method to disconnect from the MQTT server, method cut offs the connection to MQTT broker and server hosting the MQTT broker
	 * 
	 * @param pxMQTTContext struct that holds the current active information regarding the MQTT Broker connection
	 * @param pxNetworkContext struct that holds the current information regarding the network connection the access point
	 * @return true if disconnect successful
	 * @return false if disconnect unsuccessful
	 */
	bool DisconnectFromMQTTServer(MQTTContext_t *pxMQTTContext, NetworkContext_t * pxNetworkContext);
	/**
	 * @brief Method to change the class's private member SSID and PASSWORD values, before values are changed, DisconnectFROMMQTTServer()-should be called and
	 * after the new credentials are changed, ConnectToMQTTServer() should be called
	 * 
	 * @param ssid new WiFi Access Point SSID
	 * @param password new WiFi Access Point Password
	 */
	void ChangeAPCredentials(char * ssid, char * password);
	/**
	 * @brief Method to change the class's private members BROKER_IP and BROKERPORT, before values are changed, DisconnectFROMMQTTServer()-should be called and
	 * after the new credentials are changed, ConnectToMQTTServer() should be called
	 * 
	 * @param brokerIP new MQTT Broker IP address
	 * @param brokerPort new MQTT Broker port
	 */
	void ChangeBrokerIPAndPort(char * brokerIP, int brokerPort);
	/**
	 * @brief Publish a given message to a given topic when connected to the MQTT broker 
	 * 
	 * @param topic topic where the payload will be published
	 * @param payload the payload message to be published
	 * @param pxMQTTContext struct that holds the current context and information regarding the MQTT 
	 * @return true if publish successful
	 * @return false if publish unsuccessful
	 */
	bool Publish(std::string topic, std::string payload, MQTTContext_t *pxMQTTContext);
	/**
	 * @brief Method to create the payload string for MQTT Publish that publishes the data from sensor values
	 * 
	 * @param co2 co2 ppm value 
	 * @param rh relative humiditiy value
	 * @param temp temperature value
	 * @param valveState the solenoid valve state, false for closed, true for open
	 * @param setpoint the current value that the systems automatical control is upkeeping 
	 * @return std::string the message to be published
	 */
	std::string GeneratePublishPayload(int co2, int rh, int temp, bool valveState, uint32_t setpoint);
private:
	/**
	 * @brief the member to hold the given WiFi SSID
	 * 
	 */
	std::string SSID;
	/**
	 * @brief the member to hold the given WiFi Access Point password
	 * 
	 */
	std::string PASSWORD;
	/**
	 * @brief the member to hold the given IP address of the MQTT broker
	 * 
	 */
	std::string BROKER_IP;
	/**
	 * @brief the member to hold the given port of the MQTT broker
	 * 
	 */
	uint16_t BROKERPORT;
	/**
	 * @brief the member to hold current state of MQTTInterface connection, value handled by methods of the class 
	 * 
	 */
	MQTTIntefaceConnectionState ConnectionState;
};

#endif /* MQTTINTERFACE_H_ */
