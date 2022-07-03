#include <iostream>
#include <thread>


#include "ProcessInfo.h"
int main() {

    std::thread th([](){
        while (true){

            int64_t  i = 12235345 * 145345345 * 10;
        }
    });

    ProcessInfo process;

    for (int i = 0; i < 1000; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

       // auto cpu_usage1 = get_cpu_usage(_pid);

        auto cpu_usage = process.GetCpuUsage();

        printf("devenv.exe: %.2f%%\n", cpu_usage);

        std::cout << "cpu -> " << cpu_usage << std::endl;
    }

    return 0;
}
