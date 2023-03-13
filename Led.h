//Led class for giving virsual information
class Led {
private:
  uint8_t pinNumber;
  int status;
  unsigned long previousMillis = 0;
public:
  Led(uint_fast8_t pin)
    : pinNumber(pin) {
    pinMode(pinNumber, OUTPUT);
  };
  void turnOn() {
    status = HIGH;
    digitalWrite(pinNumber, status);
  };
  void turnOff() {
    status = LOW;
    digitalWrite(pinNumber, status);
  };
  void toggle() {
    digitalWrite(pinNumber, !status);
    status = !status;
  }
  // !!! BLOCKS Progress use carefully!!!
  void on_off(int millisecond) {
    turnOn();
    delay(millisecond);
    turnOff();
  }
  bool getStatus() {
    return status;
  }
};