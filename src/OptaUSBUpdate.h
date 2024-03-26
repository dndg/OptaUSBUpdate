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

#ifndef OPTA_USB_UPDATE_H_
#define OPTA_USB_UPDATE_H_

#include "OptaUSBUpdate_Config.h"
#if defined(OPTA_USB_UPDATE_QSPI_SUPPORT)
#include <QSPIFBlockDevice.h>
#endif

#include <BlockDevice.h>
#include <MBRBlockDevice.h>
#include <FATFileSystem.h>
#include <Arduino_UnifiedStorage.h>

#define OUU_QSPI_FLASH_FLAG (1 << 2)
#define OUU_SDCARD_FLAG     (1 << 3)
#define OUU_RAW_FLAG        (1 << 4)
#define OUU_FATFS_FLAG      (1 << 5)
#define OUU_LITTLEFS_FLAG   (1 << 6)
#define OUU_MBR_FLAG        (1 << 7)

#define OPTA_USB_UPDATE_HAS_WATCHDOG_FEED

#define UPDATE_FILE_NAME "/fs/UPDATE.BIN"

enum OptaUSBUpdateStorageType
{
    QSPI_FLASH_FATFS = OUU_QSPI_FLASH_FLAG | OUU_FATFS_FLAG,
    QSPI_FLASH_FATFS_MBR = OUU_QSPI_FLASH_FLAG | OUU_FATFS_FLAG | OUU_MBR_FLAG,
};

typedef void (*OptaUSBUpdateWatchdogResetFuncPointer)(void);

class OptaUSBUpdate
{
public:
    enum class Error : int
    {
        None = 0,
        NoCapableBootloader = -1,
        NoOtaStorage = -2,
        OtaStorageInit = -3,
        OtaStorageOpen = -4,
        OtaHeaderLength = -5,
        OtaHeaderCrc = -6,
        OtaHeaterMagicNumber = -7,
        OtaUsbError = -8,
        OtaFileNotFound = -9,
    };

    OptaUSBUpdate(OptaUSBUpdateStorageType const storage_type, uint32_t const data_offset);
    virtual ~OptaUSBUpdate();

    static bool isOtaCapable();
    Error begin();
    Error mount();
    Error decompress(const char *update_file_path);
    Error update();
    Error updateFromUSB(const char *update_file_path);
    void reset();
    void setFeedWatchdogFunc(OptaUSBUpdateWatchdogResetFuncPointer func);
    void feedWatchdog();

protected:
    OptaUSBUpdateStorageType _storage_type;
    uint32_t _data_offset;
    uint32_t _program_length;
    mbed::BlockDevice *_bd_raw_qspi;
    USBStorage _usb_storage;

    virtual bool init() = 0;
    virtual bool open() = 0;

    static bool findProgramLength(DIR *dir, uint32_t &program_length);

private:
    void write();
    OptaUSBUpdateWatchdogResetFuncPointer _feed_watchdog_func = 0;
};

#if defined(OPTA_USB_UPDATE_QSPI_SUPPORT)
#include "OptaUSBUpdate_QSPI.h"
#endif

#endif /* OPTA_USB_UPDATE_H_ */