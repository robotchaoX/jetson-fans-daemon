/* Jetson Nano Fan Control Daemon
 *
 * MIT License
 *
 * Copyright (c) 2019 William Hooper,robotchaoX
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "fan-daemon.h"

using namespace std;

/**
 * Exit handler. Set the fan PWM value before leaving
 * If you stop the fan control daemon with  `sudo service fan-daemon stop` the fan is set to the fan PWM cap value.
 */
// 信号处理函数
void exit_handler(int sig) {
    static unsigned pwmCap2; // maximum value that the fan PWM channel can support
    cout << "*** Oh, got a signal : " << sig << endl;
    if (pwmCap2 <= 0)
        pwmCap2 = readIntSysFs(PWM_CAP); // PWM maximum value
    writeIntSysFs("/sys/devices/pwm-fan/target_pwm", pwmCap2);
    cout << "exit_handler --sigaction-- pwmCap2: " << pwmCap2 << endl;
    exit(1);
}

/**
 * Initialize the exit handler
 */
// 信号的发送者可以是 user 也可以是 kernel
void init_exit_handler() {
    struct sigaction sigIntHandler; // 定义一个信号动作
    // Structure describing the action to be taken when a signal arrives.

    sigIntHandler.sa_handler = exit_handler; // .sa_handler信号处理函数
    sigemptyset(&sigIntHandler.sa_mask); // 清空屏蔽的信号  // .sa_mask设置在处理该信号时暂时将sa_mask指定的信号搁置.
                                         // sigaddset(&sigIntHandler.sa_mask, SIGQUIT); // 增加需要屏蔽的信号
                                         // sigIntHandler.sa_flags = SA_NODEFER | SA_RESETHAND;
    sigIntHandler.sa_flags = 0; // 标志位,取值为0，则表示默认行为
    // .sa_flags设置信号处理的其他相关操作, 下列的数值可用：
    //    A_NOCLDSTOP: 如果参数signum 为SIGCHLD, 则当子进程暂停时并不会通知父进程
    //    SA_ONESHOT/SA_RESETHAND: 处理完当前信号后，将信号处理函数设置为SIG_DFL行为
    //    SA_RESTART: 被信号中断的系统调用会自行重启
    //    SA_NOMASK/SA_NODEFER: 不进行当前处理信号到阻塞

    // Get and/or set the action for signal SIG.
    // int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
    //    signum：要操作的信号。
    //    act：要设置的对信号的新处理方式，指向sigaction结构的指针。
    //    oldact：原来对信号的处理方式。
    //    返回值：0 表示成功，-1 表示有错误发生。
    // sigaction()会依参数signum 指定的信号编号来设置该信号的处理函数,sigaction函数有阻塞的功能
    sigaction(SIGINT, &sigIntHandler, NULL);
    sigaction(SIGTERM, &sigIntHandler, NULL);
    // 终端使用kill -l 命令可以显示所有的信号
    // SIGINT	Terminate	Interrupt from keyboard
    // SIGQUIT	Dump	        Quit from keyboard
    // SIGTERM	Terminate	Process termination
    // SIGKILL	Terminate	Forced-process termination
    // SIGCHLD	Ignore	        Child process stopped or terminated, or got signal if traced
    // SIGIO	Terminate	I/O now possible
}

/**
 * Main
 */
int main(int argc, char *argv[]) {

    init_exit_handler(); // Exit handler before leaving

    //    char cmd[128] = {0};
    //    bzero(cmd, sizeof(cmd));
    //    sprintf(cmd, "sudo /usr/bin/jetson_clocks");
    //    if (system(cmd) < 0) {
    //        printf("sudo /usr/bin/jetson_clocks failed \n");
    //        int error = -1;
    //    }
    // max performance
    //    system(JETSON_CLOCKS); // bash /usr/bin/jetson_clocks

    while (true) {
        thermalControl();
    }

    return 0;
}

/**
 * Read an integer value from a sysfs path
 */
int readIntSysFs(string path) {
    int value = 0;
    //    ifstream infs(path,ios::in); // 构造函数方式打开文件
    ifstream infs;
    infs.open(path); // 默认打开方式ios::in，可省
    if (!infs.fail()) {
        infs >> value;
    } else {
        cout << "readIntSysFs failed: " << path << endl;
        value = -1;
    }
    infs.close();
    return value;
}

/**
 * Write an unsigned integer value to a sysfs path
 */
int writeIntSysFs(string path, unsigned value) {
    int error = 0;
    //    ofstream outfs(path, ios::out | ios::trunc); // 构造函数方式打开文件
    ofstream outfs;
    outfs.open(path, ios::out | ios::trunc); // 默认打开方式ios::out|ios::trunc,可省
    if (!outfs.fail()) {
        outfs << value;
        error = 0;
    } else {
        cout << "writeIntSysFs failed: " << path << endl;
        error = -1;
    }
    outfs.close();
    return error;
}

/**
 * Read fan speed
 */
int readFanSpeed() {
    int fanSpeed = 0;
    fanSpeed = readIntSysFs(TARGET_PWM);
    if (fanSpeed < 0) {
        cout << "readFanSpeed error : " << fanSpeed << endl;
    }
    return fanSpeed;
}

/**
 * set fan speed to an unsigned integer value
 */
int setFanSpeed(unsigned speedValue) { writeIntSysFs(TARGET_PWM, speedValue); }

/**
 * Get the PWM cap. This is maximum value that the fan PWM channel can support
 */
int getPwmCap() {
    int pwmCap = 0;
    pwmCap = readIntSysFs(PWM_CAP);
    if (pwmCap <= 0) {
        cout << "getPwmCap error : " << pwmCap << endl;
    }
    return pwmCap;
}

/**
 * Read the average temperature. The function reads all thermal zones and returns the average.
 *
 */
float readAverageTemp() {
    float averageTemp = 0;
    int thermal_zone;
    glob_t globResult;

    // glob() // find pathnames matching a pattern,Linux文件系统中路径名称的模式匹配，
    // 查找文件系统中指定模式的路径名
    //    typedef struct
    //    {
    //        size_t gl_pathc;    /* Count of paths matched by pattern.  */
    //        char **gl_pathv;    /* List of matched pathnames.  */
    //        size_t gl_offs;     /* Slots to reserve in ‘gl_pathv’.  */
    //    } glob_t;

    // cat /sys/devices/virtual/thermal/thermal_zone*/temp  查看所有8个类型温度
    glob(THERMAL_ZONE_GLOB, GLOB_TILDE, NULL, &globResult);
    // 遍历thermal zones所有类型的温度的平均值 0-7
    for (unsigned i = 0; i < globResult.gl_pathc; ++i) {
        thermal_zone = readIntSysFs(globResult.gl_pathv[i]);
        if (i == 6)
            thermal_zone = 0; // 剔除第七个明显偏高温度
        averageTemp += thermal_zone;
        //        averageTemp += readIntSysFs(globResult.gl_pathv[i]);
    }
    averageTemp = (averageTemp / (globResult.gl_pathc - 1)) / 1000;
    //    globfree(&globResult); // cleanup
    return averageTemp;
}

/**
 * Return the adjusted PWM fan speed. Calculated from the provided temperature and the
 * fan PWM cap value.
 */
// unsigned adjustFanSpeed(int temp) {
unsigned adjustFanSpeed(float tempe, int fanOffTempe, int fanMaxTempe) {
    static unsigned pwmCap; // maximum value that the fan PWM channel can support
    unsigned fanPWMSpeed = 0;
    if (pwmCap <= 0) {
        pwmCap = readIntSysFs(PWM_CAP); // PWM maximum value 255
        //    pwmCap = getPwmCap(); // PWM maximum value 255
        cout << "pwmCap:" << pwmCap << endl;
    }
    //    fanPWMSpeed = pwmCap * (tempe - fanOffTempe) / (fanMaxTempe - fanOffTempe); // #define
    // upper real FAN_OFF_TEMP = (FAN_MAX_TEMP + 4 * FAN_OFF_TEMP) / 5
    fanPWMSpeed = pwmCap * ((tempe - fanOffTempe) / (fanMaxTempe - fanOffTempe) * 0.6 + 0.4); // #define
    //    cout << fanPWMSpeed << endl;
    if (fanPWMSpeed <= pwmCap * 0.4) // 太慢就不需要转了
        fanPWMSpeed = 0;
    else if (fanPWMSpeed >= pwmCap)
        fanPWMSpeed = pwmCap;

    return (unsigned)fanPWMSpeed;
    //    return min((unsigned)max((unsigned)0, fanPWMSpeed), pwmCap);
}

/**
 * Thermal control by fan. The function control fan to cool down every updateInterval_s seconds.
 *
 */
// int thermalControl() {
int thermalControl(int updateInterval_s) {
    float temperature = 0;    // temperature
    unsigned fanPWMValue = 0; // 0-255

    temperature = readAverageTemp();
    cout << "temperature: " << temperature << endl;
    fanPWMValue = adjustFanSpeed(temperature);
    setFanSpeed(fanPWMValue);
    cout << "fanPWMValue: " << readFanSpeed() << endl;
    //    cout << "fanPWMValue: " << fanPWMValue << endl;
    // 阻塞当前线程rel_time的时间
    this_thread::sleep_for(chrono::milliseconds(updateInterval_s * 1000)); // milliseconds 毫秒
}