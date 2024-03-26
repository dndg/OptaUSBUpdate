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

#ifndef OPTA_USB_UPDATE_CONFIG_H_
#define OPTA_USB_UPDATE_CONFIG_H_

#include <Arduino.h>

#if defined(ARDUINO_OPTA)
#define OPTA_USB_UPDATE_MAGIC 0x23410064
#define OPTA_USB_UPDATE_QSPI_SUPPORT
#endif

#endif /* OPTA_USB_UPDATE_CONFIG_H_ */
