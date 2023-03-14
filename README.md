# ESP8266_FastLED_Controller
Control addressable led strip with <a href="https://github.com/FastLED/FastLED">FastLed</a> via web browser , remote or switches

## Features:
- **Adjust Brightness, Change Display Pattern, Adjust the Color** 
- **Physical Controll** using button, potantiomer and switches
- **Auto Brightness Controll** using lightsensor to adjust environment
- **Remote Controll**
- **Controll over Wifi** using ESP8266's wifi capabilities (*TODO*)

## Hardware:

- **ESP8266 development board**
- **Addressable LED strip** (This project made using WS2812)
- **5v Power Adapter** (Could use phone chargers but not recommended for long led strips at bright settings) more Amps makes it more bright
- 2x <b>3-way Switch</b></li>
- **Potantiometer** (*ohm value don't matter*)
- **Button**
- **Switch** for general power
- **Light sensor module**
- **Infrared Reciver** and **Remote**
- Single **LED** and **220 ohm resistor**
  Recommended by [Adafruit NeoPixel "Best Practices" ](https://learn.adafruit.com/adafruit-neopixel-uberguide/best-practices) to help protect LEDs from current onrush
- **1000µF Capacitor**
- **300 to 500 Ohm resistor**
