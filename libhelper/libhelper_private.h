/*
  GPL
  (c) 2021, thorsten.johannvorderbrueggen@t-online.de

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
*/

#ifndef _LIBHELPER_PRIVATE_H_
#define _LIBHELPER_PRIVATE_H_

#define LIBHELPER_EXPORT __attribute__ ((visibility ("default")))
#define LIBHELPER_LOCAL __attribute__ ((visibility ("hidden")))

#define GPIO_PATH_MAX_LEN 40

#define DIR_EXPORT true
#define DIR_UNEXPORT false

#define GPIO_EXPORT_PATH "/sys/class/gpio/export"
#define GPIO_UNEXPORT_PATH "/sys/class/gpio/unexport"

#define GPIO_DIRECTION_PATH "/sys/class/gpio/gpio%d/direction"
#define GPIO_EDGE_PATH "/sys/class/gpio/gpio%d/edge"

#endif
