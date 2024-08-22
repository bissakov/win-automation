#include <windows.h>

#include <cstdint>
#include <cstdio>
#include <format>
#include <iostream>
#include <string>

std::string GetLastErrorAsString() noexcept {
  char buf[256];
  size_t size = FormatMessageA(
      FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
      GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf,
      (sizeof(buf) / sizeof(wchar_t)), NULL);
  std::string message(buf, size);

  return message;
}

struct Handle {
  HANDLE handle;
  uint32_t id;

  Handle() noexcept : handle(NULL), id(0) {}

  Handle(HANDLE handle, DWORD id) noexcept : handle(handle), id(id) {}

  operator std::string() const noexcept {
    return std::format("Handle(id={})", id);
  }
};

std::ostream& operator<<(std::ostream& os, const Handle& handle) noexcept {
  os << std::string(handle);
  return os;
}

struct Application {
  std::string app_name;
  Handle process_handle;
  Handle thread_handle;
  bool is_process_running;
  bool is_64bit;

  Application() noexcept : is_process_running(false), is_64bit(false) {}

  Application(const char* app_name, HANDLE process_handle, HANDLE thread_handle,
              DWORD process_id, DWORD thread_id) noexcept
      : app_name(app_name),
        process_handle(Handle{process_handle, process_id}),
        thread_handle(Handle{thread_handle, thread_id}) {}

  ~Application() noexcept {
    if (CloseHandle(process_handle.handle) == 0) {
      std::cerr << "Failed to close process handle. Error: "
                << GetLastErrorAsString() << "\n";
    }
    if (CloseHandle(thread_handle.handle) == 0) {
      std::cerr << "Failed to close thread handle. Error: "
                << GetLastErrorAsString() << "\n";
    }
  }

  operator std::string() const noexcept {
    return std::format(
        "Application(app_name=\"{}\", process_handle={}, thread_handle={}, "
        "is_process_running={}, is_64_bit={})",
        app_name, std::string(process_handle), std::string(thread_handle),
        is_process_running, is_process_running);
  }
};

std::ostream& operator<<(std::ostream& os, const Application& app) noexcept {
  os << std::string(app);
  return os;
}

static inline bool IsProcess64Bit(HANDLE process_handle) noexcept {
  BOOL isWow64 = FALSE;

  if (IsWow64Process(process_handle, &isWow64) == 0) {
    std::cerr << "Failed to determine process architecture. Error: "
              << GetLastError() << "\n";
    return false;
  }

  if (isWow64 != 0) {
    return false;
  }

  SYSTEM_INFO si;
  GetNativeSystemInfo(&si);

  return si.wProcessorArchitecture != PROCESSOR_ARCHITECTURE_INTEL;
}

static inline bool Start(const char* app_name, Application* app,
                         uint32_t timeout = 1000) noexcept {
  STARTUPINFOA si = {};
  si.cb = sizeof(si);

  PROCESS_INFORMATION pi = {};

  if (CreateProcessA(app_name, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si,
                     &pi) == 0) {
    return false;
  }

  DWORD event = WaitForSingleObject(pi.hProcess, timeout);
  if (event == WAIT_FAILED) {
    return false;
  }

  app->app_name = app_name;
  app->process_handle = Handle{pi.hProcess, pi.dwProcessId};
  app->thread_handle = Handle{pi.hThread, pi.dwThreadId};
  app->is_process_running = true;
  app->is_64bit = IsProcess64Bit(pi.hProcess);

  return true;
}

static inline bool Kill(Application* app) noexcept {
  // TODO(bissakov): Handle UWP and apps that launch processes
  // that replaces the original one

  HANDLE wait_handle =
      OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, 0, app->process_handle.id);

  if (wait_handle == NULL) {
    std::cerr << "Failed to open handle for termination. Error: "
              << GetLastErrorAsString() << "\n";
    return false;
  }

  TerminateProcess(wait_handle, 0);

  WaitForSingleObject(wait_handle, INFINITE);

  app->is_process_running = false;
  return true;
}

int main() {
  // NOTE: test function

  const char* app_name = "C:/Windows/System32/calc.exe";

  Application app;

  if (!Start(app_name, &app)) {
    std::cerr << "\"Start\" failed" << GetLastErrorAsString() << "\n";
  }

  std::cout << app << "\n";

  size_t i = 0;
  while (true) {
    if (i == 20) {
      if (!Kill(&app)) {
        std::cerr << "Failed to kill the process" << "\n";
        return 1;
      }
      break;
    }

    std::cout << "is_process_running: "
              << (app.is_process_running ? "True" : "False") << "\n";
    i += 1;
    Sleep(250);
  }

  std::cout << app << "\n";
}
