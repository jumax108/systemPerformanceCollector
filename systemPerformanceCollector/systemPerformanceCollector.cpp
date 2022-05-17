#include "headers/systemPerformanceCollector.h"

#include <stdio.h>

CSystemPerformanceCollector::CSystemPerformanceCollector() {

	/* init var */ {

		ZeroMemory(_processorCpuUsage, sizeof(double) * _processorNum);
		_processorCpuUsageTotal = 0;
		_availableMemory = 0;
		_nonPagedPool = 0;
		_networkInterfaceNum = 0;

	}

	/* get processor num */ {

		SYSTEM_INFO systemInfo;
		GetSystemInfo(&systemInfo);
		_processorNum = systemInfo.dwNumberOfProcessors;

	}

	/* get network interface */ {

		wchar_t* counters = nullptr;
		wchar_t* interfaces = nullptr;

		DWORD counterSize, interfaceSize;

		PdhEnumObjectItems(NULL, NULL, L"Network Interface", counters, &counterSize, interfaces, &interfaceSize, PERF_DETAIL_WIZARD, 0);
		counters = new wchar_t[counterSize];
		interfaces = new wchar_t[interfaceSize];
		PdhEnumObjectItems(NULL, NULL, L"Network Interface", counters, &counterSize, interfaces, &interfaceSize, PERF_DETAIL_WIZARD, 0);

		wchar_t* context = new wchar_t[interfaceSize];

		int networkInterfaceNum = 1;
		wcstok_s(interfaces, L"\0", &context);
		while (wcstok_s(NULL, L"\0", &context) != nullptr) {
			networkInterfaceNum += 1;
		}

		_networkInterface = new stNetworkInterface[networkInterfaceNum];
		_networkInterfaceNum = networkInterfaceNum;

		_networkInterface[0]._name = wcstok_s(interfaces, L"\0", &context);
		_networkInterface[0]._recvBytes = 0;
		_networkInterface[0]._sendBytes = 0;

		for (int networkInterfaceCnt = 1; networkInterfaceCnt < networkInterfaceNum; ++networkInterfaceCnt) {
			
			stNetworkInterface* networkInterface = &_networkInterface[networkInterfaceCnt];

			networkInterface->_name = wcstok_s(nullptr, L"\0", &context);
			networkInterface->_recvBytes = 0;
			networkInterface->_sendBytes = 0;

		}

	}

	/* alloc processor arr */{

		_processorCpuUsage = new double[_processorNum];
		_processorCpuUsageCounter = new PDH_HCOUNTER[_processorNum];

	}

	/* open pdh query */ {

		PdhOpenQuery(NULL, NULL, &_pdhQuery);

	}

	/* add counter */ {

		PdhAddCounter(_pdhQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &_processorCpuUsageTotalCounter);
		PdhAddCounter(_pdhQuery, L"\\Memory\\Available MBytes", NULL, &_availableMemoryCounter);
		PdhAddCounter(_pdhQuery, L"\\Memory\\Pool Nonpaged Bytes", NULL, &_nonPagedPoolCounter);

		for (int processorCnt = 0; processorCnt < _processorNum; ++processorCnt) {

			wchar_t query[100];
			wsprintf(query, L"\\Processor(%d)\\%% Processor Time", processorCnt);
			PdhAddCounter(_pdhQuery, query, NULL, &_processorCpuUsageCounter[processorCnt]);

		}

		// network interface
		for (int networkInterfaceCnt = 0; networkInterfaceCnt < _networkInterfaceNum; ++networkInterfaceCnt) {
			
			stNetworkInterface* networkInterface = &_networkInterface[networkInterfaceCnt];
			
			wchar_t recvQuery[100];
			wsprintf(recvQuery, L"\\Network Interface(%s)\\Bytes Received/sec", networkInterface->_name);
			PdhAddCounter(_pdhQuery, recvQuery, NULL, &networkInterface->_recvCounter);

			wchar_t sendQuery[100];
			wsprintf(sendQuery, L"\\Network Interface(%s)\\Bytes Sent/sec", networkInterface->_name);
			PdhAddCounter(_pdhQuery, sendQuery, NULL, &networkInterface->_sendCounter);

		}

	}


}

CSystemPerformanceCollector::~CSystemPerformanceCollector() {

	delete[] _processorCpuUsage;
	delete[] _processorCpuUsageCounter;

}

void CSystemPerformanceCollector::update() {

	/* data update */ {
		PdhCollectQueryData(_pdhQuery);
	}

	/* get cpu total data */ {

		PDH_FMT_COUNTERVALUE counterValue;
		PdhGetFormattedCounterValue(_processorCpuUsageTotalCounter, PDH_FMT_DOUBLE, NULL, &counterValue);
		_processorCpuUsageTotal = counterValue.doubleValue;

	}

	/* get each processor data */ {

		PDH_FMT_COUNTERVALUE counterValue;
		for (int processorCnt = 0; processorCnt < _processorNum; ++processorCnt) {
			PdhGetFormattedCounterValue(_processorCpuUsageCounter[processorCnt], PDH_FMT_DOUBLE, NULL, &counterValue);
			_processorCpuUsage[processorCnt] = counterValue.doubleValue;
		}

	}

	/* get available memory */ {

		PDH_FMT_COUNTERVALUE counterValue;
		PdhGetFormattedCounterValue(_availableMemoryCounter, PDH_FMT_DOUBLE, NULL, &counterValue);
		_availableMemory = counterValue.doubleValue;
	}

	/* get non paged pool */ {

		PDH_FMT_COUNTERVALUE counterValue;
		PdhGetFormattedCounterValue(_nonPagedPoolCounter, PDH_FMT_DOUBLE, NULL, &counterValue);
		_nonPagedPool = counterValue.doubleValue;

	}

	/* get network send / recv bytes */ {

		stNetworkInterface* iter = _networkInterface;
		stNetworkInterface* end = iter + _networkInterfaceNum;

		for (; iter != end; ++iter) {

			/* get recv bytes */ {
				PDH_FMT_COUNTERVALUE counterValue;
				PdhGetFormattedCounterValue(iter->_recvCounter, PDH_FMT_DOUBLE, NULL, &counterValue);
				iter->_recvBytes = counterValue.doubleValue;
			}

			/* get send bytes */ {
				PDH_FMT_COUNTERVALUE counterValue;
				PdhGetFormattedCounterValue(iter->_sendCounter, PDH_FMT_DOUBLE, NULL, &counterValue);
				iter->_sendBytes = counterValue.doubleValue;
			}
		}


	}

}