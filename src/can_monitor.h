#pragma once
#include <Arduino.h>
#include "driver/gpio.h"

bool canInit(gpio_num_t tx, gpio_num_t rx);
String canJson();   // letzter Frame pro ID + Bus-Status als JSON