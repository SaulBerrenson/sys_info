//
// Created by root on 02.07.2022.
//
#pragma once
#ifndef CPU_INFO_PROCESSINFO_H
#define CPU_INFO_PROCESSINFO_H

#include <cstdint>

class IProcessInfo{
public:
    IProcessInfo() = default;
    virtual ~IProcessInfo() = default;

    /*!
     * Get Current % Cpu Usage
     * @return percent
     */
   virtual double GetCpuUsage() = 0;
   /*!
    * @brief Get Current Memory Usage
    * @return bytes
    */
   virtual int64_t GetMemoryUsage() = 0;
};


class ProcessInfo : public IProcessInfo{
public:
    ProcessInfo();
    ~ProcessInfo() override;

    /*!
     * @copydoc IProcessInfo
     * @return percent;
     */
    double GetCpuUsage() override;
    /*!
     * @copydoc IProcessInfo
     * @return bytes;
     */
    int64_t GetMemoryUsage() override;

private:
    struct PImpl;

    PImpl* m_impl;
};


#endif //CPU_INFO_PROCESSINFO_H
