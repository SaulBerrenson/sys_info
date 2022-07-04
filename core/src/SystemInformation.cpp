//
// Created by root on 03.07.2022.
//

#include "SystemInformation.h"

/*main info got from msdn and https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process */

#if  defined(WIN32) | defined(_WIN64)

#pragma comment(lib, "pdh.lib")
#include <Windows.h>
#include <Pdh.h>

struct SystemInformation::PImpl : public ISystemInformation
{
public:
    PImpl() { init(); }

    ~PImpl() override = default;

    int64_t GetTotalMemory() override
    {
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(MEMORYSTATUSEX);
        GlobalMemoryStatusEx(&memInfo);
        DWORDLONG totalPhysMem = memInfo.ullTotalPhys;

        return totalPhysMem;
    }

    int64_t GetTotalUsageMemory() override
    {
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(MEMORYSTATUSEX);
        GlobalMemoryStatusEx(&memInfo);
        DWORDLONG physMemUsed = memInfo.ullTotalPhys - memInfo.ullAvailPhys;

        return physMemUsed;
    }

    double GetCpuTotalUsage() override
    {
        PDH_FMT_COUNTERVALUE counterVal;

        PdhCollectQueryData(cpuQuery);
        PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
        return counterVal.doubleValue;
    }

private:
    void init()
    {
        PdhOpenQuery(NULL, NULL, &cpuQuery);
        // You can also use L"\\Processor(*)\\% Processor Time" and get individual CPU values with PdhGetFormattedCounterArray()
        PdhAddEnglishCounter(cpuQuery, LPSTR("\\Processor(_Total)\\% Processor Time"), NULL, &cpuTotal);
        PdhCollectQueryData(cpuQuery);	    
    }

private:
    PDH_HQUERY cpuQuery;
    PDH_HCOUNTER cpuTotal;

};
/*WIN32*/
#elif __linux__
/*memory*/
#include "sys/times.h"
#include "sys/vtimes.h"
#include "sys/types.h"
#include "sys/sysinfo.h"

/*cpu*/
#include <cstdlib>
#include <cstdio>
#include <cstring>

struct SystemInformation::PImpl : public ISystemInformation
{
public:
    PImpl() = default;

    ~PImpl() override = default;

    int64_t GetTotalMemory() override
    {
        struct sysinfo memInfo;

        sysinfo(&memInfo);
        long long totalPhysMem = memInfo.totalram;
        //Multiply in next statement to avoid int overflow on right hand side...
        totalPhysMem *= memInfo.mem_unit;

        return totalPhysMem;
    }

    int64_t GetTotalUsageMemory() override
    {
        struct sysinfo memInfo;

        sysinfo(&memInfo);
        long long physMemUsed = memInfo.totalram - memInfo.freeram;
        //Multiply in next statement to avoid int overflow on right hand side...
        physMemUsed *= memInfo.mem_unit;

        return physMemUsed;
    }

    double GetCpuTotalUsage() override
    {
        double percent;
        FILE* file;
        unsigned long long totalUser, totalUserLow, totalSys, totalIdle, total;

        file = fopen("/proc/stat", "r");
        fscanf(file, "cpu %llu %llu %llu %llu", &totalUser, &totalUserLow,
            &totalSys, &totalIdle);
        fclose(file);

        if (totalUser < m_lastTotalUser || totalUserLow < m_lastTotalUserLow ||
            totalSys < m_lastTotalSys || totalIdle < m_lastTotalIdle) {
            //Overflow detection. Just skip this value.
            percent = -1.0;
        }
        else {
            total = (totalUser - m_lastTotalUser) + (totalUserLow - m_lastTotalUserLow) +
                (totalSys - m_lastTotalSys);
            percent = total;
            total += (totalIdle - m_lastTotalIdle);
            percent /= total;
            percent *= 100;
        }

        m_lastTotalUser = totalUser;
        m_lastTotalUserLow = totalUserLow;
        m_lastTotalSys = totalSys;
        m_lastTotalIdle = totalIdle;

        return percent;
    }

private:
    void init() {
        FILE* file = fopen("/proc/stat", "r");
        fscanf(file, "cpu %llu %llu %llu %llu", &m_lastTotalUser, &m_lastTotalUserLow,
            &m_lastTotalSys, &m_lastTotalIdle);
        fclose(file);
    }

private:
    unsigned long long m_lastTotalUser;
    unsigned long long m_lastTotalUserLow;
    unsigned long long m_lastTotalSys;
    unsigned long long m_lastTotalIdle;
};

#else /*not support*/
#error "process info doesn't support this platform"
#endif

SystemInformation::SystemInformation() {
    m_impl = new PImpl();
}

SystemInformation::~SystemInformation() noexcept {
    delete m_impl;
}

int64_t SystemInformation::GetTotalMemory()
{
    return m_impl->GetTotalMemory();
}

int64_t SystemInformation::GetTotalUsageMemory()
{
    return m_impl->GetTotalUsageMemory();
}

double SystemInformation::GetCpuTotalUsage()
{
    return m_impl->GetCpuTotalUsage();
}
