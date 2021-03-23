#ifndef _I2C_CONTROL_PANEL_ASUKIAAA_HPP_
#define _I2C_CONTROL_PANEL_ASUKIAAA_HPP_

#include "I2cControlPanel_asukiaaa_template.hpp"
#include <Wire.h>

namespace I2cControlPanel_asukiaaa {

template <>
void DriverTemplate<TwoWire>::begin() {
  if (wire == NULL) {
    wire = &Wire;
    wire->begin();
  }
}

typedef DriverTemplate<TwoWire> Driver;

}  // namespace I2cControlPanel_asukiaaa

#endif
