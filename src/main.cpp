#define BOOST_TEST_MODULE testMTLoop

#define DEBUG
#define MTLOOP_MOCK_TIMER
#include "MTLoop.h"
#include <string>
#include <memory>
#include <vector>
#include <iostream>

using namespace MT;

    struct TMockLog: public TLog {
        std::vector<std::string> logLines;

        void Log (const char* logLine) {
            std::string strLogLine{logLine};
            logLines.push_back(strLogLine);
        }
    } log;

    struct TMyTask: public IRunnable {
        std::string msg;
        public:
            TMyTask(): msg("TMyTask IS RUN") { }
            TMyTask(std::string msg): msg(msg) { }
        virtual bool Run(TLog& log) {
            log.Log(msg.c_str());
            return true;
        }
    } myTaskStruct;

int main () {

    std::cout << "L1 ====" << std::endl;
    TLoop mtLoop { };
    mtLoop.Attach(
        {
            { { [](TLog& log){ log.Log((char*)"TASK1 IS RUN"); return true; } }, 100, 10 },
            { { [](TLog& log){ log.Log((char*)"TASK2 IS RUN"); return true; } }, 100, 20 },
            { { [](TLog& log){ log.Log((char*)"TASK3 IS RUN"); return true; } }, 50, 10  }
        }
    );

    std::cout << "L2 ====" << std::endl;
    std::cout << "mtLoop=" << std::hex << &mtLoop << std::endl;
    TTimer::time = 50;
    mtLoop.Run();

    return 0;
}
