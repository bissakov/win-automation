#ifndef SRC_CORE_APPLICATION_H_
#define SRC_CORE_APPLICATION_H_

#include <windows.h>

#include <string>

struct Application {
  HANDLE p_hwnd;
  HANDLE t_hwnd;
  std::string cmd;
  DWORD pid;
  DWORD tid;
  bool is_running;
  bool is_64bit;

  Application() noexcept;
  Application(HANDLE p_hwnd, HANDLE t_hwnd, const char* cmd, DWORD pid,
              DWORD tid) noexcept;
  ~Application() noexcept;

  operator std::string() const noexcept;
};

std::ostream& operator<<(std::ostream& os, const Application& app) noexcept;

bool IsProcessRunning(DWORD process_id);
bool Start(const char* cmd, Application* app) noexcept;
bool Kill(Application* app) noexcept;

#endif  // SRC_CORE_APPLICATION_H_
