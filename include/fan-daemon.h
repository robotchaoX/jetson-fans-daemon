#ifndef FAN_CONTROL_H_INCLUDED
#define FAN_CONTROL_H_INCLUDED

#include <algorithm>
#include <chrono>
#include <csignal>
#include <cstring>
#include <fstream>
#include <glob.h>
#include <iostream>
#include <thread>

// the following three defines can be modified to change the daemon behaviour
#define FAN_OFF_TEMP 35   // oC
#define FAN_MAX_TEMP 60   // oC
#define UPDATE_INTERVAL 2 // second
// end of modifiable code

// 读写设备文件需root权限,否则
#define JETSON_CLOCKS "/usr/bin/jetson_clocks"
#define PWM_CAP "/sys/devices/pwm-fan/pwm_cap"
#define THERMAL_ZONE_GLOB "/sys/devices/virtual/thermal/thermal_zone*/temp"
#define TARGET_PWM "/sys/devices/pwm-fan/target_pwm"

// 监控温度，控制风扇开关
// 每隔2秒就检查一下CPU的温度,需单独线程。
// 如果CPU温度达到FAN_OFF_TEMP极其以上时，就打开风扇。

void exit_handler(int sig);
void init_exit_handler();
int readIntSysFs(std::string path);
int writeIntSysFs(std::string path, unsigned value);
// API
int getPwmCap();
int readFanSpeed();
int setFanSpeed(unsigned speedValue);
float readAverageTemp();
unsigned adjustFanSpeed(float tempe, int fanOffTempe = FAN_OFF_TEMP, int fanMaxTempe = FAN_MAX_TEMP);
int thermalControl(int updateInterval_s = UPDATE_INTERVAL);

#endif // FAN_CONTROL_H_INCLUDED
