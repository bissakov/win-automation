#include <core/application.h>
#include <win32/win32.h>
#include <windows.h>

#include <cassert>
#include <format>
#include <iostream>
#include <string>

Application::Application() noexcept
    : p_hwnd(NULL),
      t_hwnd(NULL),
      pid(0),
      tid(0),
      is_running(false),
      is_64bit(false) {}

Application::Application(HANDLE p_hwnd, HANDLE t_hwnd, const char* cmd,
                         DWORD pid, DWORD tid) noexcept
    : p_hwnd(p_hwnd),
      t_hwnd(t_hwnd),
      cmd(cmd),
      pid(pid),
      tid(tid),
      is_running(IsProcessRunning(pid)),
      is_64bit(IsProcess64Bit(p_hwnd)) {}

Application::~Application() noexcept {
  CloseHwnd(p_hwnd);
  CloseHwnd(t_hwnd);
}

Application::operator std::string() const noexcept {
  return std::format(
      "Application(p_hwnd={}, t_hwnd={}, cmd=\"{}\", "
      "pid={}, tid={}, is_running={}, is_64_bit={})",
      p_hwnd, t_hwnd, cmd, pid, tid, is_running, is_64bit);
}

std::ostream& operator<<(std::ostream& os, const Application& app) noexcept {
  os << std::string(app);
  return os;
}

bool IsProcessRunning(const DWORD process_id) {
  assert(process_id > 0);

  HANDLE process_handle =
      OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, process_id);

  if (process_handle == NULL) {
    return false;
  }

  DWORD exit_code;
  if (GetExitCodeProcess(process_handle, &exit_code) == 0) {
    return false;
  }

  return exit_code == STILL_ACTIVE;
}

bool Start(const char* cmd, Application* app) noexcept {
  STARTUPINFOA si = {};
  si.cb = sizeof(si);

  PROCESS_INFORMATION pi = {};

  DWORD creation_flags = NORMAL_PRIORITY_CLASS | CREATE_SUSPENDED;
  if (CreateProcessA(cmd, NULL, NULL, NULL, FALSE, creation_flags, NULL, NULL,
                     &si, &pi) == 0) {
    std::cerr << std::format("(Failed to create process. Error: {})",
                             GetLastErrorAsString());
    return false;
  }

  if (ResumeThread(pi.hThread) == -1) {
    std::cerr << std::format("Failed to resume a thread. Error: {}\n",
                             GetLastErrorAsString());
    app = {};
    return false;
  }

  if (!IsProcessRunning(pi.dwProcessId)) {
    std::cerr << std::format("Process {} is not running\n", pi.dwProcessId);
    app = {};
    return false;
  }

  app->p_hwnd = pi.hProcess;
  app->t_hwnd = pi.hThread;
  app->cmd = cmd;
  app->pid = pi.dwProcessId;
  app->tid = pi.dwThreadId;
  app->is_running = IsProcessRunning(app->pid);
  app->is_64bit = IsProcess64Bit(pi.hProcess);

  return true;
}

bool Kill(Application* app) noexcept {
  HANDLE wait_handle =
      OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, 0, app->pid);

  if (wait_handle == NULL) {
    std::cerr << std::format(
        "Failed to open handle for termination. Error: {}\n",
        GetLastErrorAsString());
    return false;
  }

  if (TerminateProcess(wait_handle, 0) == 0) {
    std::cerr << std::format("Failed to terminate handle. Error: {}\n",
                             GetLastErrorAsString());
    return false;
  }

  WaitForSingleObject(wait_handle, INFINITE);

  if (!CloseHwnd(wait_handle)) {
    return false;
  }

  app->is_running = false;
  return true;
}
