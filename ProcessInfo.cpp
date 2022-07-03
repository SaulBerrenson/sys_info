//
// Created by root on 02.07.2022.
//

#include "ProcessInfo.h"

#ifdef WIN32

#include "windows.h"
#include <processthreadsapi.h>
#include <psapi.h>
#include <xutility>

struct ProcessInfo::PImpl : public IProcessInfo
{

public:
    PImpl() : m_pid_process(GetCurrentProcessId()) /*set own process id*/,
              m_process_handle(OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, m_pid_process)) /*open process with handle - for retrieve info*/,
              m_num_processors(getProcessorNumber()) /*count processors - need for cpu*/
    {
    }

    ~PImpl() override
    {
        CloseHandle(m_process_handle);
    }

    double GetCpuUsage() override
    {
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

        if (!m_process_handle)
            return -1;

        GetSystemTimeAsFileTime(&now);

        if (!GetProcessTimes(m_process_handle, &creation_time, &exit_time, &kernel_time, &user_time))
        {
            return -1;
        }
        system_time = (fileTime2Utc(&kernel_time) + fileTime2Utc(&user_time)) / m_num_processors;
        time = fileTime2Utc(&now);

        if ((m_last_system_time == 0) || (m_last_time == 0))
        {
            m_last_system_time = system_time;
            m_last_time = time;
            return -1;
        }

        system_time_delta = system_time - m_last_system_time;
        time_delta = time - m_last_time;

        cpu = (double)system_time_delta * 100 / (double)time_delta;
        m_last_system_time = system_time;
        m_last_time = time;
        return cpu;
    }

    int64_t GetMemoryUsage() override
    {
        int64_t memory_usage = 0;

        HANDLE handle;
        PROCESS_MEMORY_COUNTERS_EX pmc;

        if (handle && GetProcessMemoryInfo(handle, (PROCESS_MEMORY_COUNTERS *)&pmc, sizeof(pmc)))
        {
            memory_usage = std::move(pmc.PrivateUsage);
        }
        else
        {
            memory_usage = -1; /*can't retrieve info*/
        }

        CloseHandle(handle);

        return memory_usage;
    }

private:
    static int64_t fileTime2Utc(const FILETIME *ftime)
    {
        LARGE_INTEGER li;
        li.LowPart = ftime->dwLowDateTime;
        li.HighPart = ftime->dwHighDateTime;
        return li.QuadPart;
    }

    static int getProcessorNumber()
    {
        SYSTEM_INFO info;
        GetSystemInfo(&info);
        return info.dwNumberOfProcessors;
    }

private:
    DWORD m_pid_process;
    HANDLE m_process_handle;

    int32_t m_num_processors{0};
    int64_t m_last_time{0};
    int64_t m_last_system_time{0};
};
/*WIN32*/
#elif __linux__

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "sys/times.h"
#include "sys/vtimes.h"

struct ProcessInfo::PImpl : public IProcessInfo
{

public:
    PImpl()
    {
        init();
    }

    ~PImpl() override
    {
    }

    double GetCpuUsage() override
    {
        struct tms timeSample;
        clock_t now;
        double percent;

        now = times(&timeSample);
        if (now <= m_lastCPU || timeSample.tms_stime < m_lastSysCPU ||
            timeSample.tms_utime < m_lastUserCPU)
        {
            // Overflow detection. Just skip this value.
            percent = -1.0;
        }
        else
        {
            percent = (timeSample.tms_stime - m_lastSysCPU) +
                      (timeSample.tms_utime - m_lastUserCPU);
            percent /= (now - m_lastCPU);
            percent /= m_numProcessors;
            percent *= 100;
        }
        m_lastCPU = now;
        m_lastSysCPU = timeSample.tms_stime;
        m_lastUserCPU = timeSample.tms_utime;

        return percent;
    }

    int64_t GetMemoryUsage() override
    {
        FILE *file = fopen("/proc/self/status", "r");
        int result = -1;
        char line[128];

        while (fgets(line, 128, file) != NULL)
        {
            if (strncmp(line, "VmRSS:", 6) == 0)
            {
                result = parseLine(line);
                break;
            }
        }
        fclose(file);
        return result * 1024;
    }

private:
    int parseLine(char *line)
    {
        // This assumes that a digit will be found and the line ends in " Kb".
        int i = strlen(line);
        const char *p = line;
        while (*p < '0' || *p > '9')
            p++;
        line[i - 3] = '\0';
        i = atoi(p);
        return i;
    }

    void init()
    {
        FILE *file;
        struct tms timeSample;
        char line[128];

        m_lastCPU = times(&timeSample);
        m_lastSysCPU = timeSample.tms_stime;
        m_lastUserCPU = timeSample.tms_utime;

        file = fopen("/proc/cpuinfo", "r");
        m_numProcessors = 0;
        while (fgets(line, 128, file) != NULL)
        {
            if (strncmp(line, "processor", 9) == 0)
                m_numProcessors++;
        }
        fclose(file);
    }

private:
    clock_t m_lastCPU{0};
    clock_t m_lastSysCPU{0};
    clock_t m_lastUserCPU{0};
    int m_numProcessors{0};
};

#else /*not support*/
#error "process info doesn't support this platform"
#endif

ProcessInfo::ProcessInfo()
{
    m_impl = new PImpl();
}

ProcessInfo::~ProcessInfo()
{
    delete m_impl;
}

double ProcessInfo::GetCpuUsage()
{
    return m_impl->GetCpuUsage();
}

int64_t ProcessInfo::GetMemoryUsage()
{
    return m_impl->GetMemoryUsage();
}
