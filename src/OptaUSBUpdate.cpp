/* This file is part of OptaUSBUpdate.
 *
 * Copyright 2019 ARDUINO SA (http://www.arduino.cc/)
 * Copyright 2024 DNDG srl (http://dndg.it/)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Based on code from the Arduino_Portenta_OTA library.
*/

#include <Arduino.h>
#include <stm32h7xx_hal_rtc_ex.h>
#include "OptaUSBUpdate.h"
#include "OptaUSBUpdate_Debug.h"
#include "lzss.h"
#include "crc.h"

extern RTC_HandleTypeDef RTCHandle;

union HeaderVersion
{
    struct __attribute__((packed))
    {
        uint32_t header_version : 6;
        uint32_t compression : 1;
        uint32_t signature : 1;
        uint32_t spare : 4;
        uint32_t payload_target : 4;
        uint32_t payload_major : 8;
        uint32_t payload_minor : 8;
        uint32_t payload_patch : 8;
        uint32_t payload_build_num : 24;
    } field;
    uint8_t buf[sizeof(field)];
    static_assert(sizeof(buf) == 8, "Error: sizeof(HEADER.VERSION) != 8");
};

union OTAHeader
{
    struct __attribute__((packed))
    {
        uint32_t len;
        uint32_t crc32;
        uint32_t magic_number;
        HeaderVersion hdr_version;
    } header;
    uint8_t buf[sizeof(header)];
    static_assert(sizeof(buf) == 20, "Error: sizeof(HEADER) != 20");
};

OptaUSBUpdate::OptaUSBUpdate(OptaUSBUpdateStorageType const storage_type, uint32_t const data_offset)
    : _storage_type{storage_type}, _data_offset{data_offset}, _program_length{0}
{
}

OptaUSBUpdate::~OptaUSBUpdate()
{
}

bool OptaUSBUpdate::isOtaCapable()
{
#define BOOTLOADER_ADDR (0x8000000)
    uint32_t bootloader_data_offset = 0x1F000;
    uint8_t *bootloader_data = (uint8_t *)(BOOTLOADER_ADDR + bootloader_data_offset);
    uint8_t currentBootloaderVersion = bootloader_data[1];
    if (currentBootloaderVersion < 22)
    {
        return false;
    }
    else
    {
        return true;
    }
}

OptaUSBUpdate::Error OptaUSBUpdate::begin()
{
    if (!isOtaCapable())
    {
        return Error::NoCapableBootloader;
    }
    if (!init())
    {
        return Error::OtaStorageInit;
    }
    return Error::None;
}

OptaUSBUpdate::Error OptaUSBUpdate::mount()
{
    _usb_storage = USBStorage();
    if (!_usb_storage.begin())
    {
        return Error::OtaUsbError;
    }
    return Error::None;
}

OptaUSBUpdate::Error OptaUSBUpdate::decompress(const char *update_file_name)
{
    String path = String("/usb/");
    path.concat(update_file_name);

    struct stat stat_buf;
    int stat_error = stat(path.c_str(), &stat_buf);
    if (stat_error < 0) {
        return Error::OtaFileNotFound;
    }

    off_t update_file_size = stat_buf.st_size;

    FILE *update_file = fopen(path.c_str(), "rb");
    if (!update_file) {
        return Error::OtaFileNotFound;
    }

    OTAHeader ota_header;
    uint32_t read;
    uint32_t crc32;
    uint8_t crc_buf[128];

    feedWatchdog();

    /* Read the OTA header */
    fread(ota_header.buf, 1, sizeof(ota_header.buf), update_file);

    /* Check first length. */
    if (ota_header.header.len != (update_file_size - sizeof(ota_header.header.len) - sizeof(ota_header.header.crc32)))
    {
        fclose(update_file);
        return Error::OtaHeaderLength;
    }

    feedWatchdog();

    /* Check CRC, then rewind to start of CRC verified header */
    fseek(update_file, sizeof(ota_header.header.len) + sizeof(ota_header.header.crc32), SEEK_SET);
    crc32 = 0xFFFFFFFF;
    for (read = 0;
         read < (ota_header.header.len - sizeof(crc_buf));
         read += sizeof(crc_buf))
    {
        fread(crc_buf, 1, sizeof(crc_buf), update_file);
        crc32 = crc_update(crc32, crc_buf, sizeof(crc_buf));
        if (read % 4096 == 0) {
            feedWatchdog();
        }
    }
    fread(crc_buf, 1, ota_header.header.len - read, update_file);
    crc32 = crc_update(crc32, crc_buf, ota_header.header.len - read);
    crc32 ^= 0xFFFFFFFF;

    if (ota_header.header.crc32 != crc32)
    {
        fclose(update_file);
        return Error::OtaHeaderCrc;
    }

    feedWatchdog();

    if (ota_header.header.magic_number != OPTA_USB_UPDATE_MAGIC)
    {
        fclose(update_file);
        return Error::OtaHeaterMagicNumber;
    }

    /* Rewind to start of LZSS compressed binary */
    fseek(update_file, sizeof(ota_header.buf), SEEK_SET);

    uint32_t const LZSS_FILE_SIZE = update_file_size - sizeof(ota_header.buf);

    FILE *decompressed = fopen(UPDATE_FILE_NAME, "w");

    lzss_init(update_file, decompressed, LZSS_FILE_SIZE, _feed_watchdog_func);
    lzss_decode();
    lzss_flush();

    /* Delete UPDATE.BIN.LZSS because this update is complete. */
    fclose(update_file);
    fclose(decompressed);

    return Error::None;
}

OptaUSBUpdate::Error OptaUSBUpdate::update()
{
    if (!open())
    {
        return Error::OtaStorageOpen;
    }

    write();

    return Error::None;
}

OptaUSBUpdate::Error OptaUSBUpdate::updateFromUSB(const char *update_file_name)
{
    OptaUSBUpdate::Error update_error = OptaUSBUpdate::Error::None;

    if ((update_error = begin()) != OptaUSBUpdate::Error::None)
    {
        return update_error;
    }

    if ((update_error = mount()) != OptaUSBUpdate::Error::None)
    {
        return update_error;
    }

    if ((update_error = decompress(update_file_name)) != OptaUSBUpdate::Error::None)
    {
        return update_error;
    }

    if ((update_error = update()) != OptaUSBUpdate::Error::None)
    {
        return update_error;
    }

    return Error::None;
}

void OptaUSBUpdate::reset()
{
    NVIC_SystemReset();
}

void OptaUSBUpdate::setFeedWatchdogFunc(OptaUSBUpdateWatchdogResetFuncPointer func)
{
    _feed_watchdog_func = func;
}

void OptaUSBUpdate::feedWatchdog()
{
    if (_feed_watchdog_func)
        _feed_watchdog_func();
}

bool OptaUSBUpdate::findProgramLength(DIR *dir, uint32_t &program_length)
{
    struct dirent *entry = NULL;
    while ((entry = readdir(dir)) != NULL)
    {
        if (String(entry->d_name) == "UPDATE.BIN")
        {
            struct stat stat_buf;
            stat("/fs/UPDATE.BIN", &stat_buf);
            program_length = stat_buf.st_size;
            return true;
        }
    }

    return false;
}

void OptaUSBUpdate::write()
{
    HAL_RTCEx_BKUPWrite(&RTCHandle, RTC_BKP_DR0, 0x07AA);
    HAL_RTCEx_BKUPWrite(&RTCHandle, RTC_BKP_DR1, _storage_type);
    HAL_RTCEx_BKUPWrite(&RTCHandle, RTC_BKP_DR2, _data_offset);
    HAL_RTCEx_BKUPWrite(&RTCHandle, RTC_BKP_DR3, _program_length);
}
