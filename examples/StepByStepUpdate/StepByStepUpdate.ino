#include <OptaUSBUpdate.h>

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    if (digitalRead(BTN_USER) == LOW)
    {
        // Button should be pressed for at least 2 seconds, then released.
        unsigned long now = millis();
        while (digitalRead(BTN_USER) == LOW) {
            delay(1);
            if (millis() - now > 2000) {
                digitalWrite(LED_D0, HIGH);
                digitalWrite(LED_D1, HIGH);
                digitalWrite(LED_D2, HIGH);
                digitalWrite(LED_D3, HIGH);
            }
        }
        if (millis() - now > 2000) {
            digitalWrite(LED_D0, LOW);
            digitalWrite(LED_D1, LOW);
            digitalWrite(LED_D2, LOW);
            digitalWrite(LED_D3, LOW);
            doUSBUpdate();
        }
    }
}

void doUSBUpdate()
{
    OptaUSBUpdate_QSPI update(QSPI_FLASH_FATFS_MBR, 2);
    OptaUSBUpdate::Error update_error = OptaUSBUpdate::Error::None;

    Serial.println("Starting firmware update from USB storage");

    digitalWrite(LED_D0, HIGH);
    if ((update_error = update.begin()) != OptaUSBUpdate::Error::None)
    {
        Serial.println("[ERROR] USB update error: " + String((int)update_error));
        blink(LED_D0);
    }

    digitalWrite(LED_D1, HIGH);
    if ((update_error = update.mount()) != OptaUSBUpdate::Error::None)
    {
        Serial.println("[ERROR] USB update error: " + String((int)update_error));
        blink(LED_D1);
    }

    digitalWrite(LED_D2, HIGH);
    if ((update_error = update.decompress("UPDATE.OTA")) != OptaUSBUpdate::Error::None)
    {
        Serial.println("[ERROR] USB update error: " + String((int)update_error));
        blink(LED_D2);
    }

    digitalWrite(LED_D3, HIGH);
    if ((update_error = update.update()) != OptaUSBUpdate::Error::None)
    {
        Serial.println("[ERROR] USB update error: " + String((int)update_error));
        blink(LED_D3);
    }

    delay(1000);

    digitalWrite(LED_D0, LOW);
    digitalWrite(LED_D1, LOW);
    digitalWrite(LED_D2, LOW);
    digitalWrite(LED_D3, LOW);

    delay(1000);

    digitalWrite(LED_D0, HIGH);
    digitalWrite(LED_D1, HIGH);
    digitalWrite(LED_D2, HIGH);
    digitalWrite(LED_D3, HIGH);

    delay(1000);

    update.reset();
}

void blink(unsigned int led)
{
    while (true) {
        digitalWrite(led, LOW);
        delay(250);
        digitalWrite(led, HIGH);
        delay(250);
    }
}

