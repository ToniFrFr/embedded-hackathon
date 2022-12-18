/*
 * app_mqtt_config.h
 *
 *  Created on: 13.12.2022
 *      Author: Mikael
 */

#ifndef APP_MQTT_CONFIG_H_
#define APP_MQTT_CONFIG_H_

/**************************************************/
/******* DO NOT CHANGE the following order ********/
/**************************************************/

/* Include logging header files and define logging macros in the following order:
 * 1. Include the header file "logging_levels.h".
 * 2. Define the LIBRARY_LOG_NAME and LIBRARY_LOG_LEVEL macros depending on
 * the logging configuration for DEMO.
 * 3. Include the header file "logging_stack.h", if logging is enabled for DEMO.
 */

#include "logging_levels.h"

/* Logging configuration for the Demo. */
#ifndef LIBRARY_LOG_NAME
    #define LIBRARY_LOG_NAME    "MQTTDemo"
#endif

#ifndef LIBRARY_LOG_LEVEL
    #define LIBRARY_LOG_LEVEL    LOG_INFO
#endif

/* Prototype for the function used to print to console on Windows simulator
 * of FreeRTOS.
 * The function prints to the console before the network is connected;
 * then a UDP port after the network has connected. */
extern void vLoggingPrintf( const char * pcFormatString,
                            ... );

/* Map the SdkLog macro to the logging function to enable logging
 * on Windows simulator. */
#ifndef SdkLog
    //#define SdkLog( message )    vLoggingPrintf message
#endif

#include "logging_stack.h"

/************ End of logging configuration ****************/

#include "FreeRTOS.h"
#include "task.h"

/**
 * @brief The MQTT client identifier used in this example.  Each client identifier
 * must be unique so edit as required to ensure no two clients connecting to the
 * same broker use the same client identifier.
 *
 *!!! Please note a #defined constant is used for convenience of demonstration
 *!!! only.  Production devices can use something unique to the device that can
 *!!! be read by software, such as a production serial number, instead of a
 *!!! hard coded constant.
 *
 * #define democonfigCLIENT_IDENTIFIER				"insert here."
 */


//#define appconfigTEST_ENV
#ifdef appconfigTEST_ENV

/**
 * @brief MQTT client identifier constant value
 * 
 */
#define appconfigCLIENT_IDENTIFIER				"ESP-MQTT-GROUP-08"

/**
 * @brief MQTT Broker IP address constant
 * 
 */
#define appconfigMQTT_BROKER_ENDPOINT				"192.168.65.42"

/**
 * @brief WiFi access point SSID for testing purposes
 * 
 */
#define WIFI_SSID	    "OPMIKAEL"
/**
 * @brief WiFi access point password for testing purposes
 * 
 */
#define WIFI_PASS       "pellemiljoona"

/**
 * @brief MQTT Topic value for testing purposes
 * 
 */

#define appconfigMQTT_TOPIC "test/values"

/**
 * @brief The test MQTT Broker's port
 * 
 */
#define appconfigMQTT_BROKER_PORT					1883

/**
 * @brief The interval time between MQTT topic publishes from the device
 * 
 */
#define appconfigMQTT_SEND_INTERVAL 5000

#else

/**
 * @brief The demo/main setup MQTT Broker endpoint
 * 
 */
#define appconfigMQTT_BROKER_ENDPOINT "mqtt3.thingspeak.com"

/**
 * @brief The demo setup WiFi access point SSID
 * 
 */
#define WIFI_SSID	    "SmartIotMQTT"
/**
 * @brief The demo setup WiFi access point password
 * 
 */
#define WIFI_PASS       "SmartIot"

/**
 * @brief The demo setup MQTT Topic
 * 
 */
#define appconfigMQTT_TOPIC "channels/1955513/publish"

/**
 * @brief The demo setup channel ID
 * 
 */
#define CHANNEL_ID "1955513"
/**
 * @brief Demo setup MQTT client username
 * 
 */
#define appconfigSECRET_MQTT_USERNAME "DCAmDzgFFhoKKy8kCBw3NQA"
/**
 * @brief Demo setup MQTT client ID
 * 
 */
#define appconfigCLIENT_IDENTIFIER "DCAmDzgFFhoKKy8kCBw3NQA"
/**
 * @brief Demo setup MQTT Client password 
 * 
 */
#define appconfigSECRET_MQTT_PASSWORD "qKxqzEzD+xMf2LMg0SU24WYk"

/**
 * @brief Demo setup MQTT Broker port
 * 
 */

#define appconfigMQTT_BROKER_PORT					1883

/**
 * @brief Demo setup time interval between MQTT publishes 
 * 
 */

#define appconfigMQTT_SEND_INTERVAL 300000

#endif


#endif /* APP_MQTT_CONFIG_H_ */
