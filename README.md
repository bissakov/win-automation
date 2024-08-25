# Win32 automation library

## Building
```bash
# Clone the repository
git clone https://github.com/bissakov/win-automation.git

# Change directory
cd win-automation

# Build (modify the batch script if you want to target x86)
./build.bat

# Run
./build/automation.exe
```

## Future plans
- Replicate basic pywinauto features (excluding UIA).
- Connect to an already running application:
  - By PID;
  - By process path;
  - By process title;
- Think of a way of dealing with changing processes (supply the new executable name and look for processes after a certain time?).
- Interact with windows and elements.
- Print full window element tree.
- Keyboard, mouse simulation.
- Screenshots.
- Python wrapper.
