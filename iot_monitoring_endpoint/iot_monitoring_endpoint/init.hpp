#pragma once
#include <functional>
#include "common.hpp"
#include "arg_parser.hpp"

SERVICE_STATUS_HANDLE schStatusHandle;
SERVICE_STATUS schServiceStatus;
HANDLE stopEvent;
HANDLE registerWaiter;
static DWORD checkPoint = 1;

/// <summary>
/// Helper method that sends the current status of this service to the SCM
/// The current state, exit code and expected waiting time is sent to the SCM when required
/// Note that the CheckPoint value is incremented everytime the service has not changed to an effective state
/// </summary>
VOID ReportServiveStatus(DWORD currentState, DWORD dwWin32ExitCode, DWORD waitHint) {

	schServiceStatus.dwCurrentState = currentState;
	schServiceStatus.dwWin32ExitCode = dwWin32ExitCode;
	schServiceStatus.dwWaitHint = waitHint;

	if (currentState == SERVICE_START_PENDING) {
		schServiceStatus.dwControlsAccepted = 0;
	}
	else {
		schServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
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

/// <summary>
/// TODO: Event Logger 
/// </summary>
VOID LogEvent(char* szFunction)
{

}

/// <summary>
/// WINAPI Ctrl Handler used by this windows service
/// Note that it can only be set to a STOP/START state
/// </summary>
void WINAPI ServiceControlHandler(DWORD dwCtrl) {
	switch (dwCtrl) {
	case SERVICE_CONTROL_STOP:
		ReportServiveStatus(SERVICE_STOP_PENDING, NO_ERROR, 3000);
		//Force poll workers to finish
		handler(0);
		Sleep(2000); //shouldn't be hardcoded; main thread should await completion of uart/db comms
		SetEvent(stopEvent);
		ReportServiveStatus(schServiceStatus.dwCurrentState, NO_ERROR, 1000);
		return;
	case SERVICE_CONTROL_INTERROGATE:
		break;
	default:
		break;
	}
	
}

/// <summary>
/// Sends a STOP signal to SCM
/// </summary>
bool sendStopSignal() {
	SC_HANDLE schSCManager = OpenSCManager(
		NULL,                   
		NULL,                   
		SC_MANAGER_ALL_ACCESS); 

	if (NULL == schSCManager)
	{
		std::cout << "Unable to open Service Manager\n" << GetFormattedSystemError() << std::endl;
		return false;
	}

	SC_HANDLE schService = OpenService(
		schSCManager,         
		SERVICE_NAME,         
		SERVICE_STOP |
		SERVICE_QUERY_STATUS);

	if (schService == NULL)
	{
		std::cout << "Unable to open Service Handler\n" << GetFormattedSystemError() << std::endl;
		CloseServiceHandle(schSCManager);
		return false;
	}
	SERVICE_STATUS status;
	::ControlService(
		schService,
		SERVICE_CONTROL_STOP,
		&status
	);
	CloseServiceHandle(schSCManager);
	CloseServiceHandle(schService);

	return true;
}

/// <summary>
/// Main worker thread for iot-monitoring::main
/// </summary>
DWORD worker_thread(LPVOID notUsed) {
	return iot_monitoring::main(h);
}

void CALLBACK CleanUp(PVOID lpParameter,BOOLEAN TimerOrWaitFired) {
	
}


/// <summary>
/// Main entrypoint of the iot-monitoring windows service application
/// Ctrl handler is immediately registered and signaled with a pending start. Note that the dwWaitHint will ensure to any other client awaiting to start this application the amount of awaiting time expected before the service has completely start
/// </summary>
void main_service(int argc, char** argv) {

	
	schStatusHandle = RegisterServiceCtrlHandlerA(
		SERVICE_NAME,
		ServiceControlHandler
	);

	if (!schStatusHandle) {
		LogEvent("RegisterServiceControlHandler");
		return;
	}

	schServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	schServiceStatus.dwServiceSpecificExitCode = EXIT_SUCCESS;

	//Report to SCM service has initiated the start procedure
	//Expects a 3s awaiting time
	ReportServiveStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

	//Create the overall Wait Handler for this service
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

	// OK, everything seems right application effectively starting in 1s
	ReportServiveStatus(SERVICE_RUNNING, NO_ERROR, 1000);

	//This thread will carry the main worker (iot-monitoring::main)
	HANDLE workerThread = ::CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)worker_thread,
		0,
		0,
		0
	);
		
	if (!workerThread) {
		std::cout << GetFormattedSystemError() << std::endl;
		goto end;
	}


	//Registering a cleanup method for when the main Wait Handler has been signaled
	RegisterWaitForSingleObject(&registerWaiter, stopEvent, CleanUp, NULL, INFINITE, WT_EXECUTEDEFAULT);
	
	//Awaits the thread to complete its execution.
	//This should occur when either the iot-monitoring::main fails or the polling workers has been stopped
	WaitForSingleObject(workerThread, INFINITE); 

	//Let's check if the thread has exited in an invalid state. If so, stop this service
	DWORD exitCode;
	if (GetExitCodeThread(workerThread, &exitCode) && exitCode != 0) {
		sendStopSignal();
	}

	end:

	ReportServiveStatus(SERVICE_STOPPED, NO_ERROR, 0);
	
}

