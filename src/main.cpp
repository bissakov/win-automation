#include <windows.h>

#include <cstdint>
#include <cstdio>
#include <format>
#include <iostream>
#include <string>

struct Application {
  std::string app_name;
  HANDLE process_handle;
  HANDLE thread_handle;
  uint32_t process_id;
  uint32_t thread_id;

  Application() noexcept
      : process_handle(NULL),
        thread_handle(NULL),
        process_id(0),
        thread_id(0) {}

  Application(const char* app_name, HANDLE process_handle, HANDLE thread_handle,
              DWORD process_id, DWORD thread_id) noexcept
      : app_name(app_name),
        process_handle(process_handle),
        thread_handle(thread_handle),
        process_id(process_id),
        thread_id(thread_id) {}

  operator std::string() const noexcept {
    return std::format(
        "Application(app_name=\"{}\", process_id={}, thread_id={})", app_name,
        process_id, thread_id);
  }
};

std::ostream& operator<<(std::ostream& os, const Application& app) {
  os << std::string(app);
  return os;
}

static inline bool Start(const char* app_name, Application* app,
                         uint32_t timeout = 1000) {
  STARTUPINFOA si = {};
  si.cb = sizeof(si);

  PROCESS_INFORMATION pi = {};

  if (CreateProcessA(app_name, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si,
                     &pi) == 0) {
    return false;
  }

  WaitForInputIdle(pi.hProcess, timeout);

  app->app_name = app_name;
  app->process_handle = pi.hProcess;
  app->thread_handle = pi.hThread;
  app->process_id = pi.dwProcessId;
  app->thread_id = pi.dwThreadId;

  return true;
}

static inline void CleanUp(const Application& app) {
  CloseHandle(app.process_handle);
  CloseHandle(app.thread_handle);
}

static inline void Kill(const Application& app) {
  if (TerminateProcess(app.process_handle, 0) != 0) {
    std::cout << "Process terminated successfully.\n";
  } else {
    std::cerr << "Failed to terminate the process. Error: " << GetLastError()
              << "\n";
  }
  CleanUp(app);
}

int main() {
  const char* app_name = "C:/Windows/System32/calc.exe";

  Application app;

  if (!Start(app_name, &app)) {
    printf("\"Start\" failed (%lu)\n", GetLastError());
  }

  std::cout << app << "\n";

  Kill(app);
}
