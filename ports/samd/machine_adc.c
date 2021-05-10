/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Meurisse D. (MCHobby)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#include <stdio.h>

// #include "driver/gpio.h"
// #include "driver/adc.h"

#include "py/runtime.h"
#include "py/mphal.h"
#include "samd21.h"
#include "modmachine.h"
#include "machine_adc.h"
#include "machine_pin.h"

// --- ADC helper ----------------------------------------------------------------------

void adc_initialize(){
    // Enable APB Clock for ADC
    PM->APBCMASK.reg |= PM_APBCMASK_ADC;
    // Enable GCLK1 for ADC
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK1 | GCLK_CLKCTRL_ID_ADC ;
    // Wait for Synch
    while (GCLK->STATUS.bit.SYNCBUSY) {};
    // Reload calibration data
    uint32_t bias = (*((uint32_t *) ADC_FUSES_BIASCAL_ADDR) & ADC_FUSES_BIASCAL_Msk) >> ADC_FUSES_BIASCAL_Pos;
    uint32_t linearity = (*((uint32_t *) ADC_FUSES_LINEARITY_0_ADDR) & ADC_FUSES_LINEARITY_0_Msk) >> ADC_FUSES_LINEARITY_0_Pos;
    linearity |= ((*((uint32_t *) ADC_FUSES_LINEARITY_1_ADDR) & ADC_FUSES_LINEARITY_1_Msk) >> ADC_FUSES_LINEARITY_1_Pos) << 5;
    // Wait for Synch
    while (GCLK->STATUS.bit.SYNCBUSY) {};
    // Write calibration data
    ADC->CALIB.reg = ADC_CALIB_BIAS_CAL(bias) | ADC_CALIB_LINEARITY_CAL(linearity); 
}


// --- ADC class implementation --------------------------------------------------------
STATIC uint8_t adc_bit_width;

STATIC mp_obj_t madc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw,
    const mp_obj_t *args) {

    static int initialized = 0;
    if (!initialized) {
        adc_initialize();
//        #if CONFIG_IDF_TARGET_ESP32S2
//        adc1_config_width(ADC_WIDTH_BIT_13);
//        #else
//        adc1_config_width(ADC_WIDTH_BIT_12);
//        #endif
        adc_bit_width = 12;
        initialized = 1;
    }

    mp_arg_check_num(n_args, n_kw, 1, 1, true);
    gpio_num_t pin_id = machine_pin_get_id(args[0]);
    //mp_printf( MP_PYTHON_PRINTER, "DBG: pin_id %u", pin_id );
    const madc_obj_t *self = NULL;
    for (int i = 0; i < MP_ARRAY_SIZE(madc_obj); i++) {
        if (pin_id == madc_obj[i].gpio_id) {
            self = &madc_obj[i];
            break;
        }
    }
    if (!self) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid Pin for ADC"));
    }
// TODO: 
//    esp_err_t err = adc1_config_channel_atten(self->adc1_id, ADC_ATTEN_0db);
//    if (err == ESP_OK) {
        return MP_OBJ_FROM_PTR(self);
//    }
//    mp_raise_ValueError(MP_ERROR_TEXT("parameter error"));
}

STATIC void madc_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    madc_obj_t *self = self_in;
    mp_printf(print, "ADC(Pin(%u))", self->gpio_id);
}

// read_u16()
STATIC mp_obj_t madc_read_u16(mp_obj_t self_in) {
    // TODO: madc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    // TODO: uint32_t raw = adc1_get_raw(self->adc1_id);
    // Scale raw reading to 16 bit value using a Taylor expansion (for 8 <= bits <= 16)
    uint32_t u16 = 0; //TODO: raw << (16 - adc_bit_width) | raw >> (2 * adc_bit_width - 16);
    return MP_OBJ_NEW_SMALL_INT(u16);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(madc_read_u16_obj, madc_read_u16);

// Legacy method
STATIC mp_obj_t madc_read(mp_obj_t self_in) {
    //madc_obj_t *self = self_in;
    int val = 0; //TODO: adc1_get_raw(self->adc1_id);
    if (val == -1) {
        mp_raise_ValueError(MP_ERROR_TEXT("parameter error"));
    }
    return MP_OBJ_NEW_SMALL_INT(val);
}
MP_DEFINE_CONST_FUN_OBJ_1(madc_read_obj, madc_read);

STATIC mp_obj_t madc_atten(mp_obj_t self_in, mp_obj_t atten_in) {
    //madc_obj_t *self = self_in;
    //adc_atten_t atten = mp_obj_get_int(atten_in);
    //esp_err_t err = adc1_config_channel_atten(self->adc1_id, atten);
    //if (err == ESP_OK) {
    //    return mp_const_none;
    //}
    mp_raise_ValueError(MP_ERROR_TEXT("parameter error"));
}
MP_DEFINE_CONST_FUN_OBJ_2(madc_atten_obj, madc_atten);

STATIC mp_obj_t madc_width(mp_obj_t cls_in, mp_obj_t width_in) {
    // TODO: adc_bits_width_t width = mp_obj_get_int(width_in);
    // TODO:
    //esp_err_t err = adc1_config_width(width);
    //if (err != ESP_OK) {
    //    mp_raise_ValueError(MP_ERROR_TEXT("parameter error"));
    //}
    //switch (width) {
    //    #if CONFIG_IDF_TARGET_ESP32
    //    case ADC_WIDTH_9Bit:
    //        adc_bit_width = 9;
    //        break;
    //    case ADC_WIDTH_10Bit:
    //        adc_bit_width = 10;
    //        break;
    //    case ADC_WIDTH_11Bit:
    //        adc_bit_width = 11;
    //        break;
    //    case ADC_WIDTH_12Bit:
    //        adc_bit_width = 12;
    //        break;
    //    #elif CONFIG_IDF_TARGET_ESP32S2
    //    case ADC_WIDTH_BIT_13:
    //        adc_bit_width = 13;
    //        break;
    //        #endif
    //    default:
    //        break;
    //}
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(madc_width_fun_obj, madc_width);
MP_DEFINE_CONST_CLASSMETHOD_OBJ(madc_width_obj, MP_ROM_PTR(&madc_width_fun_obj));

STATIC const mp_rom_map_elem_t madc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_read_u16), MP_ROM_PTR(&madc_read_u16_obj) },

    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&madc_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_atten), MP_ROM_PTR(&madc_atten_obj) },
    { MP_ROM_QSTR(MP_QSTR_width), MP_ROM_PTR(&madc_width_obj) },

    //{ MP_ROM_QSTR(MP_QSTR_ATTN_0DB), MP_ROM_INT(ADC_ATTEN_0db) },
    //{ MP_ROM_QSTR(MP_QSTR_ATTN_2_5DB), MP_ROM_INT(ADC_ATTEN_2_5db) },
    //{ MP_ROM_QSTR(MP_QSTR_ATTN_6DB), MP_ROM_INT(ADC_ATTEN_6db) },
    //{ MP_ROM_QSTR(MP_QSTR_ATTN_11DB), MP_ROM_INT(ADC_ATTEN_11db) },

    //{ MP_ROM_QSTR(MP_QSTR_WIDTH_9BIT), MP_ROM_INT(ADC_WIDTH_9Bit) },
    //{ MP_ROM_QSTR(MP_QSTR_WIDTH_10BIT), MP_ROM_INT(ADC_WIDTH_10Bit) },
    //{ MP_ROM_QSTR(MP_QSTR_WIDTH_11BIT), MP_ROM_INT(ADC_WIDTH_11Bit) },
    { MP_ROM_QSTR(MP_QSTR_WIDTH_12BIT), MP_ROM_INT(ADC_WIDTH_12BIT) },
};

STATIC MP_DEFINE_CONST_DICT(madc_locals_dict, madc_locals_dict_table);

const mp_obj_type_t machine_adc_type = {
    { &mp_type_type },
    .name = MP_QSTR_ADC,
    .print = madc_print,
    .make_new = madc_make_new,
    .locals_dict = (mp_obj_t)&madc_locals_dict,
};