#ifndef SRC_CORE_APPLICATION_H_
#define SRC_CORE_APPLICATION_H_

#include <windows.h>

#include <string>
#include <vector>

typedef struct Application Application;

struct Window {
  HWND hwnd;
  Application* parent_app;
  std::string title;
  bool is_visible;

  explicit Window(Application* parent_app) noexcept;
  Window(HWND hwnd, Application* parent_app, std::string title,
         bool is_visible) noexcept;
  explicit Window(HWND hwnd, Application* parent_app) noexcept;

  operator std::string() const noexcept;
};

std::ostream& operator<<(std::ostream& os, const Window& win) noexcept;

struct Application {
  HANDLE p_hwnd;
  HANDLE t_hwnd;
  std::string cmd;
  DWORD pid;
  DWORD tid;
  bool is_running;
  bool is_64bit;
  std::vector<Window> windows;

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
void Windows(Application* app) noexcept;
bool TopWindow(Window* win) noexcept;

std::string GetWindowTitle(HWND hwnd);

#endif  // SRC_CORE_APPLICATION_H_
