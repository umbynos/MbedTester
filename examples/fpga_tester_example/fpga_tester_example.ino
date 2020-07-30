#include "mbed.h"
#include "MbedTester.h"

using namespace rtos;

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

void setup() {
  Serial.begin(9600);
  delay(2000);

  // Maually set control pins to comunicate with the FPGA via SPI (13, 11, 12, 10)
  tester.set_control_pins_manual(SPI_SCK, SPI_MOSI, SPI_MISO, digitalPinToPinName(PIN_SPI_SS));

  // Test if the connection with the FPGA is successfull
  Serial.print("Version: ");
  Serial.println(tester.version());

  // Reset the FPGA CI Test Shield to put it into a known state
  tester.reset();

  // Select the GPIO peripheral
  tester.select_peripheral(MbedTester::PeripheralGPIO);

  // Map D6 to LogicalPinGPIO0
  tester.pin_map_set(digitalPinToPinName(D6), MbedTester::LogicalPinGPIO0);

  // Toggle pin D6
  int toggle = 0;
  while (1) {
      tester.gpio_write(MbedTester::LogicalPinGPIO0, toggle, true);
      ThisThread::sleep_for(500);
      toggle = !toggle;
  }
}

void loop(){

}