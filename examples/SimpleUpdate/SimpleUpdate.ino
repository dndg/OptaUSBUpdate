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
        while (digitalRead(BTN_USER) == LOW)
        {
            delay(1);
            if (millis() - now > 2000)
            {
                digitalWrite(LED_D0, HIGH);
                digitalWrite(LED_D1, HIGH);
                digitalWrite(LED_D2, HIGH);
                digitalWrite(LED_D3, HIGH);
            }
        }
        if (millis() - now > 2000)
        {
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
    OptaUSBUpdate::Error update_error = update.updateFromUSB("UPDATE.OTA");

    if (update_error != OptaUSBUpdate::Error::None)
    {
        Serial.println("[ERROR] USB update error: " + String((int)update_error));
        blink();
    }

    digitalWrite(LED_D0, HIGH);
    digitalWrite(LED_D1, HIGH);
    digitalWrite(LED_D2, HIGH);
    digitalWrite(LED_D3, HIGH);

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

void blink()
{
    while (true)
    {
        digitalWrite(LED_D0, LOW);
        digitalWrite(LED_D1, LOW);
        digitalWrite(LED_D2, LOW);
        digitalWrite(LED_D3, LOW);
        delay(250);
        digitalWrite(LED_D0, HIGH);
        digitalWrite(LED_D1, HIGH);
        digitalWrite(LED_D2, HIGH);
        digitalWrite(LED_D3, HIGH);
        delay(250);
    }
}
