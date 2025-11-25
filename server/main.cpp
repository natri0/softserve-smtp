#include <iostream>
#include <memory>
#include <filesystem>

#include "Server.h"

#ifdef _WIN32
#include <windows.h>
#include <tchar.h>

struct ServiceGlobals {
  const TCHAR *serviceName = _T("SMTP server");
  SERVICE_STATUS_HANDLE statusHandle = nullptr;
  SERVICE_STATUS status = {};
  std::shared_ptr<Server> server = nullptr;
} g_Service;

void ReportSvcStatus(const DWORD currentState, const DWORD exitCode = NO_ERROR, const DWORD waitHint = 0) {
  g_Service.status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
  g_Service.status.dwCurrentState = currentState;
  g_Service.status.dwWin32ExitCode = exitCode;
  g_Service.status.dwWaitHint = waitHint;

  if (currentState == SERVICE_START_PENDING)
    g_Service.status.dwControlsAccepted = 0;
  else
    g_Service.status.dwControlsAccepted = SERVICE_ACCEPT_STOP;

  static DWORD checkPoint = 1;
  g_Service.status.dwCheckPoint = currentState == SERVICE_RUNNING || currentState == SERVICE_STOPPED ? 0 : checkPoint++;

  SetServiceStatus(g_Service.statusHandle, &g_Service.status);
}

VOID WINAPI ServiceCtrlHandler(const DWORD CtrlCode) {
  if (CtrlCode == SERVICE_CONTROL_STOP) {
    ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 3000);

    if (g_Service.server) {
      g_Service.server->stop();
    }

    ReportSvcStatus(SERVICE_STOPPED);
  }
}

VOID WINAPI ServiceMain(DWORD argc, LPTSTR *argv) {
  g_Service.statusHandle = RegisterServiceCtrlHandler(g_Service.serviceName, ServiceCtrlHandler);
  if (!g_Service.statusHandle) return;

  ReportSvcStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

  // change execution directory
  TCHAR szPath[MAX_PATH];
  if (GetModuleFileName(nullptr, szPath, MAX_PATH)) {
    const std::filesystem::path exePath(szPath);
    std::filesystem::current_path(exePath.parent_path());
  }

  g_Service.server = std::make_shared<Server>(true);
  if (!g_Service.server->init()) {
    ReportSvcStatus(SERVICE_STOPPED, ERROR_SERVICE_SPECIFIC_ERROR);
    return;
  }

  ReportSvcStatus(SERVICE_RUNNING);
  g_Service.server->run();
  ReportSvcStatus(SERVICE_STOPPED);
}

int _tmain(int argc, TCHAR *argv[]) {
  const SERVICE_TABLE_ENTRY ServiceTable[] = {
    {const_cast<LPTSTR>(g_Service.serviceName), static_cast<LPSERVICE_MAIN_FUNCTION>(ServiceMain)},
    {nullptr, nullptr}
  };

  if (!StartServiceCtrlDispatcher(ServiceTable)) {
    DWORD error = GetLastError();
    
    // means we're running from console
    if (error == ERROR_FAILED_SERVICE_CONTROLLER_CONNECT) {
      auto server = std::make_shared<Server>(false);

      if (server->init())
        server->run();

      return 0;
    }
    
    std::cerr << "Failed to start service, error code: " << error << std::endl;
    return static_cast<int>(error);
  }

  return 0;
}

#else

int main() {
  std::shared_ptr<Server> server = std::make_shared<Server>();

  if (server->init())
    server->run();

  return 0;
}

#endif