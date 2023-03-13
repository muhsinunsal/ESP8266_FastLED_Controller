#include <FastLED.h>
#include "Led.h"
#include "configs.h"

//Class for general circuit control
class CircuitBox {
private:
  uint8_t controlSwitchStatus;
  bool securitySwitchStatus;
  int potValue;
  //Light Sensor Module used at digital mode (not analog!!!)
  bool lightSensorStatus;

public:
  Led *indicatorLed;

  uint8_t patternCount;
  uint8_t paletteCount;

  uint8_t patternCounter = 0;
  uint8_t paletteCounter = 0;

  uint8_t max_Brightness = MAX_MAX_BRIGHTNESS;
  short int current_Brightness;

  CRGBPalette16 currentPalette;
  CRGBPalette16 targetPalette;

  TBlendType currentBlending;

  CircuitBox();
  bool didPotChange();
  void handleBrightnessChange();
  void handlePaletteChange();
  void handlePatternChange();
  void handleLightSensor(int);
  uint8_t getControlSwitchStatus() const;
  bool getSecuritySwitchStatus() const;
  void announceControlSwitchChange();
  void announceSecuritySwitchChange();
  void print() const;
  void modifyBrightness(short);
  void modifyPattern(short);
  void modifyPalette(short);
  void changePattern(int id);
  void changePalette(int id);
};
