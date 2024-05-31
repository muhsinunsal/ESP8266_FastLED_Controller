#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Hash.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <FS.h>

#include <FastLED.h>

#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

#include "CircuitBox.h"
#include "IRremoteCodes.h"
#include "configs.h"
#include "utilty.h"

CircuitBox box;
CRGB leds[300];

IRrecv irrecv(IR_SENSOR_PIN);
decode_results results;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "tr.pool.ntp.org");

ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      //Serial.printf("[%s] ", timeClient.getFormattedTime().c_str());
      Serial.printf("[%u] Disconnected!\n", num);
      break;

    case WStype_CONNECTED:
      {
        box.indicatorLed->on_off(300);
        //Serial.printf("[%s] ", timeClient.getFormattedTime().c_str());
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        // send message to client
        sendStartMessage(num, payload, length);
        sendUpdateMessage(num, payload, length, "startMessage");

        webSocket.sendTXT(num, String(num).c_str());
      }
      break;

    case WStype_TEXT:
      IPAddress ip = webSocket.remoteIP(num);

      //Check if security switch is on
      if (box.getSecuritySwitchStatus()) {
        //Swtich turned on
        Serial.printf("[%u] Request from %d.%d.%d.%d is blocked by security switch.\n", num, ip[0], ip[1], ip[2], ip[3]);
      } else {
        //Swtich turned off

        //Serial.printf("[%s] [%u] ", timeClient.getFormattedTime().c_str(), num);

        handleWebSocketEventTXT(num, payload, length);
        // send message to client
        // webSocket.sendTXT(num, "message here");
        // send data to all connected clients
        // webSocket.broadcastTXT("message here");
      }
      break;
  }
}
typedef struct {
  CRGB RGB;
  CHSV HSV;
} Colorpackage;

Colorpackage startingWebColor = { CRGB(255, 255, 255), CHSV(0, 0, 100) };

Colorpackage lastWebColor = startingWebColor;

float lastAlpha = box.max_Brightness;

typedef void (*Pattern)();
//typedef Pattern PatternList[];
typedef struct {
  Pattern pattern;
  String name;
} PatternAndName;
typedef PatternAndName PatternAndNameList[];

typedef struct {
  TProgmemRGBPalette16 palette;
  String name;
} PaletteAndName;
typedef PaletteAndName PaletteAndNameList[];

template<typename T>
void solid_Color(T colorElement) {
  fill_solid(leds, LED_NUMBER, colorElement);
}

void black() {
  solid_Color(CRGB::Black);
}

//Fill_grad by Andrew Tuline: https://github.com/atuline/FastLED-Demos/blob/master/fill_grad/fill_grad.ino
void fill_grad() {

  uint8_t starthue = beatsin8(5, 0, 255);
  uint8_t endhue = beatsin8(7, 0, 255);

  if (starthue < endhue) {
    fill_gradient(leds, LED_NUMBER, CHSV(starthue, 255, 255), CHSV(endhue, 255, 255), FORWARD_HUES);  // If we don't have this, the colour fill will flip around.
  } else {
    fill_gradient(leds, LED_NUMBER, CHSV(starthue, 255, 255), CHSV(endhue, 255, 255), BACKWARD_HUES);
  }
}

//Noise16_2 by Andrew Tuline: https://github.com/atuline/FastLED-Demos/blob/master/noise16_2/noise16_2.ino
void noise16_2() {

  uint8_t scale = 1000;

  for (uint16_t i = 0; i < LED_NUMBER; i++) {

    uint16_t shift_x = millis() / 10;
    uint16_t shift_y = 0;

    uint32_t real_x = (i + shift_x) * scale;
    uint32_t real_y = (i + shift_y) * scale;
    uint32_t real_z = 4223;

    uint8_t noise = inoise16(real_x, real_y, real_z) >> 8;

    uint8_t index = sin8(noise * 3);
    uint8_t bri = noise;

    leds[i] = ColorFromPalette(box.currentPalette, index, bri, LINEARBLEND);
  }
}

//Sawtooth by Andrew Tuline: https://github.com/atuline/FastLED-Demos/blob/master/fill_grad/fill_grad.ino
void sawtooth() {

  int bpm = 20;
  int ms_per_beat = 60000 / bpm;
  int ms_per_led = 60000 / bpm / LED_NUMBER;

  int cur_led = ((millis() % ms_per_beat) / ms_per_led) % (LED_NUMBER);

  if (cur_led == 0)
    fill_solid(leds, LED_NUMBER, CRGB::Black);
  else
    //leds[cur_led] = ColorFromPalette(box.currentPalette, 0, 255, box.currentBlending);
    leds[cur_led] = lastWebColor.RGB;
}
//Simple plain red
void redColor() {
  solid_Color(CRGB::Red);
}
//Web controlled colors
void webColor() {
  solid_Color(lastWebColor.RGB);
}

PatternAndNameList patterns = {
  { black, "Black" },
  { fill_grad, "Fill Gradient" },
  { noise16_2, "Noise16_2" },
  { sawtooth, "Sawtooth" },
  { redColor, "Red" },
  { webColor, "Solid Color" }
};

const CRGBPalette16 palettes[] = {
  RainbowColors_p,
  RainbowStripeColors_p,
  CloudColors_p,
  LavaColors_p,
  OceanColors_p,
  ForestColors_p,
  PartyColors_p,
  HeatColors_p
};

const String palette_names[] = {
  "Rainbow",
  "Rainbow Stripes",
  "Cloud",
  "Lava",
  "Ocean",
  "Forest",
  "Party",
  "Heat"
};


void handleWebSocketEventTXT(uint8_t num, uint8_t *payload, size_t length) {
  //ColorPackageObject package(str);
  StaticJsonDocument<256> doc;
  //Serial.printf("[%u] get Text: %S\n", num, payload);
  DeserializationError error = deserializeJson(doc, (const char *)payload, length);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  // Type
  const String type = doc["type"];

  if (type == "updateMessage") {
    // Color
    JsonObject rgba = doc["rgba"];
    JsonObject hsv = doc["hsv"];
    lastWebColor.RGB = CRGB(rgba["r"], rgba["g"], rgba["b"]);
    lastAlpha = rgba["a"];
    //Serial.printf("Color: | R: %-3d G: %-3d B: %-3d A: %-3.2f| H: %-3d S: %-3d V: %-3d \n", (uint8_t)rgba["r"], (uint8_t)rgba["g"], (uint8_t)rgba["b"], (float)rgba["a"], (uint8_t)hsv["h"], (uint8_t)hsv["s"], (uint8_t)hsv["v"]);

    // Mapped alpha , dividing by 8 to smooth
    uint8_t mapped_alpha = map((float)rgba["a"] * 100, 0, 100, 0, box.max_Brightness);
    box.current_Brightness = mapped_alpha;
    FastLED.setBrightness(mapped_alpha);


    // Pattern Index
    int patternIndex = doc["patternIndex"];
    Serial.printf("Pattern set to: %d - %s\n",patternIndex, palette_names[patternIndex]);
    box.patternCounter = patternIndex;

    // Palette Index
    int paletteIndex = doc["paletteIndex"];
    Serial.printf("Palette set to: %d - %s\n",paletteIndex, palette_names[paletteIndex]);
    box.paletteCounter = paletteIndex;

    // Broadcast update
    for (uint8_t i = 0; i < webSocket.connectedClients(); i++) {
      if (i != num) {
        Serial.print("Sent to %d");
        Serial.println(i);
        sendUpdateMessage(i, payload, length, type);
      }
    }
  } else if (type == "startMessage") {
    // ESP8266 can't recive startMessage
  }
}


void startWifi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  WiFi.hostname("ESP8266_FastLED_Controller");
  Serial.println("");
  Serial.print("Connecting to WiFi ");
  while (WiFi.status() != WL_CONNECTED) {
    delay(50);
    Serial.print(".");
    box.indicatorLed->toggle();
  }

  delay(1500);
  box.indicatorLed->turnOff();
  Serial.println(" connected.\n");
  Serial.print("Can connect with: ");
  Serial.print("http://");
  Serial.println(WiFi.localIP());
  startmDNS();
}

void startmDNS() {
  if (MDNS.begin(DNS_URL)) {

    Serial.print("Can also connect from: ");
    Serial.print("http://");
    Serial.print(DNS_URL);
    Serial.print(".local\n");
  }
  MDNS.addService("http", "tcp", 80);
}

void tickServer() {
  MDNS.update();
  server.handleClient();
  webSocket.loop();
}

void sendUpdateMessage(uint8_t num, uint8_t *payload, size_t length, String type) {
  String output;
  StaticJsonDocument<256> doc;

  doc["type"] = "updateMessage";
  doc["senderID"] = num;  // ?? num doğru olmayabilir
  JsonObject rgba = doc.createNestedObject("rgba");
  rgba["r"] = lastWebColor.RGB.red;
  rgba["g"] = lastWebColor.RGB.green;
  rgba["b"] = lastWebColor.RGB.blue;
  rgba["a"] = lastAlpha;

  JsonObject hsv = doc.createNestedObject("hsv");
  hsv["h"] = lastWebColor.HSV.hue;
  hsv["s"] = lastWebColor.HSV.saturation;
  hsv["v"] = lastWebColor.HSV.value;

  doc["patternIndex"] = box.patternCounter;
  doc["paletteIndex"] = box.paletteCounter;

  serializeJson(doc, output);
  webSocket.sendTXT(num, output.c_str(), strlen(output.c_str()));

  // Serial.println(output.c_str());
}

void sendStartMessage(uint8_t num, uint8_t *payload, size_t length) {
  String output;
  StaticJsonDocument<512> doc;

  doc["type"] = "startMessage";
  doc["assignedID"] = num;

  JsonArray patternList = doc.createNestedArray("patternList");
  JsonArray paletteList = doc.createNestedArray("paletteList");

  for (int i = 0; i < box.patternCount; i++) {
    patternList.add(patterns[i].name);
  }

  for (int i = 0; i < box.paletteCount; i++) {
    paletteList.add(palette_names[i]);
  }

  serializeJson(doc, output);

  webSocket.sendTXT(num, output.c_str());
}

void handleIRremote() {
  if (irrecv.decode(&results)) {
    static unsigned long key_value = 0;
    if (results.value == 0xFFFFFFFF) {
      results.value = key_value;
    }
    key_value = results.value;
    // Serial.println(key_value, HEX);
    switch (results.value) {
      //Keys and signals differs from remote. Update accordingly to own requirements
      //This link can help you to find right signals for your own remote: https://github.com/crankyoldgit/IRremoteESP8266/tree/master/examples
      case R_BTN1:
        Serial.println("IR Remote Button 1");
        break;
      case R_BTN2:
        Serial.println("IR Remote Button 2");
        break;
      case R_BTN3:
        Serial.println("IR Remote Button 3");
        break;
      case R_BTN4:
        Serial.println("IR Remote Button 4");
        break;
      case R_BTN5:
        Serial.println("IR Remote Button 5");
        break;
      case R_BTN6:
        Serial.println("IR Remote Button 6");
        break;
      case R_BTN7:
        Serial.println("IR Remote Button 7");
        break;
      case R_BTN8:
        Serial.println("IR Remote Button 8");
        break;
      case R_BTN9:
        Serial.println("IR Remote Button 9");
        break;
      case R_BTN0:
        Serial.println("IR Remote Button 0");
        break;
      case R_BTN_STAR:
        Serial.println("IR Remote Button *");
        box.modifyPalette(+1);
        box.print();
        break;
      case R_BTN_SHARP:
        Serial.println("IR Remote Button #");
        box.modifyPalette(-1);
        box.print();
        break;
      case R_BTN_UP:
        Serial.println("IR Remote Button Up");
        box.modifyBrightness(+10);
        box.print();
        break;

      case R_BTN_LEFT:
        Serial.println("IR Remote Button Left");
        box.modifyPattern(1);
        box.print();
        break;

      case R_BTN_RIGHT:
        Serial.println("IR Remote Button Right");
        box.modifyPattern(-1);
        box.print();
        break;

      case R_BTN_DOWN:
        Serial.println("IR Remote Button Down");
        box.modifyBrightness(-10);
        box.print();
        break;

      case R_BTN_OK:
        Serial.println("IR Remote Button Ok");
        box.indicatorLed->toggle();
        box.print();
        break;
    }
    irrecv.resume();
  }
}

//Main function to controll switches and potentiometer.
void handleCircuitBox() {
  if (box.didPotChange()) {
    switch (box.getControlSwitchStatus()) {
      case 1:
        box.handleBrightnessChange();
        break;
      case 2:
        box.handlePaletteChange();
        break;
      case 3:
        box.handlePatternChange();
        break;
    }
    //**************************************************************************************************************
    //Has an delay because it runs faster then switch is fliped
    EVERY_N_MILLISECONDS(300) {
      box.announceControlSwitchChange();
    }
  };
}

void setup() {
  Serial.begin(115200);

  //Update this line to your own equipment. WS2812 usually has color order GRB
  FastLED.addLeds<WS2812, LED_STRIP_PIN, GRB>(leds, LED_NUMBER);

  irrecv.enableIRIn();

  //Updating current pattern and palette count after initiation
  box.patternCount = sizeof(patterns) / sizeof((patterns)[0]);
  box.paletteCount = sizeof(palettes) / sizeof((palettes)[0]);

  box.targetPalette = palettes[random(0, box.paletteCount)];

  FastLED.setBrightness(box.max_Brightness);
  FastLED.setMaxPowerInVoltsAndMilliamps(MAXVOLT, MAXMILLIAMP);


  startWifi();
  timeClient.begin();
  SPIFFS.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  server.onNotFound([]() {
    if (!handleFileRead(server.uri()))
      handleNotFound();
  });

  server.begin();
};

void handleNotFound() {
  server.send(404, "text/html", "<html><body><p style=\"color:red;font-weight:bold;\">404 Error</p></body></html>");
}

String getContentType(String filename) {
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  return "text/plain";
}

bool handleFileRead(String path) {
  if (path.endsWith("/")) path += "index.html";
  String contentType = getContentType(path);
  if (SPIFFS.exists(path)) {
    File file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, contentType);
    file.close();
    return true;
  }
  Serial.println("handleFileRead: " + path);
  Serial.println("\tFile Not Found");
  return false;
}

void loop() {
  handleCircuitBox();

  tickServer();

  box.announceSecuritySwitchChange();
  if (box.getSecuritySwitchStatus()) {
    handleIRremote();
  }

  //Check the environment every 5 seconds for updates.
  EVERY_N_SECONDS(1) {
    box.handleLightSensor(20);
  }

  //Serial.println(FastLED.getBrightness());

  //Updates target palette according the  operations done to paletteCounter
  box.targetPalette = palettes[box.paletteCounter];

  //Blend a bit each 50 millisecond to next color palette.
  EVERY_N_MILLISECONDS(50) {
    nblendPaletteTowardPalette(box.currentPalette, box.targetPalette, 24);
  }

  //Run current led pattern.
  patterns[box.patternCounter].pattern();


  FastLED.show();
};
