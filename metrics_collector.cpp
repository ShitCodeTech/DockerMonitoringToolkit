// metrics_collector.cpp
#include <iostream>
#include <sys/sysinfo.h>
#include <fstream>
#include <string>
#include <stdio.h>

void getMetrics() {
    struct sysinfo info;
    if (sysinfo(&info) != 0) {
        std::cerr << "Error collecting system info" << std::endl;
        return;
    } 

    // Загрузка CPU (усреднённая за последние 1, 5, 15 минут)
    std::cout << "CPU Load (1 min): " << (100 * info.loads[0] / 65536) << std::endl;
    std::cout << "CPU Load (5 min): " << (100 * info.loads[1] / 65536) << std::endl;
    std::cout << "CPU Load (15 min): " << (100 * info.loads[2] / 65536) << std::endl;

    // Свободная и общая память
    std::cout << "Total RAM: " << info.totalram / 1024 / 1024 << " MB" << std::endl;
    std::cout << "Free RAM: " << info.freeram / 1024 / 1024 << " MB" << std::endl;

    // Время работы системы
    std::cout << "Uptime: " << info.uptime / 60 << " minutes" << std::endl;
}


void getSysInfo() {
    std::ifstream file("/etc/os-release");
    if (!file.is_open()) {
        std::cerr << "Unable to open /etc/os-release file.\n";
        return;
    }

    std::string line;
    std::string id, pretty_name;

    while (std::getline(file, line)) {
        if (line.rfind("ID=", 0) == 0) {
            id = line.substr(3); // Remove "ID=" prefix
        } else if (line.rfind("PRETTY_NAME=", 0) == 0) {
            pretty_name = line.substr(12); // Remove "PRETTY_NAME=" prefix
        }
    }

    file.close();

    if (!pretty_name.empty()) {
        std::cout << "PRETTY_NAME: " << pretty_name << "\n";
    } else {
        std::cerr << "PRETTY_NAME not found.\n";
    }

    if (!id.empty()) {
        std::cout << "ID: " << id << "\n";
    } else {
        std::cerr << "ID not found.\n";
    }
}


int main() {
    getMetrics();
    getSysInfo();
    return 0;
}
