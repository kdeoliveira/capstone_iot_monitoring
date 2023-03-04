#pragma once
#include <functional>
#include "common.hpp"
#include "arg_parser.hpp"

SERVICE_STATUS_HANDLE schStatusHandle;
SERVICE_STATUS schServiceStatus;
HANDLE stopEvent;

static DWORD checkPoint = 1;

VOID ReportServiveStatus(DWORD currentState, DWORD dwWin32ExitCode, DWORD waitHint) {

	schServiceStatus.dwCurrentState = currentState;
	schServiceStatus.dwWin32ExitCode = dwWin32ExitCode;
	schServiceStatus.dwWaitHint = waitHint;

	if (currentState == SERVICE_START_PENDING) {
		schServiceStatus.dwControlsAccepted = 0;
	}
	else {
		schServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_PAUSE_CONTINUE;
	}

	if (currentState == SERVICE_RUNNING || currentState == SERVICE_STOPPED) {
		schServiceStatus.dwCheckPoint = 0;
	}
	else {
		schServiceStatus.dwCheckPoint = checkPoint++;
	}

	SetServiceStatus(
		schStatusHandle,
		&schServiceStatus
	);
}

VOID LogEvent(char* szFunction)
{

}

void WINAPI ServiceControlHandler(DWORD dwCtrl) {
	switch (dwCtrl) {
	case SERVICE_CONTROL_STOP:
		ReportServiveStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);
		//Force poll workers to finish
		handler(0);
		Sleep(2000); //shouldn't be hardcoded; main thread should await completion of uart/db comms
		SetEvent(stopEvent);
		ReportServiveStatus(schServiceStatus.dwCurrentState, NO_ERROR, 0);
		return;
	case SERVICE_CONTROL_INTERROGATE:
		break;
	default:
		break;
	}
	
}

DWORD worker_thread(LPVOID notUsed) {
	return iot_monitoring::main(h);
}

void main_service(int argc, char** argv) {

	//Sleep(10000);
	schStatusHandle = RegisterServiceCtrlHandlerA(
		"iot-monitoring",
		ServiceControlHandler
	);

	if (!schStatusHandle) {
		LogEvent("RegisterServiceControlHandler");
		return;
	}

	schServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	schServiceStatus.dwServiceSpecificExitCode = EXIT_SUCCESS;

	ReportServiveStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

	stopEvent = CreateEventA(
		NULL,
		TRUE,
		FALSE,
		NULL
	);

	if (!stopEvent) {
		LogEvent("CreateEvent");
		return;
	}

	ReportServiveStatus(SERVICE_RUNNING, NO_ERROR, 0);

	HANDLE workerThread = ::CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)worker_thread,
		0,
		0,
		0
	);
	
		
	WaitForSingleObject(stopEvent, INFINITE); //TODO: RegisterCallback for cleanup

	ReportServiveStatus(SERVICE_STOPPED, NO_ERROR, 0);
	
}

