#include <stdio.h>

#include <thread>

#include "../headers/systemPerformanceCollector.h"
#pragma comment(lib, "../release/systemPerformanceCollector")


unsigned __stdcall func(void*) {

	int* arr[100];

	for (;;) {

		for (int i = 0; i < 100; i++) {
			arr[i] = new int;
		}

		for (int i = 0; i < 100; i++) {
			delete arr[i];
		}

	}

}


int main() {

	CSystemPerformanceCollector performance;

	_beginthreadex(nullptr, 0, func, nullptr, 0, nullptr);
	_beginthreadex(nullptr, 0, func, nullptr, 0, nullptr);
	_beginthreadex(nullptr, 0, func, nullptr, 0, nullptr);

	DWORD processorNum = performance.getProcessorNum();
	DWORD networkInterfaceNum = performance.getNetworkInterfaceNum();

	for (;;) {

		performance.update();

		printf("CPU usage (total) : %.2lf\n", performance._processorCpuUsageTotal);

		for (int processorCnt = 0; processorCnt < processorNum; ++processorCnt) {
			printf("CPU usage (%5d) : %.2lf\n", processorCnt, performance._processorCpuUsage[processorCnt]);
		}
		printf("\n");

		printf("Available Memory  : %.2lf\n", performance._availableMemory);
		printf("Non Paged Pool    : %.2lf\n", performance._nonPagedPool);
		printf("\n");

		printf("Network Interface\n");
		printf(" recv\n");

		for (int networkInterfaceCnt = 0; networkInterfaceCnt < networkInterfaceNum; ++networkInterfaceCnt) {
			CSystemPerformanceCollector::stNetworkInterface* networkInterface = &performance._networkInterface[networkInterfaceCnt];
			wprintf(L"  %s: %.2lf\n", networkInterface->_name, networkInterface->_recvBytes);
		}

		printf(" send\n");

		for (int networkInterfaceCnt = 0; networkInterfaceCnt < networkInterfaceNum; ++networkInterfaceCnt) {
			CSystemPerformanceCollector::stNetworkInterface* networkInterface = &performance._networkInterface[networkInterfaceCnt];
			wprintf(L"  %s: %.2lf\n", networkInterface->_name, networkInterface->_sendBytes);
		}


		Sleep(1000);
		system("cls");

	}

	return 0;

}