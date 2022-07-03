//
// Created by root on 02.07.2022.
//


#include "ProcessInfo.h"

#ifdef WIN32

#include "windows.h"
#include <processthreadsapi.h>
#include <psapi.h>
#include <xutility>

struct ProcessInfo::PImpl : public IProcessInfo {

public:
    PImpl() :
    m_pid_process(GetCurrentProcessId()) /*set own process id*/,
    m_process_handle(OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, m_pid_process)) /*open process with handle - for retrieve info*/,
    m_num_processors(getProcessorNumber())/*count processors - need for cpu*/
    {


    }

    ~PImpl() {
        CloseHandle(m_process_handle);
    }

    double GetCpuUsage() override {
        FILETIME now;
        FILETIME creation_time;
        FILETIME exit_time;
        FILETIME kernel_time;
        FILETIME user_time;
        int64_t system_time;
        int64_t time;
        int64_t system_time_delta;
        int64_t time_delta;

        double cpu = -1;

        if (!m_process_handle) return -1;

        GetSystemTimeAsFileTime(&now);

        if (!GetProcessTimes(m_process_handle, &creation_time, &exit_time, &kernel_time, &user_time)) {
            return -1;
        }
        system_time = (fileTime2Utc(&kernel_time) + fileTime2Utc(&user_time)) / m_num_processors;
        time = fileTime2Utc(&now);

        if ((m_last_system_time == 0) || (m_last_time == 0)) {
            m_last_system_time = system_time;
            m_last_time = time;
            return -1;
        }

        system_time_delta = system_time - m_last_system_time;
        time_delta = time - m_last_time;

        cpu = (double) system_time_delta * 100 / (double) time_delta;
        m_last_system_time = system_time;
        m_last_time = time;
        return cpu;
    }


    int64_t GetMemoryUsage() override {
        int64_t memory_usage = 0;

        HANDLE handle;
        PROCESS_MEMORY_COUNTERS_EX pmc;

        if (handle && GetProcessMemoryInfo(handle, (PROCESS_MEMORY_COUNTERS *) &pmc, sizeof(pmc))) {
            memory_usage = std::move(pmc.PrivateUsage);
        } else {
            memory_usage = -1;/*can't retrieve info*/
        }

        CloseHandle(handle);

        return memory_usage;
    }

private:
    static int64_t fileTime2Utc(const FILETIME *ftime) {
        LARGE_INTEGER li;
        li.LowPart = ftime->dwLowDateTime;
        li.HighPart = ftime->dwHighDateTime;
        return li.QuadPart;
    }

   static int getProcessorNumber() {
        SYSTEM_INFO info;
        GetSystemInfo(&info);
        return info.dwNumberOfProcessors;
    }


private:
    DWORD m_pid_process;
    HANDLE m_process_handle;

    int32_t m_num_processors {0};
    int64_t m_last_time {0};
    int64_t m_last_system_time{0};
};

#endif /*WIN32*/


ProcessInfo::ProcessInfo() {
    m_impl = new PImpl();
}

ProcessInfo::~ProcessInfo() {
    delete m_impl;
}

double ProcessInfo::GetCpuUsage() {
    return m_impl->GetCpuUsage();
}

int64_t ProcessInfo::GetMemoryUsage() {
    return m_impl->GetMemoryUsage();
}
