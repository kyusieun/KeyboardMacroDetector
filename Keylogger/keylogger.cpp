#include <chrono>
#include <string>
#include <windows.h>
#include <vector>
#include <iostream>
#include <thread>
#include <mutex>
#include <fstream>
#include <ctime>
#include <sstream>
#include <iomanip>


enum class EventType {
    KEY_DOWN,
    KEY_UP
};

struct InputEvent {
    std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
    EventType type;
    unsigned int keyCode;
};

class KeyboardLogger {
private:
    std::vector<InputEvent> events;
    bool isRunning;
    HHOOK keyboardHook;
    std::mutex eventsMutex;
    std::ofstream logFile;
    std::string filename;

    static KeyboardLogger* instance;
    static LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
        if (nCode >= 0 && instance) {
            KBDLLHOOKSTRUCT* kbStruct = (KBDLLHOOKSTRUCT*)lParam;
            InputEvent event;
            event.timestamp = std::chrono::high_resolution_clock::now();
            event.keyCode = kbStruct->vkCode;

            if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
                event.type = EventType::KEY_DOWN;
                //std::cout << "Key pressed: " << event.keyCode << std::endl;
            }
            else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
                event.type = EventType::KEY_UP;
                //std::cout << "Key released: " << event.keyCode << std::endl;
            }

            std::lock_guard<std::mutex> lock(instance->eventsMutex);
            instance->events.push_back(event);

            if (event.keyCode == VK_ESCAPE) {
                instance->isRunning = false;
            }
        }
        return CallNextHookEx(NULL, nCode, wParam, lParam);
    }

    std::string getCurrentDateTimeString() {
        auto now = std::time(nullptr);
        std::tm tm;
        localtime_s(&tm, &now);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y%m%d_%H%M%S");
        return oss.str();
    }

public:
    KeyboardLogger() : isRunning(false), keyboardHook(NULL) {
        instance = this;
        filename = "keyboard_log_" + getCurrentDateTimeString() + ".csv";
        logFile.open(filename, std::ios::out);
        if (logFile.is_open()) {
            logFile << "Timestamp,EventType,KeyCode\n";
        }
    }

    ~KeyboardLogger() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }

    void start() {
        keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);
        if (!keyboardHook) {
            std::cout << "Failed to set keyboard hook" << std::endl;
            return;
        }

        isRunning = true;
        std::cout << "Keyboard logging started... (Press ESC to exit)" << std::endl;
        std::cout << "Log file: " << filename << std::endl;
        std::cout << "Press any key to test if logging is working..." << std::endl;

        // 이벤트 출력을 위한 별도 스레드 시작
        std::thread printThread([this]() {
            while (isRunning) {
                printEvents();
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            });

        // Message loop
        MSG msg = {};
        while (isRunning && GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (keyboardHook) {
            UnhookWindowsHookEx(keyboardHook);
        }

        // 출력 스레드 종료 대기
        if (printThread.joinable()) {
            printThread.join();
        }
    }

    void printEvents() {
        std::lock_guard<std::mutex> lock(eventsMutex);
        if (!events.empty()) {
            //std::cout << "\nRecorded Events:" << std::endl;
            for (const auto& event : events) {
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                    event.timestamp.time_since_epoch());
                std::cout << "Time: " << duration.count() << "ms, "
                    << "Type: " << (event.type == EventType::KEY_DOWN ? "KEY_DOWN" : "KEY_UP")
                    << ", KeyCode: " << event.keyCode << std::endl;

                // 파일 저장
                if (logFile.is_open()) {
                    logFile << duration.count() << ","
                        << (event.type == EventType::KEY_DOWN ? "KEY_DOWN" : "KEY_UP") << ","
                        << event.keyCode << "\n";
                }
            }
            events.clear(); // 출력한 이벤트는 삭제
        }
    }
};

KeyboardLogger* KeyboardLogger::instance = nullptr;

int main() {
    KeyboardLogger logger;
    logger.start();
    return 0;
}