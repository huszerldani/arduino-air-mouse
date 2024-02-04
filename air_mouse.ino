#include <Wire.h>
#include <MPU6050.h>
#include <Mouse.h>

MPU6050 mpu;

int16_t ax, ay, az, gx, gy, gz;
float moveX, moveY, unprocessedMoveY;

bool buttonDown = false;
bool buttonUp = false;

bool gyroActive = false;
bool mousePressed = false;

unsigned long buttonDownTime = NULL;
unsigned long vibrationStart = NULL;

unsigned int vibrationTime = 40;

int maxZ = 0;

int vibrationPin = 4;
int buttonPin = 5;
int switchPin = 8;

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(switchPin, INPUT);
  pinMode(vibrationPin, OUTPUT);

  delay(500);
  Wire.begin();

  mpu.initialize();

  if (!mpu.testConnection()) {
    while (1) {
      Serial.println("MPU6050 Sensor not found");
    }
  }

  Serial.println("MPU6050 Sensor initialized");
}

void loop() {
  checkButtons();
  getMovement();
  moveMouse();
  clickMouse();
  vibrationStop();
  processScroll();

  delay(15);
}

void processScroll() {
  if (!gyroActive && buttonDown && buttonDownTime != NULL && (millis() - buttonDownTime) > 200) {
    Mouse.move(0, 0, -(unprocessedMoveY));
    delay(100);
  }
}

void clickMouse() {
  if (buttonDown && !mousePressed) {
    mousePressed = true;
    buttonDownTime = millis();
  }

  if (gyroActive) {
    if (buttonDown && mousePressed && millis() - buttonDownTime > 150) {
      Mouse.press(MOUSE_LEFT);
      buttonDownTime = NULL;
    }

    if (!buttonDown && mousePressed) {
      if (buttonDownTime) {
        vibrationtart();
        Mouse.click(MOUSE_LEFT);
      } else {
        vibrationtart();
        Mouse.release(MOUSE_LEFT);
      }

      buttonDownTime = NULL;
      mousePressed = false;
    }
  } else {
    if (!buttonDown && mousePressed && (millis() - buttonDownTime) < 500) {
      vibrationtart();
      Mouse.click(MOUSE_LEFT);
      buttonDownTime = NULL;
      mousePressed = false;
    }
  }
}

void vibrationtart() {
  vibrationStart = millis();
  digitalWrite(vibrationPin, HIGH);
}

void vibrationStop() {
  if (vibrationStart == NULL) {
    return;
  }

  if ((millis() - vibrationStart) >= vibrationTime) {
    digitalWrite(vibrationPin, LOW);
  }
}

void checkButtons() {
  buttonDown = digitalRead(buttonPin) == LOW;

  if (!gyroActive && digitalRead(switchPin) == HIGH) {
    vibrationtart();
    buttonDownTime = NULL;
    mousePressed = false;
  }

  if (gyroActive && digitalRead(switchPin) == LOW) {
    buttonDownTime = NULL;
    mousePressed = false;
  }

  gyroActive = digitalRead(switchPin) == HIGH;
}

void moveMouse() {
  if (gyroActive) {
    Mouse.move(moveX, moveY);
  }
}

void getMovement() {
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  moveX = -(gz + 100) / 150;
  moveY = -(gx + 260) / 150;
  // int moveZ = (gz + 260) / 150;

  // Serial.println(az);

  unprocessedMoveY = moveY;

  moveX += 1.5;

  if (abs(moveX) < 1) {
    moveX = 0;
  }

  if (abs(moveY) < 1) {
    moveY = 0;
  }

  moveX /= 2;
  moveY /= 2;
}
