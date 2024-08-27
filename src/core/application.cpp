#include <core/application.h>
#include <win32/win32.h>
#include <windows.h>

#include <cassert>
#include <cstdint>
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
      reinterpret_cast<uintptr_t>(p_hwnd), reinterpret_cast<uintptr_t>(t_hwnd),
      cmd, pid, tid, is_running, is_64bit);
}

std::ostream& operator<<(std::ostream& os, const Application& app) noexcept {
  os << std::string(app);
  return os;
}

Window::Window() noexcept : hwnd(NULL), is_visible(false) {}

Window::Window(HWND hwnd, std::string title, bool is_visible) noexcept
    : hwnd(hwnd), title(title), is_visible(is_visible) {}

Window::Window(HWND hwnd) noexcept
    : hwnd(hwnd),
      title(GetWindowTitle(hwnd)),
      is_visible(IsWindowVisible(hwnd) != 0) {}

Window::operator std::string() const noexcept {
  return std::format("Window(hwnd={}, title=\"{}\", is_visible={})",
                     reinterpret_cast<uintptr_t>(hwnd), title, is_visible);
}

std::ostream& operator<<(std::ostream& os, const Window& win) noexcept {
  os << std::string(win);
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
  app->is_running = true;
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

std::string GetWindowTitle(HWND hwnd) {
  char title[256];
  GetWindowTextA(hwnd, title, sizeof(title));
  return std::string(title);
}

static inline BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
  Application* app = reinterpret_cast<Application*>(lParam);
  assert(app != nullptr);

  DWORD window_pid = 0;
  GetWindowThreadProcessId(hwnd, &window_pid);

  if (window_pid == app->pid) {
    app->windows.push_back(Window{hwnd});
  }

  return TRUE;
}

void Windows(Application* app) noexcept {
  EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(app));

  for (const Window& win : app->windows) {
    std::cout << win << "\n";
  }
}
