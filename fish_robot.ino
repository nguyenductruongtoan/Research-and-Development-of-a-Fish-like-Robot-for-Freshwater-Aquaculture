#include <ESP32Servo.h>
#include <Wire.h>
#include <MPU6050.h>
MPU6050 mpu;
// ==========================
// KHAI BÁO SERVO
// ==========================
Servo servo1;
Servo servo2;
// ==========================
// CHÂN KẾT NỐI
// ==========================
#define SERVO1_PIN 23
#define SERVO2_PIN 18
#define SHARP_LEFT 34
#define SHARP_RIGHT 35
// ==========================
// THÔNG SỐ DAO ĐỘNG
// ==========================
float A1 = 25.0;
float A2 = 30.0;
float freq = 1.2;
float phase = PI / 3.0;
float center1 = 90;
float center2 = 90;
// ==========================
// BIẾN HỆ THỐNG
// ==========================
unsigned long startTime;
bool obstacleDetected = false;
bool stuckDetected = false;
int stuckCounter = 0;
// ==========================
// HÀM KHỞI TẠO
// ==========================
void setup()
{
    Serial.begin(115200);
    servo1.attach(SERVO1_PIN);
    servo2.attach(SERVO2_PIN);
    pinMode(SHARP_LEFT, INPUT);
    pinMode(SHARP_RIGHT, INPUT);
    Wire.begin();
    mpu.initialize();
    if(mpu.testConnection())
    {
        Serial.println("MPU6050 connected");
    }
    else
    {
        Serial.println("MPU6050 error");
    }
    startTime = millis();
}
// ==========================
// VÒNG LẶP CHÍNH
// ==========================
void loop()
{
    readSensor();

    if(stuckDetected)
    {
        escapeStuck();
    }
    else if(obstacleDetected)
    {
        avoidObstacle();
    }
    else
    {
        swimForward();
    }
    delay(20);
}
// ==========================
// ĐỌC CẢM BIẾN
// ==========================
void readSensor()
{
    int leftValue = analogRead(SHARP_LEFT);
    int rightValue = analogRead(SHARP_RIGHT);
    obstacleDetected = false;
    if(leftValue > 2500 || rightValue > 2500)
    {
        obstacleDetected = true;
    }
    detectStuck();
}
// ==========================
// BƠI THẲNG
// ==========================

void swimForward()
{
    float t = (millis() - startTime) / 1000.0;
    float theta1 =
        center1 +
        A1 * sin(2.0 * PI * freq * t);
    float theta2 =
        center2 +
        A2 * sin(2.0 * PI * freq * t + phase);

    servo1.write(theta1);
    servo2.write(theta2);
}
// ==========================
// TRÁNH VẬT CẢN
// ==========================
void avoidObstacle()
{
    Serial.println("Obstacle detected");
    for(int i = 0; i < 20; i++)
    {
        servo1.write(60);
        servo2.write(120);

        delay(30);
    }
}
// ==========================
// PHÁT HIỆN KẸT
// ==========================
void detectStuck()
{
    int16_t ax, ay, az;
    int16_t gx, gy, gz;
    mpu.getMotion6(
        &ax, &ay, &az,
        &gx, &gy, &gz
    );
    int motionLevel =
        abs(ax) +
        abs(ay) +
        abs(gx);

    if(motionLevel < 500)
    {
        stuckCounter++;
    }
    else
    {
        stuckCounter = 0;
    }
    if(stuckCounter > 100)
    {
        stuckDetected = true;
    }
}
// ==========================
// THOÁT KẸT
// ==========================
void escapeStuck()
{
    Serial.println("ESCAPE_STUCK");
    for(int i = 0; i < 25; i++)
    {
        servo1.write(130);
        servo2.write(50);
        delay(25);
    }
    stuckDetected = false;
    stuckCounter = 0;
}

