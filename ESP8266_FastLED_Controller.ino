#include <FastLED.h>

#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

#include "CircuitBox.h"
#include "IRremoteCodes.h"
#include "configs.h"

CircuitBox box;
CRGB leds[300];

IRrecv irrecv(IR_SENSOR_PIN);
decode_results results;

typedef void (*Pattern)();
typedef Pattern PatternList[];
typedef struct {
  Pattern pattern;
  String name;
} PatternAndName;
typedef PatternAndName PatternAndNameList[];

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
    leds[cur_led] = ColorFromPalette(box.currentPalette, 0, 255, box.currentBlending);
}
//Simple plain red
void plainRed() {
  solid_Color(CRGB::Red);
}

PatternAndNameList patterns = {
  { black, "Black" },
  { fill_grad, "Fill Gradient" },
  { noise16_2, "Noise16_2" },
  { sawtooth, "Sawtooth" },
  { plainRed, "Plain Red" },
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
  box.paletteCount = sizeof(patterns) / sizeof((patterns)[0]);

  box.targetPalette = palettes[random(0, box.paletteCount)];

  FastLED.setBrightness(box.max_Brightness);
  FastLED.setMaxPowerInVoltsAndMilliamps(MAXVOLT, MAXMILLIAMP);
};


void loop() {
  handleCircuitBox();

  box.announceSecuritySwitchChange();
  if (box.getSecuritySwitchStatus()) {
    handleIRremote();
  }

  //Check the environment every 5 seconds for updates.
  EVERY_N_SECONDS(5) {
    box.handleLightSensor(20);
  }
  
  //Updates target palette according the  operations done to paletteCounter
  box.targetPalette = palettes[box.paletteCounter];

  //Blend a bit each 50 millisecond to next color pallete.
  EVERY_N_MILLISECONDS(50) {
    nblendPaletteTowardPalette(box.currentPalette, box.targetPalette, 24);
  }
  //Run current led pattern.
  patterns[box.patternCounter].pattern();

  FastLED.show();
};
