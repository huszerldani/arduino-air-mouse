#include <Wire.h>
#include <MPU6050.h>
#include <Mouse.h>

MPU6050 mpu;

int16_t ax, ay, az, gx, gy, gz;
float moveX, moveY;

bool buttonDown = false;
bool buttonUp = false;

unsigned long buttonDownTime = NULL;

void setup() {
  Serial.begin(9600);
  pinMode(5, INPUT_PULLUP);

  delay(1000);
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
  checkButton();
  getMovement();
  moveMouse();
  clickMouse();

  delay(20);
}

void clickMouse() {
  if (buttonUp) {
    Mouse.click(MOUSE_LEFT);
  }
}

void checkButton() {
  if (buttonUp) {
    buttonUp = false;
  }

  bool currentButtonDown = digitalRead(5) == HIGH;

  if (buttonDown && !currentButtonDown) {
    buttonUp = true;
    buttonDownTime = NULL;
  }

  if (currentButtonDown && !buttonDownTime) {
    buttonDownTime = millis();
  }

  buttonDown = currentButtonDown;
}

void moveMouse() {
  if (buttonDown && (millis() - buttonDownTime) > 100) {
    Mouse.move(moveX, moveY);
  }
}

void getMovement() {
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  moveX = -(gz + 100) / 150;
  moveY = -(gx + 260) / 150;

  moveX += 1.5;

  if (abs(moveX) < 1) {
    moveX = 0;
  }

  if (abs(moveY) < 1) {
    moveY = 0;
  }
}
