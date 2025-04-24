#include <Arduino.h>

void sendData(char *label, long enc)
{
    Serial.print(label);
    Serial.print(": ");
    Serial.println(enc);
}