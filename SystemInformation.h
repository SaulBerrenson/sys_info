//
// Created by root on 03.07.2022.
//

#ifndef CPU_INFO_SYSTEMINFORMATION_H
#define CPU_INFO_SYSTEMINFORMATION_H
#include <cstdint>

class ISystemInformation{
public:
    ISystemInformation() = default;
    virtual ~ISystemInformation() = default;

    /**
	* @brief Get Total Available Memory Bytes
	*
	* \return
	*/
    virtual int64_t GetTotalMemory() = 0;


    /**
    * @brief Get Total Usage Memory Bytes at System
    *
    * \return
    */
    virtual int64_t GetTotalUsageMemory() = 0;


    virtual double GetCpuTotalUsage() = 0;
};


class SystemInformation: public ISystemInformation{
    
public:
    SystemInformation();
    virtual ~SystemInformation() override;

    int64_t GetTotalMemory() override;
    int64_t GetTotalUsageMemory() override;
    double GetCpuTotalUsage() override;

private:
    struct PImpl;

    PImpl* m_impl;
};


#endif //CPU_INFO_SYSTEMINFORMATION_H
