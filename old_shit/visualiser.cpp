#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <thread>
#include <chrono>

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

    static void visualizeCpuLoad() {
        while (true) {
            // Get initial CPU stats
            std::vector<long> previousStats = getCpuStats();

            // Wait for 1 second
            std::this_thread::sleep_for(std::chrono::seconds(1));

            // Get CPU stats again
            std::vector<long> currentStats = getCpuStats();

            // Calculate CPU load
            double cpuLoad = calculateCpuLoad(previousStats, currentStats);

            // Visualize CPU load
            visualize(cpuLoad);

            // Clear the console output (optional, for better visualization)
            std::cout << "\033[2J\033[1;1H"; // ANSI escape code to clear the console
        }
    }

private:
    static double calculateCpuLoad(const std::vector<long>& previousStats, const std::vector<long>& currentStats) {
        long previousIdle = previousStats[3]; // Idle time is typically the 4th value
        long currentIdle = currentStats[3];

        long previousTotal = 0;
        long currentTotal = 0;

        // Calculate total CPU time for previous and current stats
        for (const auto& stat : previousStats) {
            previousTotal += stat;
        }
        for (const auto& stat : currentStats) {
            currentTotal += stat;
        }

        long totalDifference = currentTotal - previousTotal;
        long idleDifference = currentIdle - previousIdle;

        return (1.0 - (static_cast<double>(idleDifference) / totalDifference)) * 100.0; // CPU load percentage
    }

    static void visualize(double cpuLoad) {
        int barWidth = 50; // Width of the progress bar
        int filledWidth = static_cast<int>((cpuLoad / 100.0) * barWidth);

        std::cout << "CPU Load: " << cpuLoad << "% [";
        for (int i = 0; i < filledWidth; ++i) {
            std::cout << '#'; // Filled part
        }
        for (int i = filledWidth; i < barWidth; ++i) {
            std::cout << '-'; // Unfilled part
        }
        std::cout << "]" << std::endl;
    }
};

int main() {
    CPUUsage::visualizeCpuLoad();
    return 0;
}
