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

#ifndef OPTA_USB_UPDATE_QSPI_H_
#define OPTA_USB_UPDATE_QSPI_H_

#include "OptaUSBUpdate.h"
#include "OptaUSBUpdate_Config.h"

#if defined(OPTA_USB_UPDATE_QSPI_SUPPORT)

class OptaUSBUpdate_QSPI : public OptaUSBUpdate
{

public:
    OptaUSBUpdate_QSPI(OptaUSBUpdateStorageType const storage_type, uint32_t const data_offset);
    virtual ~OptaUSBUpdate_QSPI() {}

protected:
    virtual bool init() override;
    virtual bool open() override;

private:
    mbed::BlockDevice *_bd_qspi;
    mbed::FATFileSystem *_fs_qspi;
};

#endif /* OPTA_USB_UPDATE_QSPI_SUPPORT */
#endif /* OPTA_USB_UPDATE_QSPI_H_ */
