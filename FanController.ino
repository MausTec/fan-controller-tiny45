#include <OneButton.h>

// Pin Config
#define FAN_PWM_PIN PB4
#define SPEED_BTN_PIN PB3

// Misc Settings
#define SPEED_INTERVAL 64
#define SPEED_MAX     255
#define SPEED_HIGH    SPEED_MAX - (1 * SPEED_INTERVAL)
#define SPEED_MEDIUM  SPEED_MAX - (2 * SPEED_INTERVAL)
#define SPEED_LOW     SPEED_MAX - (3 * SPEED_INTERVAL)
#define SPEED_OFF     0

// Globals
OneButton Speed(SPEED_BTN_PIN, HIGH, false);
uint8_t currentSpeed = SPEED_OFF;

void initTimer1(void) {
  TCCR1 = 0x03 << COM1A0;

  // Timer 1 prescaler is 4 bits wide. At 8MHz:
  // 0x0 = ~ 15.4 KHz
  // 0x7 = ~ 963 Hz
  // 0xA = ~ 120 Hz
  // 0xC = ~ 30 Hz    <- This seems to balance rattle + whine
  // 0xF = < 10Hz
  TCCR1 |= 0xC << CS10;
}

void initButtons(void) {
  Speed.attachDoubleClick(fanMax);
  Speed.attachClick(cycleSpeed);
  Speed.attachLongPressStart(fanOff);
}

void cycleSpeed(void) {
  // Turn Off
  if (currentSpeed == 0) {
    currentSpeed = SPEED_MAX;
  // Slow Down
#ifdef SPEED_LOW
  } else if (currentSpeed <= SPEED_LOW) {
    currentSpeed = 0;
  } else if (currentSpeed <= SPEED_MEDIUM) {
    currentSpeed = SPEED_LOW;
#else
  } else if (currentSpeed <= SPEED_MEDIUM) {
    currentSpeed = 0;
#endif
  } else if (currentSpeed <= SPEED_HIGH) {
    currentSpeed = SPEED_MEDIUM;
  } else if (currentSpeed <= SPEED_MAX) {
    currentSpeed = SPEED_HIGH;
  }

  setSpeed(currentSpeed);
}

void fanMax(void) {
  currentSpeed = SPEED_MAX;
  setSpeed(currentSpeed);
}

void fanOff(void) {
  currentSpeed = 0;
  setSpeed(currentSpeed);
  Speed.reset();
}

void setSpeed(uint8_t speed) {
  analogWrite(FAN_PWM_PIN, speed);
}

void setup() {
  // Slow the prescaler:
  initTimer1();

  // Init Pins
  pinMode(FAN_PWM_PIN, OUTPUT);
  pinMode(SPEED_BTN_PIN, INPUT);

  // Init Buttons
  initButtons();
  
  setSpeed(currentSpeed);
}

void loop() {
  Speed.tick();
}
