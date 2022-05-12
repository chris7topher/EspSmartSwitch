/*
  user_config_override.h - user configuration overrides my_user_config.h for Tasmota

  Copyright (C) 2021  Theo Arends

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _USER_CONFIG_OVERRIDE_H_
#define _USER_CONFIG_OVERRIDE_H_

/*****************************************************************************************************\
 * USAGE:
 *   To modify the stock configuration without changing the my_user_config.h file:
 *   (1) copy this file to "user_config_override.h" (It will be ignored by Git)
 *   (2) define your own settings below
 *
 ******************************************************************************************************
 * ATTENTION:
 *   - Changes to SECTION1 PARAMETER defines will only override flash settings if you change define CFG_HOLDER.
 *   - Expect compiler warnings when no ifdef/undef/endif sequence is used.
 *   - You still need to update my_user_config.h for major define USE_MQTT_TLS.
 *   - All parameters can be persistent changed online using commands via MQTT, WebConsole or Serial.
\*****************************************************************************************************/

/*
Examples :

// -- Master parameter control --------------------
#undef  CFG_HOLDER
#define CFG_HOLDER        4617                   // [Reset 1] Change this value to load SECTION1 configuration parameters to flash

// -- Setup your own Wifi settings  ---------------
#undef  STA_SSID1
#define STA_SSID1         "YourSSID"             // [Ssid1] Wifi SSID

#undef  STA_PASS1
#define STA_PASS1         "YourWifiPassword"     // [Password1] Wifi password
*/
// -- Setup your own MQTT settings  ---------------
#undef  MQTT_HOST
#define MQTT_HOST         "192.168.178.99" // [MqttHost]

#undef  MQTT_PORT
#define MQTT_PORT         1883                   // [MqttPort] MQTT port (10123 on CloudMQTT)

#undef  MQTT_USER
#define MQTT_USER         "enter_your_your_user_here"         // [MqttUser] Optional user

#undef  MQTT_PASS
#define MQTT_PASS         "enter_your_password_here"         // [MqttPassword] Optional password

/*
// You might even pass some parameters from the command line ----------------------------
// Ie:  export PLATFORMIO_BUILD_FLAGS='-DUSE_CONFIG_OVERRIDE -DMY_IP="192.168.1.99" -DMY_GW="192.168.1.1" -DMY_DNS="192.168.1.1"'

#ifdef MY_IP
#undef  WIFI_IP_ADDRESS
#define WIFI_IP_ADDRESS     MY_IP                // Set to 0.0.0.0 for using DHCP or enter a static IP address
#endif

#ifdef MY_GW
#undef  WIFI_GATEWAY
#define WIFI_GATEWAY        MY_GW                // if not using DHCP set Gateway IP address
#endif

#ifdef MY_DNS
#undef  WIFI_DNS
#define WIFI_DNS            MY_DNS               // If not using DHCP set DNS IP address (might be equal to WIFI_GATEWAY)
#endif

#ifdef MY_DNS2
#undef  WIFI_DNS2
#define WIFI_DNS2           MY_DNS2              // If not using DHCP set DNS IP address (might be equal to WIFI_GATEWAY)
#endif

// !!! Remember that your changes GOES AT THE BOTTOM OF THIS FILE right before the last #endif !!!
*/

// Passwords
#undef STA_SSID1
#define STA_SSID1      "YOUR_SSID"

#undef STA_PASS1
#define STA_PASS1      "YOUR_WIFI_PASSWORD"


// Temperatur Sensor
#define USE_AHT1x

// Template
#define USER_TEMPLATE "{\"NAME\":\"SmartSwitch\",\"GPIO\":[418,416,36,417,640,608,1,1,34,35,33,1,96,1],\"FLAG\":0,\"BASE\":18}"

#define USE_WEBSEND_RESPONSE

//HAAS
#undef HOME_ASSISTANT_DISCOVERY_ENABLE
#define HOME_ASSISTANT_DISCOVERY_ENABLE   false

// Use only singe press for fast response, if hold is needed this must be set to false (setoption13)
#undef KEY_ONLY_SINGLE_PRESS  false
#define KEY_ONLY_SINGLE_PRESS   true

#undef FALLBACK_MODULE
#define FALLBACK_MODULE        USER_MODULE

#undef FRIENDLY_NAME
#define FRIENDLY_NAME          "SmartSwitch" 

#undef KEY_HOLD_TIME
#define KEY_HOLD_TIME 10

#undef MQTT_BUTTON_TOPIC
#define MQTT_BUTTON_TOPIC      "1"  

#undef MQTT_CLIENT_ID
#define MQTT_CLIENT_ID "SmartSwitch_%06X"

// Try to disable firmware reset
#undef KEY_DISABLE_MULTIPRESS
#define KEY_DISABLE_MULTIPRESS true

// Disable other temp sensors
#ifdef USE_ADE7953
#undef USE_ADE7953                              
#endif 

#undef MQTT_BUTTONS                              
#define MQTT_BUTTONS true

#ifdef MQTT_SWITCHES
#undef MQTT_SWITCHES                              
#define MQTT_SWITCHES true
#endif 

#ifdef USE_VEML6075
#undef USE_VEML6075                              
#endif 

#undef NTP_SERVER1    
#define NTP_SERVER1 "192.168.178.99"                          

#undef NTP_SERVER2      
#define NTP_SERVER2 ""                        

#undef NTP_SERVER3      
#define NTP_SERVER3 ""                        

#ifdef USE_DOMOTICZ
#undef USE_DOMOTICZ                              
#endif 

#ifdef USE_HOME_ASSISTANT
#undef USE_HOME_ASSISTANT                              
#endif 

#endif  // _USER_CONFIG_OVERRIDE_H_
