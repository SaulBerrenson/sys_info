#include <iostream>
#include <thread>


#include "ProcessInfo.h"

int main() {

    std::thread th([](){
        while (true){

            auto i = 1223 * 1453;
        }
    });

    ProcessInfo process;

    auto memory = process.GetMemoryUsage();

    for (int i = 0; i < 1000; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

       // auto cpu_usage1 = get_cpu_usage(_pid);

        auto cpu_usage = process.GetCpuUsage();
         auto memory = process.GetMemoryUsage();

        printf("devenv.exe: %.2f%%\n", cpu_usage);
        printf("devenv.exe: %.2f%%\n", memory);

        std::cout << "cpu -> " << cpu_usage << std::endl;
    }

    return 0;
}
