# Opta update via USB

The library's functionality centers around facilitating the process of
updating Opta by reading the new firmware from a USB storage device.

## Usage

The code below shows a basic example of how to use this library. More
examples are included in the `examples` folder.

```cpp
#include <OptaUSBUpdate.h>

void loop()
{
    // Start the update when the user button is pressed for at least 2s.
    if (digitalRead(BTN_USER) == LOW)
    {
        unsigned long now = millis();
        while (digitalRead(BTN_USER) == LOW)
        {
            if (millis() - now > 2000)
            {
                doUSBUpdate();
            }
        }
    }
}

void doUSBUpdate()
{
    // The file UPDATE.OTA is stored on the second FAT partition.
    OptaUSBUpdate_QSPI update(QSPI_FLASH_FATFS_MBR, 2);
    OptaUSBUpdate::Error update_error = update.updateFromUSB("UPDATE.OTA"));

    if (update_error != OptaUSBUpdate::Error::None)
    {
        Serial.println("USB update error: " + String((int)update_error));
    }

    update.reset();
}
```

## How to create `UPDATE.OTA`

To create the `UPDATE.OTA` file, start from a sketch and follow the steps:

1) In Arduino IDE select **Sketch->Export compiled Binary** from the menu or,
if using `arduino-cli` use the `--export-binaries` command line option.

2) From the command line, execute the `bin2ota.py` program that can be found
in the `extras/make-ota` folder:

```
$ extras/make-ota/bin2ota.py OPTA yoursketch.bin UPDATE.OTA
```

## Resources

* [Getting started with Opta](https://opta.findernet.com/en/tutorial/getting-started).

## License

This library is released under version 3 of the GNU General Public License. 
For more details read [the full license](./LICENSE).

The code in `extras/make-ota` is a modified version of the Arduino code
available from https://github.com/arduino-libraries/ArduinoIoTCloud/ and
has the same license (GPL3).

## Contact

For communication reach out to <iot@dndg.it>.
