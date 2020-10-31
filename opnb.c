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
#include <string.h>
#include <stdint.h>
#include "opnb.h"

enum {
    eg_num_attack = 0,
    eg_num_decay = 1,
    eg_num_sustain = 2,
    eg_num_release = 3
};

enum {
    clock_fm_1 = 0,
    clock_fm_2,
};

/* logsin table */
static const uint16_t logsinrom[256] = {
    0x859, 0x6c3, 0x607, 0x58b, 0x52e, 0x4e4, 0x4a6, 0x471,
    0x443, 0x41a, 0x3f5, 0x3d3, 0x3b5, 0x398, 0x37e, 0x365,
    0x34e, 0x339, 0x324, 0x311, 0x2ff, 0x2ed, 0x2dc, 0x2cd,
    0x2bd, 0x2af, 0x2a0, 0x293, 0x286, 0x279, 0x26d, 0x261,
    0x256, 0x24b, 0x240, 0x236, 0x22c, 0x222, 0x218, 0x20f,
    0x206, 0x1fd, 0x1f5, 0x1ec, 0x1e4, 0x1dc, 0x1d4, 0x1cd,
    0x1c5, 0x1be, 0x1b7, 0x1b0, 0x1a9, 0x1a2, 0x19b, 0x195,
    0x18f, 0x188, 0x182, 0x17c, 0x177, 0x171, 0x16b, 0x166,
    0x160, 0x15b, 0x155, 0x150, 0x14b, 0x146, 0x141, 0x13c,
    0x137, 0x133, 0x12e, 0x129, 0x125, 0x121, 0x11c, 0x118,
    0x114, 0x10f, 0x10b, 0x107, 0x103, 0x0ff, 0x0fb, 0x0f8,
    0x0f4, 0x0f0, 0x0ec, 0x0e9, 0x0e5, 0x0e2, 0x0de, 0x0db,
    0x0d7, 0x0d4, 0x0d1, 0x0cd, 0x0ca, 0x0c7, 0x0c4, 0x0c1,
    0x0be, 0x0bb, 0x0b8, 0x0b5, 0x0b2, 0x0af, 0x0ac, 0x0a9,
    0x0a7, 0x0a4, 0x0a1, 0x09f, 0x09c, 0x099, 0x097, 0x094,
    0x092, 0x08f, 0x08d, 0x08a, 0x088, 0x086, 0x083, 0x081,
    0x07f, 0x07d, 0x07a, 0x078, 0x076, 0x074, 0x072, 0x070,
    0x06e, 0x06c, 0x06a, 0x068, 0x066, 0x064, 0x062, 0x060,
    0x05e, 0x05c, 0x05b, 0x059, 0x057, 0x055, 0x053, 0x052,
    0x050, 0x04e, 0x04d, 0x04b, 0x04a, 0x048, 0x046, 0x045,
    0x043, 0x042, 0x040, 0x03f, 0x03e, 0x03c, 0x03b, 0x039,
    0x038, 0x037, 0x035, 0x034, 0x033, 0x031, 0x030, 0x02f,
    0x02e, 0x02d, 0x02b, 0x02a, 0x029, 0x028, 0x027, 0x026,
    0x025, 0x024, 0x023, 0x022, 0x021, 0x020, 0x01f, 0x01e,
    0x01d, 0x01c, 0x01b, 0x01a, 0x019, 0x018, 0x017, 0x017,
    0x016, 0x015, 0x014, 0x014, 0x013, 0x012, 0x011, 0x011,
    0x010, 0x00f, 0x00f, 0x00e, 0x00d, 0x00d, 0x00c, 0x00c,
    0x00b, 0x00a, 0x00a, 0x009, 0x009, 0x008, 0x008, 0x007,
    0x007, 0x007, 0x006, 0x006, 0x005, 0x005, 0x005, 0x004,
    0x004, 0x004, 0x003, 0x003, 0x003, 0x002, 0x002, 0x002,
    0x002, 0x001, 0x001, 0x001, 0x001, 0x001, 0x001, 0x001,
    0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000
};

/* exp table */
static const uint16_t exprom[256] = {
    0x7fa, 0x7f5, 0x7ef, 0x7ea, 0x7e4, 0x7df, 0x7da, 0x7d4,
    0x7cf, 0x7c9, 0x7c4, 0x7bf, 0x7b9, 0x7b4, 0x7ae, 0x7a9,
    0x7a4, 0x79f, 0x799, 0x794, 0x78f, 0x78a, 0x784, 0x77f,
    0x77a, 0x775, 0x770, 0x76a, 0x765, 0x760, 0x75b, 0x756,
    0x751, 0x74c, 0x747, 0x742, 0x73d, 0x738, 0x733, 0x72e,
    0x729, 0x724, 0x71f, 0x71a, 0x715, 0x710, 0x70b, 0x706,
    0x702, 0x6fd, 0x6f8, 0x6f3, 0x6ee, 0x6e9, 0x6e5, 0x6e0,
    0x6db, 0x6d6, 0x6d2, 0x6cd, 0x6c8, 0x6c4, 0x6bf, 0x6ba,
    0x6b5, 0x6b1, 0x6ac, 0x6a8, 0x6a3, 0x69e, 0x69a, 0x695,
    0x691, 0x68c, 0x688, 0x683, 0x67f, 0x67a, 0x676, 0x671,
    0x66d, 0x668, 0x664, 0x65f, 0x65b, 0x657, 0x652, 0x64e,
    0x649, 0x645, 0x641, 0x63c, 0x638, 0x634, 0x630, 0x62b,
    0x627, 0x623, 0x61e, 0x61a, 0x616, 0x612, 0x60e, 0x609,
    0x605, 0x601, 0x5fd, 0x5f9, 0x5f5, 0x5f0, 0x5ec, 0x5e8,
    0x5e4, 0x5e0, 0x5dc, 0x5d8, 0x5d4, 0x5d0, 0x5cc, 0x5c8,
    0x5c4, 0x5c0, 0x5bc, 0x5b8, 0x5b4, 0x5b0, 0x5ac, 0x5a8,
    0x5a4, 0x5a0, 0x59c, 0x599, 0x595, 0x591, 0x58d, 0x589,
    0x585, 0x581, 0x57e, 0x57a, 0x576, 0x572, 0x56f, 0x56b,
    0x567, 0x563, 0x560, 0x55c, 0x558, 0x554, 0x551, 0x54d,
    0x549, 0x546, 0x542, 0x53e, 0x53b, 0x537, 0x534, 0x530,
    0x52c, 0x529, 0x525, 0x522, 0x51e, 0x51b, 0x517, 0x514,
    0x510, 0x50c, 0x509, 0x506, 0x502, 0x4ff, 0x4fb, 0x4f8,
    0x4f4, 0x4f1, 0x4ed, 0x4ea, 0x4e7, 0x4e3, 0x4e0, 0x4dc,
    0x4d9, 0x4d6, 0x4d2, 0x4cf, 0x4cc, 0x4c8, 0x4c5, 0x4c2,
    0x4be, 0x4bb, 0x4b8, 0x4b5, 0x4b1, 0x4ae, 0x4ab, 0x4a8,
    0x4a4, 0x4a1, 0x49e, 0x49b, 0x498, 0x494, 0x491, 0x48e,
    0x48b, 0x488, 0x485, 0x482, 0x47e, 0x47b, 0x478, 0x475,
    0x472, 0x46f, 0x46c, 0x469, 0x466, 0x463, 0x460, 0x45d,
    0x45a, 0x457, 0x454, 0x451, 0x44e, 0x44b, 0x448, 0x445,
    0x442, 0x43f, 0x43c, 0x439, 0x436, 0x433, 0x430, 0x42d,
    0x42a, 0x428, 0x425, 0x422, 0x41f, 0x41c, 0x419, 0x416,
    0x414, 0x411, 0x40e, 0x40b, 0x408, 0x406, 0x403, 0x400
};

/* Note table */
static const uint32_t fn_note[16] = {
    0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 3, 3, 3, 3, 3, 3
};

/* Envelope generator */
static const uint32_t eg_stephi[4][4] = {
    { 0, 0, 0, 0 },
    { 1, 0, 0, 0 },
    { 1, 0, 1, 0 },
    { 1, 1, 1, 0 }
};

static const uint8_t eg_am_shift[4] = {
    7, 3, 1, 0
};

/* Phase generator */
static const uint32_t pg_detune[8] = { 16, 17, 19, 20, 22, 24, 27, 29 };

static const uint32_t pg_lfo_sh1[8][8] = {
    { 7, 7, 7, 7, 7, 7, 7, 7 },
    { 7, 7, 7, 7, 7, 7, 7, 7 },
    { 7, 7, 7, 7, 7, 7, 1, 1 },
    { 7, 7, 7, 7, 1, 1, 1, 1 },
    { 7, 7, 7, 1, 1, 1, 1, 0 },
    { 7, 7, 1, 1, 0, 0, 0, 0 },
    { 7, 7, 1, 1, 0, 0, 0, 0 },
    { 7, 7, 1, 1, 0, 0, 0, 0 }
};

static const uint32_t pg_lfo_sh2[8][8] = {
    { 7, 7, 7, 7, 7, 7, 7, 7 },
    { 7, 7, 7, 7, 2, 2, 2, 2 },
    { 7, 7, 7, 2, 2, 2, 7, 7 },
    { 7, 7, 2, 2, 7, 7, 2, 2 },
    { 7, 7, 2, 7, 7, 7, 2, 7 },
    { 7, 7, 7, 2, 7, 7, 2, 1 },
    { 7, 7, 7, 2, 7, 7, 2, 1 },
    { 7, 7, 7, 2, 7, 7, 2, 1 }
};

/* Address decoder */
static const uint32_t slot_map[16] = {
    0x000, /* Ch1 OP1/OP2 */
    0x001, /* Ch2 OP1/OP2 */
    0x002, /* Ch3 OP1/OP2 */
    0x003, /* Illegal */
    0x100, /* Ch4 OP1/OP2 */
    0x101, /* Ch5 OP1/OP2 */
    0x102, /* Ch6 OP1/OP2 */
    0x103, /* Illegal */
    0x004, /* Ch1 OP3/OP4 */
    0x005, /* Ch2 OP3/OP4 */
    0x006, /* Ch3 OP3/OP4 */
    0x007, /* Illegal */
    0x104, /* Ch4 OP3/OP4 */
    0x105, /* Ch5 OP3/OP4 */
    0x106  /* Ch6 OP3/OP4 */
    0x107, /* Illegal */
};

/* LFO */
static const uint32_t lfo_cycles[8] = {
    108, 77, 71, 67, 62, 44, 8, 5
};

/* FM algorithm */
static const uint32_t fm_algorithm[4][6][8] = {
    {
        { 1, 1, 1, 1, 1, 1, 1, 1 }, /* OP1_0         */
        { 1, 1, 1, 1, 1, 1, 1, 1 }, /* OP1_1         */
        { 0, 0, 0, 0, 0, 0, 0, 0 }, /* OP2           */
        { 0, 0, 0, 0, 0, 0, 0, 0 }, /* Last operator */
        { 0, 0, 0, 0, 0, 0, 0, 0 }, /* Last operator */
        { 0, 0, 0, 0, 0, 0, 0, 1 }  /* Out           */
    },
    {
        { 0, 1, 0, 0, 0, 1, 0, 0 }, /* OP1_0         */
        { 0, 0, 0, 0, 0, 0, 0, 0 }, /* OP1_1         */
        { 1, 1, 1, 0, 0, 0, 0, 0 }, /* OP2           */
        { 0, 0, 0, 0, 0, 0, 0, 0 }, /* Last operator */
        { 0, 0, 0, 0, 0, 0, 0, 0 }, /* Last operator */
        { 0, 0, 0, 0, 0, 1, 1, 1 }  /* Out           */
    },
    {
        { 0, 0, 0, 0, 0, 0, 0, 0 }, /* OP1_0         */
        { 0, 0, 0, 0, 0, 0, 0, 0 }, /* OP1_1         */
        { 0, 0, 0, 0, 0, 0, 0, 0 }, /* OP2           */
        { 1, 0, 0, 1, 1, 1, 1, 0 }, /* Last operator */
        { 0, 0, 0, 0, 0, 0, 0, 0 }, /* Last operator */
        { 0, 0, 0, 0, 1, 1, 1, 1 }  /* Out           */
    },
    {
        { 0, 0, 1, 0, 0, 1, 0, 0 }, /* OP1_0         */
        { 0, 0, 0, 0, 0, 0, 0, 0 }, /* OP1_1         */
        { 0, 0, 0, 1, 0, 0, 0, 0 }, /* OP2           */
        { 1, 1, 0, 1, 1, 0, 0, 0 }, /* Last operator */
        { 0, 0, 1, 0, 0, 0, 0, 0 }, /* Last operator */
        { 1, 1, 1, 1, 1, 1, 1, 1 }  /* Out           */
    }
};

static void OPNB_FMDoIO1(opnb_t *chip)
{
    chip->write_d_en = (chip->write_d & 20) == 4;
    chip->write_a_en = (chip->write_a & 20) == 4;
    chip->write_d <<= 1;
    chip->write_a <<= 1;
}

static void OPNB_FMDoIO2(opnb_t *chip)
{
    chip->write_d <<= 1;
    chip->write_a <<= 1;
}

static void OPNB_DoRegWrite(opnb_t *chip)
{
    uint32_t i, bank, slot, channel;
    uint16_t bus, businv, addr;

    if (chip->ic_latch & 2)
    {
        bus = 0;
        businv = (chip->write_data ^ 255) & 255;
    }
    else
    {
        bus = chip->write_data & 255;
        businv = bus ^ 255;
    }
    bus |= chip->write_data & 256;
    /* Update registers */
    if (chip->write_fm_data)
    {
        /* Slot */
        if (slot_map[chip->slot_counter_h * 4 + chip->slot_counter_l] == (chip->fm_address & 0x107))
        {
            slot = chip->fmcycles % 12;
            bank = (chip->fm_address & 8) != 0;
            switch (chip->fm_address & 0xf0)
            {
            case 0x30: /* DT, MULTI */
                chip->fm_multi[bank][slot] = chip->fm_data & 0x0f;
                chip->fm_dt[bank][slot] = (chip->fm_data >> 4) & 0x07;
                break;
            case 0x40: /* TL */
                chip->fm_tl[bank][slot] = chip->fm_data & 0x7f;
                break;
            case 0x50: /* KS, AR */
                chip->fm_ar[bank][slot] = chip->fm_data & 0x1f;
                chip->fm_ks[bank][slot] = (chip->fm_data >> 6) & 0x03;
                break;
            case 0x60: /* AM, DR */
                chip->fm_dr[bank][slot] = chip->fm_data & 0x1f;
                chip->fm_am[bank][slot] = (chip->fm_data >> 7) & 0x01;
                break;
            case 0x70: /* SR */
                chip->fm_sr[bank][slot] = chip->fm_data & 0x1f;
                break;
            case 0x80: /* SL, RR */
                chip->fm_rr[bank][slot] = chip->fm_data & 0x0f;
                chip->fm_sl[bank][slot] = (chip->fm_data >> 4) & 0x0f;
                break;
            case 0x90: /* SSG-EG */
                chip->fm_ssg_eg[bank][slot] = chip->fm_data & 0x0f;
                break;
            }
        }
        /* Channel */
        if (slot_map[(chip->slot_counter_h & 1) * 4 + chip->slot_counter_l] == (chip->fm_address & 0x103))
        {
            channel = chip->fmcycles % 6;
            switch (chip->fm_address & 0xfc)
            {
            case 0xa0:
                chip->fm_fnum[channel] = (chip->fm_data & 0xff) | ((chip->fm_reg_a4 & 0x07) << 8);
                chip->fm_block[channel] = (chip->fm_reg_a4 >> 3) & 0x07;
                break;
            case 0xa4:
                chip->fm_reg_a4 = chip->fm_data & 0x3f;
                break;
            case 0xa8:
                chip->fm_fnum_3ch[channel] = (chip->fm_data & 0xff) | ((chip->fm_reg_ac & 0x07) << 8);
                chip->fm_block_3ch[channel] = (chip->fm_reg_ac >> 3) & 0x07;
                break;
            case 0xac:
                chip->fm_reg_ac = chip->fm_data & 0x3f;
                break;
            case 0xb0:
                chip->fm_connect[channel] = chip->fm_data & 0x07;
                chip->fm_fb[channel] = (chip->fm_data >> 3) & 0x07;
                break;
            case 0xb4:
                chip->fm_pms[channel] = chip->fm_data & 0x07;
                chip->fm_ams[channel] = (chip->fm_data >> 4) & 0x07;
                chip->fm_pan_l[channel] = (chip->fm_data >> 7) & 0x01;
                chip->fm_pan_r[channel] = (chip->fm_data >> 6) & 0x01;
                break;
            }
        }
    }
    /* Data */
    if (chip->write_d_en)
    {
        if ((bus & 256) == 0)
        {
            if (chip->write_mode_1c)
            {
                chip->mode_1c = bus & 191;
            }
            if (chip->write_mode_27) /* CSM, Timer control */
            {
                chip->mode_ch3 = (bus & 0xc0) >> 6;
                chip->mode_csm = chip->mode_ch3 == 2;
                chip->timer_a_load = bus & 0x01;
                chip->timer_a_enable = (bus >> 2) & 0x01;
                chip->timer_a_reset = (bus >> 4) & 0x01;
                chip->timer_b_load = (bus >> 1) & 0x01;
                chip->timer_b_enable = (bus >> 3) & 0x01;
                chip->timer_b_reset = (bus >> 5) & 0x01;
            }
            if (chip->write_mode_25) /* Timer A */
            {
                chip->timer_a_reg &= 0x3fc;
                chip->timer_a_reg |= bus & 0x03;
            }
            if (chip->write_mode_24)
            {
                chip->timer_a_reg &= 0x03;
                chip->timer_a_reg |= (bus & 0xff) << 2;
            }
            if (chip->write_mode_21) /* LSI test */
            {
                chip->mode_test = bus & 253;
            }
            if (chip->write_mode_28) /* Key on/off */
            {
                for (i = 0; i < 4; i++)
                {
                    chip->mode_kon_operator[i] = (bus >> (4 + i)) & 0x01;
                }
                chip->mode_kon_channel = bus & 0x07;
            }
            if (chip->write_mode_26) /* Timer B */
            {
                chip->timer_b_reg = bus & 255;
            }
            if (chip->write_mode_22) /* LFO control */
            {
                if ((bus >> 3) & 0x01)
                {
                    chip->lfo_en = 0x7f;
                }
                else
                {
                    chip->lfo_en = 0;
                }
                chip->lfo_freq = bus & 0x07;
            }
            if (chip->write_mode_02) /* ADPCM LSI Test */
            {
                chip->mode_test2 = bus & 184;
            }
        }
        if (chip->write_fm_address)
        {
            chip->fm_data = bus;
        }
    }
    if (chip->write_a_en)
    {
        chip->write_fm_data = 0;
    }
    if (chip->write_fm_address && chip->write_d_en)
    {
        chip->write_fm_data = 1;
    }
    /* Address */
    if (chip->write_a_en)
    {
        chip->write_fm_address = (bus & 0xf0) != 0;
        if (chip->write_fm_address)
        {
            chip->fm_address = bus & 0x1ff;
        }
        if (chip->ic_latch & 2)
        {
            chip->write_mode_1c = (bus & 0x100) == 0 && (businv & 0x1c) == 0;
            chip->write_mode_27 = (bus & 0x100) == 0 && (businv & 0x27) == 0;
            chip->write_mode_25 = (bus & 0x100) == 0 && (businv & 0x25) == 0;
            chip->write_mode_24 = (bus & 0x100) == 0 && (businv & 0x24) == 0;
            chip->write_mode_21 = (bus & 0x100) == 0 && (businv & 0x21) == 0;
            chip->write_mode_28 = (bus & 0x100) == 0 && (businv & 0x28) == 0;
            chip->write_mode_26 = (bus & 0x100) == 0 && (businv & 0x26) == 0;
            chip->write_mode_22 = (bus & 0x100) == 0 && (businv & 0x22) == 0;
            chip->write_mode_02 = (bus & 0x100) == 0 && (businv & 0x02) == 0;
        }
        else
        {
            chip->write_mode_1c = bus == 0x1c;
            chip->write_mode_27 = bus == 0x27;
            chip->write_mode_25 = bus == 0x25;
            chip->write_mode_24 = bus == 0x24;
            chip->write_mode_21 = bus == 0x21;
            chip->write_mode_28 = bus == 0x28;
            chip->write_mode_26 = bus == 0x26;
            chip->write_mode_22 = bus == 0x22;
            chip->write_mode_02 = bus == 0x02;
        }
    }
    if (chip->slot_counter_l & 2)
    {
        chip->slot_counter_h = (chip->slot_counter_h + 1) & 3;
        chip->slot_counter_l = 0;
    }
    else
    {
        chip->slot_counter_l++;
    }
    if (chip->fmcycles[22] & 2)
    {
        chip->slot_counter_h = 0;
        chip->slot_counter_l = 0;
    }
    if (chip->ic)
    {
        chip->mode_1c = 0;
        chip->mode_ch3 = 0;
        chip->mode_csm = 0;
        chip->timer_a_load = 0;
        chip->timer_a_enable = 0;
        chip->timer_a_reset = 0;
        chip->timer_b_load = 0;
        chip->timer_b_enable = 0;
        chip->timer_b_reset = 0;
        chip->timer_a_reg = 0;
        chip->mode_test = 0;
        chip->mode_kon_operator[0] = 0;
        chip->mode_kon_operator[1] = 0;
        chip->mode_kon_operator[2] = 0;
        chip->mode_kon_operator[3] = 0;
        chip->mode_kon_channel = 0;
        chip->timer_b_reg = 0;
        chip->lfo_en = 0;
        chip->lfo_freq = 0;
        chip->mode_test2 = 0;
        chip->fm_address = 0;
        chip->fm_data = 0;
        chip->slot_counter_h = 0;
        chip->slot_counter_l = 0;
        slot = chip->fmcycles % 12;
        channel = chip->fmcycles % 6;
        chip->fm_reg_a4 = 0;
        chip->fm_reg_ac = 0;
        chip->fm_fnum[channel] = 0;
        chip->fm_block[channel] = 0;
        chip->fm_fnum_3ch[channel] = 0;
        chip->fm_block_3ch[channel] = 0;
        chip->fm_connect[channel] = 0;
        chip->fm_fb[channel] = 0;
        chip->fm_pms[channel] = 0;
        chip->fm_ams[channel] = 0;
        chip->fm_pan_l[channel] = 1;
        chip->fm_pan_r[channel] = 1;
        chip->fm_ssg_eg[0][slot] = 0;
        chip->fm_ssg_eg[1][slot] = 0;
        chip->fm_dt[0][slot] = 0;
        chip->fm_dt[1][slot] = 0;
        chip->fm_multi[0][slot] = 0;
        chip->fm_multi[1][slot] = 0;
        chip->fm_tl[0][slot] = 0;
        chip->fm_tl[1][slot] = 0;
        chip->fm_ks[0][slot] = 0;
        chip->fm_ks[1][slot] = 0;
        chip->fm_ar[0][slot] = 0;
        chip->fm_ar[1][slot] = 0;
        chip->fm_am[0][slot] = 0;
        chip->fm_am[1][slot] = 0;
        chip->fm_dr[0][slot] = 0;
        chip->fm_dr[1][slot] = 0;
        chip->fm_sr[0][slot] = 0;
        chip->fm_sr[1][slot] = 0;
        chip->fm_rr[0][slot] = 0;
        chip->fm_rr[1][slot] = 0;
        chip->fm_sl[0][slot] = 0;
        chip->fm_sl[1][slot] = 0;
    }
}

static void OPNB_FMCounter1(opnb_t *chip)
{
    if (chip->fmstate_l & 2)
    {
        chip->fmstate_h = (chip->fmstate_h + 1) & 7;
        chip->fmstate_l = 0;
    }
    else
    {
        chip->fmstate_l++;
    }
    if (chip->ic2 & 8)
    {
        chip->fmstate_h = 0;
        chip->fmstate_l = 0;
    }
}

static void OPNB_FMCounter2(opnb_t* chip)
{
    chip->fmcycle[22] <<= 1;
    chip->fmcycle[22] |= chip->fmstate_h == 7 && chip->fmstate_l == 1;
}

static void OPNB_FMClock1(opnb_t *chip)
{
    chip->ic_latch |= chip->ic;
    OPNB_FMDoIO1(chip);
    OPNB_DoRegWrite(chip);
    OPNB_FMCounter1(chip);
}

static void OPNB_FMClock2(opnb_t *chip)
{
    chip->ic_latch <<= 1;
    OPNB_FMDoIO2(chip);
    chip->fmcycles = (chip->fmcycles + 1) % 24;

}

static uint8_t OPNB_ReadBus(opnb_t *chip, uint32_t ssgread)
{
    if (chip->ic_latch & 2)
    {
        return 0;
    }
    if (ssgread)
    {
        return 0; // TODO:
    }
    return chip->write_data & 255;
}

void OPNB_Clock(opnb_t *chip)
{
    uint32_t cycle;
    if (chip->ic)
    {
        chip->write_data = 0;
    }
    for (cycle = 0; cycle < 12; cycle++)
    {
        if (chip->cycles & (1 << cycle))
        {
            // FM clock
            switch (cycle % 6)
            {
            case 2: // rising
            case 3:
                if (!(chip->clock_state & clock_fm_1))
                {
                    OPNB_FMClock1(chip);
                    chip->clock_state |= clock_fm_1;
                }
                break;
            case 4: // falling
                chip->clock_state &= ~clock_fm_1;
                break;
            case 0: // rising
            case 5:
            case 3:
                if (!(chip->clock_state & clock_fm_2))
                {
                    OPNB_FMClock2(chip);
                    chip->clock_state |= clock_fm_2;
                }
                break;
            case 1: // falling
                chip->clock_state &= ~clock_fm_2;
                break;
            default:
                break;
            }
        }
    }

    // Cycle one-hot
    chip->cycles <<= 1;
    if ((chip->cycles & 0xffe) == 0)
    {
        chip->cycles |= 1;
    }
    chip->ic2 <<= 1;
    if ((chip->ic_sr & 1) && !(chip->ic_sr & (1<<18)))
    {
        chip->cycles &= ~1;
        chip->ic2 |= 1;
    }
    chip->ic_sr <<= 1;
    chip->ic_sr |= chip->ic;
}

void OPNB_Write(opnb_t *chip, uint32_t port, uint8_t data)
{
    port &= 3;
    chip->write_data = ((port << 7) & 0x100) | data;
    if (port & 1)
    {
        if (!(chip->write_d & 4))
        {
            chip->write_d |= 1;
        }
    }
    else
    {
        if (!(chip->write_a & 4))
        {
            chip->write_a |= 1;
        }
    }
}

uint8_t OPNB_Read(opnb_t *chip, uint32_t port)
{
    uint8_t ret = 0;
    switch (port & 3)
    {
    case 1:
        ret = OPNB_ReadBus(chip, 1);
        break;
    default:
        break;
    }
    return ret;
}
