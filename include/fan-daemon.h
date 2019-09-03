#ifndef FAN_CONTROL_H_INCLUDED
#define FAN_CONTROL_H_INCLUDED

#include <algorithm>
#include <chrono>
#include <csignal>
#include <fstream>
#include <glob.h>
#include <iostream>
#include <thread>

// the following three defines can be modified to change the daemon behaviour
#define FAN_OFF_TEMP 20   // oC
#define FAN_MAX_TEMP 60   // oC
#define UPDATE_INTERVAL 2 // second
// end of modifiable code

#define JETSON_CLOCKS "/usr/bin/jetson_clocks"
#define THERMAL_ZONE_GLOB "/sys/devices/virtual/thermal/thermal_zone*/temp"
#define PWM_CAP "/sys/devices/pwm-fan/pwm_cap"
#define TARGET_PWM "/sys/devices/pwm-fan/target_pwm"

void exit_handler(int s);
void init_exit_handler();
unsigned readIntSysFs(std::string);
void writeIntSysFs(std::string path, unsigned value);
// int getPwmCap();
int readAverageTemp();
unsigned adjustFanSpeed(int tempe, int fanOffTempe = FAN_OFF_TEMP, int fanMaxTempe = FAN_MAX_TEMP);
int thermalControl(int updateInterval_s = UPDATE_INTERVAL);

#endif // FAN_CONTROL_H_INCLUDED
