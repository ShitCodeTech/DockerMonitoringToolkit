// metrics_collector.cpp
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <sstream>
#include <vector>
#include <thread>
#include <chrono>
#include <cmath>
#include <curl/curl.h> 
#include <sys/sysinfo.h>
#include <typeinfo>
#include <iomanip>

class CPUUsage {
public:
    static std::vector<long> getCpuStats() {
        std::ifstream file("/proc/stat");
        std::string line;
        std::vector<long> cpuStats;

        if (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string cpu;
            long value;

            iss >> cpu; // Skip the "cpu" label
            while (iss >> value) {
                cpuStats.push_back(value);
            }
        }
        return cpuStats;
    }

    static double calculateCpuLoad(const std::vector<long>& previousStats, const std::vector<long>& currentStats) {
        long previousIdle = previousStats[3];
        long currentIdle = currentStats[3];

        long previousTotal = 0;
        long currentTotal = 0;

        for (const auto& stat : previousStats) {
            previousTotal += stat;
        }
        for (const auto& stat : currentStats) {
            currentTotal += stat;
        }

        long totalDifference = currentTotal - previousTotal;
        long idleDifference = currentIdle - previousIdle;

        return (1.0 - (static_cast<double>(idleDifference) / totalDifference)) * 100.0;
    }
};


class SystemInfo {
public:
    static std::string getExternalIP() {
        CURL* curl;
        CURLcode res;
        std::string ipAddress;

        curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, "https://ifconfig.me");
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ipAddress);
            res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);
        }

        return ipAddress;
    }

private:
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
        s->append((char*)contents, size * nmemb);
        return size * nmemb;
    }
};

void getCpuUsage() {
    std::vector<long> previousStats = CPUUsage::getCpuStats();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::vector<long> currentStats = CPUUsage::getCpuStats();

    double cpuLoad = CPUUsage::calculateCpuLoad(previousStats, currentStats);
    std::cout << std::round(cpuLoad) << std::endl;
}

void getOtherShit() {
    struct sysinfo info;
    if (sysinfo(&info) != 0) {
        std::cerr << "Error collecting system info" << std::endl;
        return;
    }
    std::cout << info.uptime / 60 << std::endl;
}

void getMemLoad() {
    std::ifstream meminfo("/proc/meminfo");
    if (!meminfo.is_open()) {
        std::cerr << "Unable to open /proc/meminfo" << std::endl;
        return;
    }

    std::string line;
    long totalMemory = 0;
    long freeMemory = 0;

    while (std::getline(meminfo, line)) {
        if (line.find("MemTotal:") == 0) {
            totalMemory = std::stol(line.substr(line.find_first_of("0123456789")));
        } else if (line.find("MemAvailable:") == 0) {
            freeMemory = std::stol(line.substr(line.find_first_of("0123456789")));
            break;
        }
    }

    meminfo.close();

    if (totalMemory == 0) {
        std::cerr << "Could not read total memory from /proc/meminfo" << std::endl;
        return;
    }
    std::cout << std::setprecision(3) << totalMemory/1000000.0  << std::endl;
    float usedMemory = totalMemory - freeMemory;
    std::cout << std::setprecision(3) << usedMemory/1000000.0 << std::endl;
    float memLoad = (usedMemory / totalMemory) * 100;
    std::cout << std::round(memLoad) << std::endl;
}

void getExternalIP() {
    std::string ip = SystemInfo::getExternalIP();
    std::cout << ip << std::endl;
}

void getOsInfo() {                              //need to rewrite cause cant get NODE data
    
    if (const char* pretty_name = std::getenv("PRETTY_NAME")) {
        std::cout << pretty_name << "\n";
    } else {
        std::cerr << "NAMEfucked\n";
    }

    if (const char* id = std::getenv("ID")) {
        std::cout << id << "\n";
    } else {
        std::cerr << "IDfucked\n";
    }
}




int main() {
    getCpuUsage();
    getMemLoad();
    getOtherShit();
    getExternalIP(); 
    getOsInfo();
    return 0;
}
