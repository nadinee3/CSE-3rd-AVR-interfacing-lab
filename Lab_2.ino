//Nadine Haytham FathAllah 
//Section 2



// 0 --> 7 PD
// 8 --> 13 PB
// A0 --> A5 PC
// OUTPUT --> DDR 1 , PORT has value
// INPUT --> DDR 0 , read from PIN
// INPUT PULLUP --> DDR 0 , PORT 1 , read from PIN


//D register
#define IN1 2
#define IN2 3
#define IN3 4
#define IN4 5
#define LED_DIRECTION 6
#define LED_STATUS 7

//C register
#define BTN_SPEED_UP 0
#define BTN_SPEED_DOWN 1
#define BTN_START_STOP 2
#define BTN_DIR 3
#define BTN_MODE 4


int DELAY = 1000;
int MODE = 0;
int STEP = 0;
bool DIR = true; //initially CW
static bool dirPressed = false; 
static bool startStopPressed = false;
static bool stopped = false; //initially moving 
static bool EmergencyStop = false;

//for debouncing issue
unsigned long lastPress = 0;
unsigned long lastStepTime = 0;

//for emergency stop
unsigned long startTime = 0;
unsigned long endTime = 0;
bool buttonHeld = false;

//for speed up and speed down to make sure buttons aren't blocked
static bool speedUpPressed = false;
static bool speedDownPressed = false;

void setup() {
  Serial.begin(9600);

  //serial monitor + motor driver + leds
  for (int i = 2; i < 8; i++) {
    DDRD |= (1 << i);
  }

  //buttons
  for (int i = 0; i < 5; i++) {
    DDRC &= ~(1 << i);
    PORTC |= (1 << i);
  }

  //7 segment
  for (int i = 0; i < 6; i++) {
    DDRB |= (1 << i);
  }
  DDRC |= (1 << 5);
}

void loop() {

  //direction led, on --> forward, off --> reverse
  if (DIR) PORTD |= (1 << LED_DIRECTION);
  else PORTD &= ~(1 << LED_DIRECTION);

  //status led, on --> moving, off --> stopped (in auto mode)
  if (!stopped) PORTD |= (1 << LED_STATUS);
  else PORTD &= ~(1 << LED_STATUS);

  //changing mode button
  if ((PINC & (1 << BTN_MODE)) == 0) {
    while ((PINC & (1 << BTN_MODE)) == 0)
      ;
    MODE += 1;
    MODE %= 5;
    lastPress = millis();
  }

  //toggling direction button
  if ((PINC & (1 << BTN_DIR)) == 0) {
    if (!dirPressed) {
      DIR = !DIR;
      dirPressed = true;
      lastPress = millis();
    }
  } else dirPressed = false;

  // speed up
  if ((PINC & (1 << BTN_SPEED_UP)) == 0) {
    if (!speedUpPressed) {
      if (DELAY > 100) DELAY -= 100;
      else DELAY = 100;
      speedUpPressed = true;
      lastPress = millis();
    }
  } else speedUpPressed = false;

  // speed down
  if ((PINC & (1 << BTN_SPEED_DOWN)) == 0) {
    if (!speedDownPressed) {
      if (DELAY < 3000) DELAY += 50;
      else DELAY = 3000;
      speedDownPressed = true;
      lastPress = millis();
    }
  } else speedDownPressed = false;

  //emergency stop and resuming logic
  if ((PINC & (1 << BTN_START_STOP)) == 0) {
    if (startTime == 0) startTime = millis();
    if (!buttonHeld && millis() - startTime >= 2000) {
      EmergencyStop = !EmergencyStop;
      stopped = EmergencyStop;
      buttonHeld = true;
      Serial.println(EmergencyStop ? "EMERGENCY STOP" : "RESUME");
    }
  } else {
    startTime = 0;
    buttonHeld = false;
  }

  //emergency stop on 7 segment
  if (EmergencyStop) {
    PORTD ^= (1 << LED_STATUS);
    PORTC |= (1 << 5);
    PORTB = 0b001111;
    delay(200);
    return;
  }

  //main logic
  switch (MODE) {
    case 0:
      PORTC &= ~(1 << 5);
      PORTB = 0b111111;
      stopped = true;
      Serial.println("Stop");
      delay(100);
      break;
    case 1:
      PORTB = 0b110000;
      Serial.println("Manual Half Step");
      Manual_Mode();
      delay(100);
      break;
    case 2:
      PORTB = 0b101101;
      PORTC |= (1 << 5);
      Serial.println("Manual Full Step");
      Manual_Mode();
      delay(100);
      break;
    case 3:
      PORTB = 0b111001;
      PORTC |= (1 << 5);
      stopped = false;
      Serial.println("Auto Half Step");
      Auto_Mode();
      delay(100);
      break;
    case 4:
      PORTB = 0b110010;
      PORTC |= (1 << 5);
      Serial.println("Auto Full Step");
      Auto_Mode();
      delay(100);
      break;
    default:
      Serial.println("STOP");
      delay(100);
      break;
  }
}

//functions

void Manual_Mode() {

  if ((PINC & (1 << BTN_START_STOP)) == 0 && millis() - lastPress > 250) {
    while ((PINC & (1 << BTN_START_STOP)) == 0)
      ;
    if (DIR) STEP += 1;
    else STEP -= 1;
    lastPress = millis();
  }

  if (MODE == 1) {
    if (STEP > 7) STEP = 0;
    else if (STEP < 0) STEP = 7;
    Serial.println(STEP);
    Half_Step_Logic();
    lastStepTime = millis();
  } else if (MODE == 2) {
    if (STEP > 3) STEP = 0;
    if (STEP < 0) STEP = 3;
    Serial.println(STEP);
    Full_Step_Logic();
    lastStepTime = millis();
  }
}

void Auto_Mode() {
  if ((PINC & (1 << BTN_START_STOP)) == 0) {
    if (!startStopPressed) {
      stopped = !stopped;
      startStopPressed = true;
      lastPress = millis();
    }
  } else startStopPressed = false;

  if ((millis() - lastStepTime >= DELAY) && !stopped) {
    if (DIR) STEP++;
    else STEP--;

    if (MODE == 3) {
      if (STEP > 7) STEP = 0;
      else if (STEP < 0) STEP = 7;
      Serial.println(STEP);
      Half_Step_Logic();
      lastStepTime = millis();
    } else if (MODE == 4) {
      if (STEP > 3) STEP = 0;
      if (STEP < 0) STEP = 3;
      Serial.println(STEP);
      Full_Step_Logic();
      lastStepTime = millis();
    }
  }
}

void Half_Step_Logic() {
  switch (STEP) {
    case 0:
      PORTD &= 0xC3;
      PORTD |= (1 << IN1);
      delay(100);
      break;
    case 1:
      PORTD &= 0xC3;
      PORTD |= (1 << IN1) | (1 << IN2);
      delay(100);
      break;
    case 2:
      PORTD &= 0xC3;
      PORTD |= (1 << IN2);
      delay(100);
      break;
    case 3:
      PORTD &= 0xC3;
      PORTD |= (1 << IN2) | (1 << IN3);
      delay(100);
      break;
    case 4:
      PORTD &= 0xC3;
      PORTD |= (1 << IN3);
      delay(100);
      break;
    case 5:
      PORTD &= 0xC3;
      PORTD |= (1 << IN3) | (1 << IN4);
      delay(100);
      break;
    case 6:
      PORTD &= 0xC3;
      PORTD |= (1 << IN4);
      delay(100);
      break;
    case 7:
      PORTD &= 0xC3;
      PORTD |= (1 << IN1) | (1 << IN4);
      delay(100);
      break;
    default:
      PORTD &= 0xC3;
      delay(100);
      break;
  }
}

void Full_Step_Logic() {
  switch (STEP) {
    case 0:
      PORTD &= 0xC3;
      PORTD |= (1 << IN1) | (1 << IN2);
      delay(100);
      break;
    case 1:
      PORTD &= 0xC3;
      PORTD |= (1 << IN2) | (1 << IN3);
      delay(100);
      break;
    case 2:
      PORTD &= 0xC3;
      PORTD |= (1 << IN3) | (1 << IN4);
      delay(100);
      break;
    case 3:
      PORTD &= 0xC3;
      PORTD |= (1 << IN4) | (1 << IN1);
      delay(100);
      break;
    default:
      PORTD &= 0xC3;
      delay(100);
      break;
  }
}
