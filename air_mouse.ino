//calibrate_air_mouse
#include <Wire.h>
#include <MPU6050.h>
#include <Mouse.h>

int count = 0;

MPU6050 mpu;

int16_t ax, ay, az, gx, gy, gz;
float moveX, moveY;
bool buttonState = false;

void setup() {
  Serial.begin(9600);
  pinMode(5, INPUT_PULLUP);

  while (!Serial)
    ;  // unless serial cable is connected, do nothing

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

  delay(20);
}

void checkButton() {
  buttonState = digitalRead(5) == HIGH;
}

void moveMouse() {
  if (buttonState) {
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
