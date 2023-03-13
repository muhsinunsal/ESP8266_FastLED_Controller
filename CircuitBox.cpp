#include "CircuitBox.h"

CircuitBox::CircuitBox() {
  //Potantiometer for controlling most of box
  pinMode(POTANTIOMETER_PIN, INPUT);

  //Indicator led for virsuals
  pinMode(INDICATOR_LED_PIN, OUTPUT);
  indicatorLed = new Led(INDICATOR_LED_PIN);

  //Switch pins 1 and 2 is for Brightness/Palette/Pattern mode controls
  pinMode(MOD_CONTROLLER_SWITCH_1_PIN, INPUT_PULLUP);
  pinMode(MOD_CONTROLLER_SWITCH_2_PIN, INPUT_PULLUP);

  //Switch SECURITY_SWITCH_PIN is for Wifi/IR Remote safe switch
  pinMode(SECURITY_SWITCH_PIN, INPUT_PULLUP);
}

void CircuitBox::print() const {
  short int paletteCounterDisplay = abs(paletteCounter) % paletteCount;
  short int patternCounterDisplay = abs(patternCounter) % patternCounter;

  //Serial.printf("Brightness: %3d\tPalette: %d\tPattern: %d\n", current_Brightness , lightSensorStatus ? "-" : "+", paletteCounterDisplay, patternCounterDisplay);
  Serial.printf("Brightness: %3d%s\tPalette: %d\tPattern: %d\n", current_Brightness, lightSensorStatus ? "-" : "+", paletteCounter, patternCounter);
}

uint8_t CircuitBox::getControlSwitchStatus() const {
  bool isUp = digitalRead(MOD_CONTROLLER_SWITCH_1_PIN) && !digitalRead(MOD_CONTROLLER_SWITCH_2_PIN);
  bool isDown = !digitalRead(MOD_CONTROLLER_SWITCH_1_PIN) && digitalRead(MOD_CONTROLLER_SWITCH_2_PIN);
  bool isMiddle = digitalRead(MOD_CONTROLLER_SWITCH_1_PIN) && digitalRead(MOD_CONTROLLER_SWITCH_2_PIN);

  return isUp ? 1 : isMiddle ? 2
                  : isDown   ? 3
                             : 0;
};

bool CircuitBox::getSecuritySwitchStatus() const {
  return digitalRead(SECURITY_SWITCH_PIN);
};

void CircuitBox::announceSecuritySwitchChange() {
  uint8_t newSecuritySwitchStatus = getSecuritySwitchStatus();
  if (securitySwitchStatus != newSecuritySwitchStatus) {
    securitySwitchStatus = newSecuritySwitchStatus;
    Serial.print("Security switch switched:");
    if (securitySwitchStatus) {
      reciver.resume();
      Serial.println("ON");
    } else {
      reciver.pause();
      Serial.println("OFF");
    }
    indicatorLed->on_off(200);
  }
};

void CircuitBox::announceControlSwitchChange() {
  uint8_t newControlSwitchStatus = getControlSwitchStatus();

  if (controlSwitchStatus != newControlSwitchStatus) {
    controlSwitchStatus = newControlSwitchStatus;

    switch (getControlSwitchStatus()) {
      case 1:
        Serial.println("Potantiometer mode changed to Brightness");
        break;
      case 2:
        Serial.println("Potantiometer mode changed to Palette");
        break;
      case 3:
        Serial.println("Potantiometer mode changed to Patern");
        break;
    }

    indicatorLed->on_off(100);
  };
};

void CircuitBox::handleBrightnessChange() {
  int potValue = analogRead(POTANTIOMETER_PIN);
  int newBrightness = map(potValue, 0, 1023, 0, max_Brightness);
  if (newBrightness > max_Brightness) newBrightness = max_Brightness;

  if (current_Brightness != newBrightness) {
    current_Brightness = newBrightness;
    FastLED.setBrightness(current_Brightness);
    //Filters brightness change log
    if (!(newBrightness % 10)) {
      print();
      indicatorLed->on_off(100);
    }
  }
}

void CircuitBox::handlePatternChange() {
  int potValue = analogRead(POTANTIOMETER_PIN);
  uint8_t newPattern = map(potValue, 0, 1023, 0, patternCount - 1);
  if (newPattern != patternCounter) {
    patternCounter = newPattern;
    print();
    indicatorLed->on_off(100);
  }
}

void CircuitBox::handlePaletteChange() {
  int potValue = analogRead(POTANTIOMETER_PIN);
  int newPatternID = map(potValue, 0, 1023, 0, paletteCount - 1);
  if (paletteCounter != newPatternID) {
    paletteCounter = newPatternID;
    print();
    indicatorLed->on_off(100);
  };
  ;
}

void CircuitBox::handleLightSensor(int lightChange) {
  bool newLightSensorStatus = digitalRead(LIGHT_SENSOR_PIN);
  if (lightSensorStatus != newLightSensorStatus) {
    lightSensorStatus = newLightSensorStatus;

    if (newLightSensorStatus) {
      Serial.println("Brightness Limit Decreased.");
      //TODO Dim the brigtness slowly
      max_Brightness -= lightChange;
      current_Brightness -= lightChange;

      if (current_Brightness > max_Brightness) current_Brightness = max_Brightness;
    } else {
      Serial.println("Brightness Limit Increased.");
      max_Brightness += lightChange;
      current_Brightness += lightChange;
    }
  }
}

bool CircuitBox::didPotChange() {
  int current_PotValue = !analogRead(POTANTIOMETER_PIN) ? 1 : analogRead(POTANTIOMETER_PIN);
  int diff = abs(potValue - current_PotValue);

  if (potValue != current_PotValue) {
    potValue = current_PotValue;
  }
  return 2 <= diff;
};

void CircuitBox::modifyBrightness(short int amount) {
  if (current_Brightness + amount < 0) {
    current_Brightness = 0;
  } else if (current_Brightness + amount > max_Brightness) {
    current_Brightness = max_Brightness;
  } else {
    current_Brightness += amount;
  }
}

void CircuitBox::modifyPattern(short change) {
  patternCounter = patternCounter + change % patternCount;
}
void CircuitBox::modifyPalette(short change) {
  paletteCounter = paletteCounter + change % paletteCount;
}
