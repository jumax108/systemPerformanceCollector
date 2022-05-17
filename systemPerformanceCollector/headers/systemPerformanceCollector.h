#pragma once

#include <Windows.h>

#include <Pdh.h>

#pragma comment(lib, "pdh")

class CSystemPerformanceCollector {

public:

	struct stNetworkInterface {
		friend CSystemPerformanceCollector;
	public:
		wchar_t* _name;
		double _sendBytes;
		double _recvBytes;
	private:
		PDH_HCOUNTER _recvCounter;
		PDH_HCOUNTER _sendCounter;
	};

public:

	CSystemPerformanceCollector();
	~CSystemPerformanceCollector();

	void update();

	double* _processorCpuUsage;
	double _processorCpuUsageTotal;

	double _availableMemory;
	double _nonPagedPool;

	stNetworkInterface* _networkInterface;

	inline DWORD getProcessorNum() {
		return _processorNum;
	}
	inline DWORD getNetworkInterfaceNum() {
		return _networkInterfaceNum;
	}

private:

	DWORD _processorNum;
	DWORD _networkInterfaceNum;

	PDH_HQUERY _pdhQuery;

	PDH_HCOUNTER* _processorCpuUsageCounter;
	PDH_HCOUNTER _processorCpuUsageTotalCounter;
	PDH_HCOUNTER _availableMemoryCounter;
	PDH_HCOUNTER _nonPagedPoolCounter;


};