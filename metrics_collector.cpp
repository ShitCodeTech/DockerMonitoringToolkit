// metrics_collector.cpp
#include <iostream>
#include <sys/sysinfo.h>

void getMetrics() {
    struct sysinfo info;
    if (sysinfo(&info) != 0) {
        std::cerr << "Error collecting system info" << std::endl;
        return;
    }

    // Загрузка CPU (усреднённая за последние 1, 5, 15 минут)
    std::cout << "CPU Load (1 min): " << info.loads[0] / 65536.0 << std::endl;
    std::cout << "CPU Load (5 min): " << info.loads[1] / 65536.0 << std::endl;
    std::cout << "CPU Load (15 min): " << info.loads[2] / 65536.0 << std::endl;

    // Свободная и общая память
    std::cout << "Total RAM: " << info.totalram / 1024 / 1024 << " MB" << std::endl;
    std::cout << "Free RAM: " << info.freeram / 1024 / 1024 << " MB" << std::endl;

    // Время работы системы
    std::cout << "Uptime: " << info.uptime / 60 << " minutes" << std::endl;
}

int main() {
    getMetrics();
    return 0;
}
