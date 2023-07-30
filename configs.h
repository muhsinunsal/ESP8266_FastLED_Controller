/////////////////////////////////////////
// UPDATE THIS FILE BEFORE UPLOAD!!!!! //
/////////////////////////////////////////

#ifndef CONFIGS
#define CONFIGS

//Not every pin can be used
//You can check wether it is compatible from here: https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/
#define POTANTIOMETER_PIN A0
#define INDICATOR_LED_PIN D0
#define MOD_CONTROLLER_SWITCH_1_PIN D1
#define MOD_CONTROLLER_SWITCH_2_PIN D2
#define SECURITY_SWITCH_PIN D5
#define IR_SENSOR_PIN D7
#define LIGHT_SENSOR_PIN D6
#define LED_STRIP_PIN D8

#define MAXVOLT 5
#define MAXMILLIAMP 2000

#define LED_NUMBER 300 // 60 leds per meter * 5 meter
#define MAX_MAX_BRIGHTNESS 255

//Variable for false commands
#define POTANTIMETER_SENSIVITY_PERCENTAGE 5

#define WIFI_SSID "WIFI_NAME"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define DNS_URL "esp8266_fastled"

#endif