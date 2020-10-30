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
    uint32_t fmcycles;
    uint32_t fmstate;
} opnb_fm_t;

typedef struct {
    opnb_fm_t fm;
    uint32_t clock_state;
    uint32_t cycles;
    uint8_t ic;
    uint32_t ic_sr;
    uint32_t ic2;
    uint32_t ic_latch;

    uint8_t fm_data;
    uint16_t fm_address;
    uint16_t timer_a_reg;
    uint8_t timer_b_reg;
    uint8_t mode_1c;
    uint8_t mode_ch3;
    uint8_t mode_csm;
    uint8_t mode_test;
    uint8_t mode_test2;
    uint8_t mode_kon_operator[4];
    uint8_t mode_kon_channel;
    uint8_t lfo_en;
    uint8_t lfo_freq;
    uint8_t timer_a_load;
    uint8_t timer_a_enable;
    uint8_t timer_a_reset;
    uint8_t timer_b_load;
    uint8_t timer_b_enable;
    uint8_t timer_b_reset;
    uint8_t write_fm_address;
    uint8_t write_fm_data;
    uint8_t write_mode_1c;
    uint8_t write_mode_27;
    uint8_t write_mode_25;
    uint8_t write_mode_24;
    uint8_t write_mode_21;
    uint8_t write_mode_28;
    uint8_t write_mode_26;
    uint8_t write_mode_22;
    uint8_t write_mode_02;
    uint8_t write_d;
    uint8_t write_d_en;
    uint8_t write_a;
    uint8_t write_a_en;
    uint16_t write_data;
    uint8_t write_ssg;
} opnb_t;

#endif
