/*
 * RTLoop.h
 */

#ifndef MTLOOP_H
#define MTLOOP_H

#include <inttypes.h>
#include <vector>

namespace MT {

    class TStat {
    private:
        uint32_t counter = 0;
        uint32_t totalDuration = 0;
        uint32_t startTime = 0;
        uint32_t stopTime = 0;
        uint32_t maxDuration = 0;
    public:
        TStat() {}

        void Start() {
            startTime = micros();
        }

        void Stop() {
            stopTime = micros();
            uint32_t d = GetDuration();
            if (d > maxDuration)
                maxDuration = d;
            totalDuration += d;
            counter++;
        }

        uint32_t GetDuration() {
            return stopTime - startTime;
        }

        uint32_t GetAvgDuration() {
            if (counter == 0)
                return 0;
            return totalDuration / counter;
        }

        uint32_t GetMaxDuration() {
            return maxDuration;
        }

        uint32_t GetCounter() {
            return counter;
        }

        uint32_t GetStartTime() {
            return startTime;
        }

        uint32_t GetStopTime() {
            return stopTime;
        }
    };

    class TLog {
        private:
        public:
            TLog() {}
            void Log(std::string logLine = "") {
            }
    };

    class TTask {
        private:
        public:
            TTask() {}
            virtual ~TTask() {}
            virtual void Run() = 0;
    };

    class TTimeSlot {
    public:
        TStat stat;
        TLog log;

    private:
        TTask* task;
        uint32_t startTime;
        uint32_t duration;

    public:
        TTimeSlot(TTask* task, uint32_t duration = 100)
            : task(task)
            , startTime(0)
            , duration(duration) {}

        ~TTimeSlot() {
            delete task;
        }

        void Start(uint32_t currTime) {
            startTime = currTime;
            stat.Start();
            task->Run();
            stat.Stop();
        }

        bool IsStoped(uint32_t currTime) {
            return currTime >= startTime + duration;
        }
    };


    class TTimeSlotChain {
        private:
            std::vector<TTimeSlot> timeSlots;
        public:
            TTimeSlotChain(std::vector<TTimeSlot> timeSlots)
            : timeSlots(timeSlots) { }

            bool Tick() { }
    };


    class TLoop {
        private:
            std::vector<TTimeSlotChain> timeSlotChains;
        public:
            TLoop() {}
            void AddTaskChain(std::vector<TTimeSlot> timeSlots) {
                timeSlotChains.push_back(TTimeSlotChain(timeSlots));
            }
            void Tick() {
            }
    };
}

#endif
