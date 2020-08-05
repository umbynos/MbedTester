/*
 * Copyright (c) 2019, Arm Limited and affiliates.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "utest/utest.h"
#include "unity/unity.h"
#include "greentea-client/test_env.h"
#include "mbed.h"
#include "pinmap.h"
#include "hal/static_pinmap.h"
#include "test_utils.h"
#include "MbedTester.h"

using namespace utest::v1;

#define analogin_debug_printf(...)

#define DELTA_FLOAT                     (0.1f)       // 10%
#define DELTA_U16                       (2*3277)     // 10%

static const PinName ff_arduino_pins[] = {
    digitalPinToPinName(D0), digitalPinToPinName(D1), digitalPinToPinName(D2), digitalPinToPinName(D3), digitalPinToPinName(D4), digitalPinToPinName(D5), digitalPinToPinName(D6), digitalPinToPinName(D7),
    digitalPinToPinName(D8), digitalPinToPinName(D9), digitalPinToPinName(D10), digitalPinToPinName(D11), digitalPinToPinName(D12), digitalPinToPinName(D13),
    digitalPinToPinName(A0), digitalPinToPinName(A1), digitalPinToPinName(A2), digitalPinToPinName(A3), digitalPinToPinName(A4), digitalPinToPinName(A5), digitalPinToPinName(A6), digitalPinToPinName(A7)
};

static const char *ff_arduino_names[] = {
    "D0", "D1", "D2", "D3", "D4", "D5", "D6", "D7",
    "D8", "D9", "D10", "D11", "D12", "D13" ,
    "A0", "A1", "A2", "A3", "A4", "A5" , "A6" , "A7"
};

static const PinList ff_arduino_list = {
    sizeof(ff_arduino_pins) / sizeof(ff_arduino_pins[0]),
    ff_arduino_pins
};

MBED_STATIC_ASSERT(sizeof(ff_arduino_pins) / sizeof(ff_arduino_pins[0]) == sizeof(ff_arduino_names) / sizeof(ff_arduino_names[0]),
                   "Arrays must have the same length");

const PinList *pinmap_ff_arduino_pins()
{
    return &ff_arduino_list;
}

const char *pinmap_ff_arduino_pin_to_string(PinName pin)
{
    if (pin == NC) {
        return "NC";
    }
    for (size_t i = 0; i < ff_arduino_list.count; i++) {
        if (ff_arduino_list.pins[i] == pin) {
            return ff_arduino_names[i];
        }
    }
    return "Unknown";
}

const PinList *form_factor = pinmap_ff_arduino_pins();
const PinList *restricted = pinmap_restricted_pins();

MbedTester tester(form_factor, restricted);

void fpga_analogin_init_test(PinName pin)
{
    analogin_t analogin;

    analogin_init(&analogin, pin);
    analogin_free(&analogin);
}

template<bool init_direct>
void fpga_analogin_test(PinName pin)
{
    tester.reset();
    tester.pin_map_set(pin, MbedTester::LogicalPinGPIO0);
    tester.select_peripheral(MbedTester::PeripheralGPIO);

    /* Test analog input */

    analogin_t analogin;

    if (init_direct) {
        const PinMap pinmap = get_analogin_pinmap(pin);
        analogin_init_direct(&analogin, &pinmap);
    } else {
        analogin_init(&analogin, pin);
    }

    tester.gpio_write(MbedTester::LogicalPinGPIO0, 1, true);
    TEST_ASSERT_FLOAT_WITHIN(DELTA_FLOAT, 1.0f, analogin_read(&analogin));
    TEST_ASSERT_UINT16_WITHIN(DELTA_U16, 65535, analogin_read_u16(&analogin));

    tester.gpio_write(MbedTester::LogicalPinGPIO0, 0, true);
    TEST_ASSERT_FLOAT_WITHIN(DELTA_FLOAT, 0.0f, analogin_read(&analogin));
    TEST_ASSERT_UINT16_WITHIN(DELTA_U16, 0, analogin_read_u16(&analogin));

    /* Set gpio back to Hi-Z */
    tester.gpio_write(MbedTester::LogicalPinGPIO0, 0, false);

    analogin_free(&analogin);
}

Case cases[] = {
    // This will be run for all pins
    Case("AnalogIn - init test", all_ports<AnaloginPort, DefaultFormFactor, fpga_analogin_init_test>),
    // This will be run for single pin
    Case("AnalogIn - read test", all_ports<AnaloginPort, DefaultFormFactor, fpga_analogin_test<false>>),
    Case("AnalogIn (direct init) - read test", all_ports<AnaloginPort, DefaultFormFactor, fpga_analogin_test<true>>),
};

utest::v1::status_t greentea_test_setup(const size_t number_of_cases)
{
    GREENTEA_SETUP(120, "default_auto");
    return greentea_test_setup_handler(number_of_cases);
}

Specification specification(greentea_test_setup, cases, greentea_test_teardown_handler);

void setup(){
    Harness::run(specification);
}

void loop(){

}