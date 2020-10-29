/* Nuked OPNB
 * Copyright (C) 2020 Nuke.YKT
 *
 * This file is part of Nuked OPNB.
 *
 * Nuked OPNB is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 2.1
 * of the License, or (at your option) any later version.
 *
 * Nuked OPNB is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Nuked OPNB. If not, see <https://www.gnu.org/licenses/>.
 *
 *  Nuked OPNB emulator.
 *  Thanks:
 *      siliconpr0n.org(John McMaster, digshadow):
 *          YM2610 and other FM chip decaps and die shots.
 *
 * version: 0.0
 */
#ifndef _OPM_H_
#define _OPM_H_

#include <stdint.h>

typedef struct {
    uint32_t cycles;
} opnb_t;

#endif