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

#ifndef OPTA_USB_UPDATE_DEBUG_H_
#define OPTA_USB_UPDATE_DEBUG_H_

/******************************************************************************
 * INCLUDE
 ******************************************************************************/

#if defined __has_include
#if __has_include(<Arduino_DebugUtils.h>)
#include <Arduino_DebugUtils.h>
#else
#define OPTA_USB_UPDATE_NO_DEBUG
#endif
#else
#define OPTA_USB_UPDATE_NO_DEBUG
#endif

#ifdef OPTA_USB_UPDATE_NO_DEBUG
#define DEBUG_ERROR(fmt, ...) ((void)0)
#define DEBUG_WARNING(fmt, ...) ((void)0)
#define DEBUG_INFO(fmt, ...) ((void)0)
#define DEBUG_DEBUG(fmt, ...) ((void)0)
#define DEBUG_VERBOSE(fmt, ...) ((void)0)
#endif
#undef OPTA_USB_UPDATE_NO_DEBUG

#endif /* OPTA_USB_UPDATE_DEBUG_H_ */
