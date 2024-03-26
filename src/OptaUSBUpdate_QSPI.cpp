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

#include "OptaUSBUpdate_Config.h"
#if defined(OPTA_USB_UPDATE_QSPI_SUPPORT)

#include "OptaUSBUpdate_QSPI.h"
#include "OptaUSBUpdate_Debug.h"
#include <assert.h>

using namespace arduino;

OptaUSBUpdate_QSPI::OptaUSBUpdate_QSPI(OptaUSBUpdateStorageType const storage_type, uint32_t const data_offset)
    : OptaUSBUpdate(storage_type, data_offset), _bd_qspi{NULL}, _fs_qspi{NULL}
{
    assert(_storage_type == QSPI_FLASH_FATFS || _storage_type == QSPI_FLASH_FATFS_MBR);
}

bool OptaUSBUpdate_QSPI::init()
{
    _bd_raw_qspi = mbed::BlockDevice::get_default_instance();

    if (_bd_raw_qspi->init() != QSPIF_BD_ERROR_OK)
    {
        DEBUG_ERROR(F("Error: QSPI init failure."));
        return false;
    }

    if (_storage_type == QSPI_FLASH_FATFS)
    {
        _fs_qspi = new mbed::FATFileSystem("fs");
        int const err_mount = _fs_qspi->mount(_bd_raw_qspi);
        if (err_mount)
        {
            DEBUG_ERROR(F("Error while mounting the filesystem. Err = %d"), err_mount);
            return false;
        }
        return true;
    }

    if (_storage_type == QSPI_FLASH_FATFS_MBR)
    {
        _bd_qspi = new mbed::MBRBlockDevice(_bd_raw_qspi, _data_offset);
        _fs_qspi = new mbed::FATFileSystem("fs");
        int const err_mount = _fs_qspi->mount(_bd_qspi);
        if (err_mount)
        {
            DEBUG_ERROR(F("Error while mounting the filesystem. Err = %d"), err_mount);
            return false;
        }
        return true;
    }
    return false;
}

bool OptaUSBUpdate_QSPI::open()
{
    DIR *dir = NULL;
    if ((dir = opendir("/fs")) != NULL)
    {
        if (OptaUSBUpdate::findProgramLength(dir, _program_length))
        {
            closedir(dir);
            return true;
        }
        closedir(dir);
    }

    return false;
}

#endif /* OPTA_USB_UPDATE_QSPI_SUPPORT */
