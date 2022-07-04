#include <iostream>
#include <thread>


#include "ProcessInfo.h"
#include "SystemInformation.h"

int main() {

    std::thread th([](){
        while (true){

            auto i = 1223 * 1453;
        }
    });

    ProcessInfo process;
    SystemInformation sys_info;

    auto memory = process.GetMemoryUsage();
    auto max_memory = sys_info.GetTotalMemory();

    auto mem_usage = sys_info.GetTotalUsageMemory();

   

    for (int i = 0; i < 1000; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

       // auto cpu_usage1 = get_cpu_usage(_pid);

        auto cpu_usage = process.GetCpuUsage();

        auto cpu_sys_usage = sys_info.GetCpuTotalUsage();

        printf("devenv.exe: %.2f%%\n", cpu_usage);

        std::cout << "cpu -> " << cpu_sys_usage << std::endl;
    }

    return 0;
}
