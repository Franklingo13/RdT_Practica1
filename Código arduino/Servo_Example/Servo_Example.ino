#include <Servo.h>

static const int servoPin = 13;
Servo servo1;
int posDegrees = 0;

void setup() {
    Serial.begin(115200);
    servo1.attach(servoPin);
}

void loop() {
        servo1.write(posDegrees);
        Serial.println(posDegrees);
        delay(20);

}
