#include <core/application.h>

#include <format>
#include <iostream>

int main() {
  // NOTE: test function

  const char* cmds[2] = {
      "C:/Home/Soft/DB Browser for SQLite/DB Browser for SQLite.exe",
      "C:/WINDOWS/system32/notepad.exe"};

  for (size_t i = 0; i < 2; ++i) {
    Application app;

    if (!Start(cmds[i], &app)) {
      std::cerr << std::format("Failed to start the app - {}\n", cmds[i]);
      return 1;
    }

    std::cout << app << "\n";

    Sleep(1000);

    Windows(&app);

    Window win{&app};
    if (!TopWindow(&win)) {
      std::cout << "Application does not have visible windows" << "\n";
    } else {
      std::cout << "Top window: " << win << "\n";
    }

    if (!Kill(&app)) {
      std::cerr << "Failed to kill the process" << "\n";
      return 1;
    }

    std::cout << "\n";
  }

  return 0;
}
