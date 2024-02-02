//calibrate_air_mouse
#include <Wire.h>
#include <MPU6050.h>
#include <Mouse.h>

MPU6050 mpu;

int16_t ax, ay, az, gx, gy, gz;
float moveX, moveY;
bool buttonState = false;
bool buttonUp = false;
bool gyroActive = false;
bool mousePressed = false;

void setup() {
  Serial.begin(9600);
  pinMode(5, INPUT_PULLUP);
  pinMode(8, INPUT);

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

  delay(15);
}

void clickMouse() {
  if (buttonState && !mousePressed) {
    Mouse.press(MOUSE_LEFT);
    mousePressed = true;
  }

  if (!buttonState && mousePressed) {
    Mouse.release(MOUSE_LEFT);
    mousePressed = false;
  }
}

void checkButtons() {
  buttonState = digitalRead(5) == LOW;
  gyroActive = digitalRead(8) == HIGH;
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

  moveX += 1.5;

  if (abs(moveX) < 1) {
    moveX = 0;
  }

  if (abs(moveY) < 1) {
    moveY = 0;
  }
}
