/*
 * MTLoop.h
 */

#ifndef MTLOOP_H
#define MTLOOP_H

#include <stddef.h>
#include <inttypes.h>
#include <initializer_list> // Custom initializer_list for AVR

namespace MT {

using tick_t = uint32_t;

// ///////////////////////// //
//         TTimer            //
// ///////////////////////// //
#ifdef MTLOOP_MOCK_TIMER
    class TTimer {
        public:
            static tick_t time;
            static tick_t increment;

            static tick_t GetTime();
    };

    tick_t TTimer::time = 1;
    tick_t TTimer::increment = 0;

    inline tick_t TTimer::GetTime() {
        tick_t t = time;
        time += increment;
        return t;
    }

#elif MTLOOP_DUMMY_TIMER
#else
    class TTimer {
        public:
            static tick_t GetTime();
        private:
    };

    inline tick_t TTimer::GetTime() {
        return 1;
    }
#endif


// ///////////////////////// //
//         TLog              //
// ///////////////////////// //
    class TLog {
        public:
            TLog() = default;
            virtual ~TLog() = default;
            void virtual Log(const char* logLine);
        private:
    };

    inline void TLog::Log(const char* logLine) {
    }


// ///////////////////////// //
//         IRunnable         //
// ///////////////////////// //
    class IRunnable {
        public:
            virtual bool Run(TLog& log) = 0;
            virtual ~IRunnable() = default;
    };


// ///////////////////////// //
//         TStat             //
// ///////////////////////// //
    class TStat {
        public:
            TStat();
            TStat(const TStat& stat);
            TStat& operator=(const TStat& ts);
            void SetStartTime(tick_t tm);
            void SetStopTime(tick_t tm);
            tick_t GetStartTime();
            tick_t GetStopTime();
            tick_t GetDuration();
        private:
            tick_t startTime;
            tick_t stopTime;
    };
 
    inline TStat::TStat()
            : startTime(0)
            , stopTime(0) {
    }
    inline TStat::TStat(const TStat& stat)
            : startTime(stat.startTime)
            , stopTime(stat.stopTime) {
    }
    inline TStat& TStat::operator=(const TStat& a) {
        if(this != &a) {
            startTime = a.startTime;
            stopTime = a.stopTime;
        }
        return *this;
    }
    inline void TStat::SetStartTime(tick_t tm) {
        startTime = tm;
    }
    inline void TStat::SetStopTime(tick_t tm) {
        stopTime = tm;
    }
    inline tick_t TStat::GetStartTime() {
        return startTime;
    }
    inline tick_t TStat::GetStopTime() {
        return stopTime;
    }
    inline tick_t TStat::GetDuration() {
        return stopTime - startTime;
    }


// ///////////////////////// //
//         IAdapter          //
// ///////////////////////// //
    class IAdapter: public TStat, public IRunnable {
        public:
            virtual IAdapter* Clone() const = 0;
            virtual ~IAdapter() = default;
            bool Execute(TLog& log);
    };

    inline bool IAdapter::Execute(TLog& log) {
        tick_t tm = TTimer::GetTime();
        if (Run(log)) {
            SetStartTime(tm);
            SetStopTime(TTimer::GetTime());
            return true;
        }
        return false;
    }


// ///////////////////////// //
//         TCbAdapter        //
// ///////////////////////// //
    using callbackPtr = bool(*)(TLog& log);
    class TCbAdapter: public IAdapter {
        public:
            TCbAdapter(callbackPtr cb);
            TCbAdapter(const TCbAdapter& ca);
            IAdapter* Clone() const override;
            TCbAdapter& operator=(const TCbAdapter& ts);
            bool Run(TLog& log) override;
        private:
            callbackPtr cb;
    };

    inline TCbAdapter::TCbAdapter(callbackPtr cb): IAdapter(), cb(cb) {
    }
    inline TCbAdapter::TCbAdapter(const TCbAdapter& ca): IAdapter(ca), cb(ca.cb) {
    }
    inline IAdapter* TCbAdapter::Clone() const {
        return new TCbAdapter{*this};
    }
    inline TCbAdapter& TCbAdapter::operator=(const TCbAdapter& a) {
        if(this != &a) {
            TStat::operator=(a);
            cb = a.cb;
        }
        return *this;
    }
    inline bool TCbAdapter::Run(TLog& log) {
        return cb(log);
    }


// ///////////////////////// //
//         TCbDummyAdapter        //
// ///////////////////////// //
    using callbackDummyPtr = void(*)();
    class TCbDummyAdapter: public IAdapter {
        public:
            TCbDummyAdapter(callbackDummyPtr cbd);
            TCbDummyAdapter(const TCbDummyAdapter& ca);
            IAdapter* Clone() const override;
            TCbDummyAdapter& operator=(const TCbDummyAdapter& ts);
            bool Run(TLog& log) override;
        private:
            callbackDummyPtr cbd;
    };

    inline TCbDummyAdapter::TCbDummyAdapter(callbackDummyPtr cbd): IAdapter(), cbd(cbd) {
    }
    inline TCbDummyAdapter::TCbDummyAdapter(const TCbDummyAdapter& ca): IAdapter(ca), cbd(ca.cbd) {
    }
    inline IAdapter* TCbDummyAdapter::Clone() const {
        return new TCbDummyAdapter{*this};
    }
    inline TCbDummyAdapter& TCbDummyAdapter::operator=(const TCbDummyAdapter& a) {
        if(this != &a) {
            TStat::operator=(a);
            cbd = a.cbd;
        }
        return *this;
    }
    inline bool TCbDummyAdapter::Run(TLog& log) {
        cbd();
        return true;
    }


// ///////////////////////// //
//         TTskAdapter       //
// ///////////////////////// //
    class TTskAdapter: public IAdapter {
        public:
            TTskAdapter(IRunnable& task);
            TTskAdapter(const TTskAdapter& ta);
            IAdapter* Clone() const override;
            TTskAdapter& operator=(const TTskAdapter& ts);
            bool Run(TLog& log) override;
        private:
            IRunnable& task;
    };

    inline TTskAdapter::TTskAdapter(IRunnable& task): IAdapter(), task(task) {
    }
    inline TTskAdapter::TTskAdapter(const TTskAdapter& ta): IAdapter(ta), task(ta.task) {
    }
    inline IAdapter* TTskAdapter::Clone() const {
        return new TTskAdapter{*this};
    }
    inline TTskAdapter& TTskAdapter::operator=(const TTskAdapter& a) {
        if(this != &a) {
            TStat::operator=(a);
            task = a.task;
        }
        return *this;
    }
    inline bool TTskAdapter::Run(TLog& log) {
        return task.Run(log);
    }


// ///////////////////////// //
//         TTskPtrAdapter       //
// ///////////////////////// //
    class TTskPtrAdapter: public IAdapter {
        public:
            TTskPtrAdapter(IRunnable* task);
            TTskPtrAdapter(const TTskPtrAdapter& ta);
            ~TTskPtrAdapter();
            IAdapter* Clone() const override;
            TTskPtrAdapter& operator=(const TTskPtrAdapter& ts);
            bool Run(TLog& log) override;
        private:
            IRunnable* task;
    };

    inline TTskPtrAdapter::TTskPtrAdapter(IRunnable* task): IAdapter(), task(task) {
    }
    inline TTskPtrAdapter::TTskPtrAdapter(const TTskPtrAdapter& ta): IAdapter(ta), task(ta.task) {
    }
    inline TTskPtrAdapter::~TTskPtrAdapter() {
        delete task;
    }
    inline IAdapter* TTskPtrAdapter::Clone() const {
        return new TTskPtrAdapter{*this};
    }
    inline TTskPtrAdapter& TTskPtrAdapter::operator=(const TTskPtrAdapter& a) {
        if(this != &a) {
            TStat::operator=(a);
            delete task;
            task = a.task;
        }
        return *this;
    }
    inline bool TTskPtrAdapter::Run(TLog& log) {
        return task->Run(log);
    }


// ///////////////////////// //
//         TTimeSlot         //
// ///////////////////////// //
    class TTimeSlot: public IRunnable {
    public:
        TTimeSlot(TCbAdapter ca, tick_t minDuration = 100, tick_t padding = 0);
        TTimeSlot(TCbDummyAdapter ca, tick_t minDuration = 100, tick_t padding = 0);
        TTimeSlot(TTskAdapter ta, tick_t minDuration = 100, tick_t padding = 0);
        TTimeSlot(TTskPtrAdapter ta, tick_t minDuration = 100, tick_t padding = 0);
        TTimeSlot(const TTimeSlot& ts);
        virtual ~TTimeSlot();
        TTimeSlot * Clone() const;
        TTimeSlot& operator=(const TTimeSlot& ts);
        bool Run(TLog& log) override;
        void SetStartTime(tick_t time);
        void SetMinDuration(tick_t time);
        void SetPadding(tick_t time);
        tick_t GetLTime();
        tick_t GetRTime();
    private:
        IAdapter* task;
        tick_t slotStartTime = 1;
        tick_t minDuration;
        tick_t padding;
    };

    inline TTimeSlot::TTimeSlot(TCbAdapter ca, tick_t minDuration, tick_t padding)
        : task(ca.Clone())
        , minDuration(minDuration)
        , padding(padding) {
    }
    inline TTimeSlot::TTimeSlot(TCbDummyAdapter ca, tick_t minDuration, tick_t padding)
        : task(ca.Clone())
        , minDuration(minDuration)
        , padding(padding) {
    }
    inline TTimeSlot::TTimeSlot(TTskAdapter ta, tick_t minDuration, tick_t padding)
        : task(ta.Clone())
        , minDuration(minDuration)
        , padding(padding) {
    }
    inline TTimeSlot::TTimeSlot(TTskPtrAdapter ta, tick_t minDuration, tick_t padding)
        : task(ta.Clone())
        , minDuration(minDuration)
        , padding(padding) {
    }
    inline TTimeSlot::TTimeSlot(const TTimeSlot& ts)
        : task(ts.task->Clone())
        , slotStartTime(ts.slotStartTime)
        , minDuration(ts.minDuration)
        , padding(ts.padding) {
    }
    inline TTimeSlot& TTimeSlot::operator=(const TTimeSlot& ts) {
        if(this != &ts) {
            delete task;
            task = ts.task->Clone();
            slotStartTime = ts.slotStartTime;
            minDuration = ts.minDuration;
            padding = ts.padding;
        }
        return *this;
    }
    inline TTimeSlot::~TTimeSlot() {
        delete task;
    }
    inline TTimeSlot * TTimeSlot::Clone() const {
        return new TTimeSlot(*this);
    }
    inline void TTimeSlot::SetStartTime(tick_t time) {
        slotStartTime = time;
    }
    inline void TTimeSlot::SetMinDuration(tick_t time) {
        minDuration = time;
    }
    inline void TTimeSlot::SetPadding(tick_t time) {
        padding = time;
    }
    inline bool TTimeSlot::Run(TLog& log) {
        tick_t tm = TTimer::GetTime();
        if (tm < slotStartTime)
            return false;
        if (task->GetStartTime() >= slotStartTime)
            return true;
        if (task->Execute(log))
            return true;
        return false;
    }
    inline tick_t TTimeSlot::GetLTime() {
        return slotStartTime;
    }
    inline tick_t TTimeSlot::GetRTime() {
        tick_t tm = TTimer::GetTime();
        tick_t rTime = slotStartTime + minDuration;
        if (rTime > 0)
           rTime--;
        if (task->GetStartTime() >= slotStartTime) {
            tick_t taskStopTimeWithPadding = task->GetStopTime() + padding;
            if (rTime < taskStopTimeWithPadding)
                rTime = taskStopTimeWithPadding;
        } else if (tm > rTime) {
            rTime = tm + padding;
        }
        return rTime;
    }


// ///////////////////////// //
//      TTimeSlotChain       //
// ///////////////////////// //
    using TTimeSlotPtr = TTimeSlot *;
    class TTimeSlotChain {
        public:
            TTimeSlotChain(const std::initializer_list<TTimeSlot>& ts);
            ~TTimeSlotChain();
            bool Run(TLog& log);
        private:
            TTimeSlotPtr* timeSlots;
            size_t size = 0;
            size_t curTimeSlot = 0;
    };

    inline TTimeSlotChain::TTimeSlotChain(const std::initializer_list<TTimeSlot>& ts) {
        size = ts.size();
        timeSlots = new TTimeSlotPtr[size];
        size_t i = 0;
        for (const auto& item : ts) {
            timeSlots[i++] = item.Clone();
        }
    }
    inline TTimeSlotChain::~TTimeSlotChain() {
        for (size_t i = 0; i < size; ++i) {
            delete timeSlots[i];
        }
        delete[] timeSlots;
    }
    inline bool TTimeSlotChain::Run(TLog& log) {
        TTimeSlot* ts = timeSlots[curTimeSlot];
        if (ts->Run(log)) {
             curTimeSlot = (curTimeSlot + 1) % size;
             timeSlots[curTimeSlot]->SetStartTime(ts->GetRTime() + 1);
             return true;
        };
        return false;
    }


// ///////////////////////// //
//          TLoop            //
// ///////////////////////// //
    using TTimeSlotChainPtr = TTimeSlotChain *;
    static TLog defaultLog;
    class TLoop {
        public:
            TLoop(size_t count = 10, TLog& log = defaultLog);
            ~TLoop();
            bool Attach(const std::initializer_list<TTimeSlot>& ts);
            bool Run();
        private:
            TLog& log;
            TTimeSlotChainPtr* timeSlotChains;
            size_t count;
            size_t size;
            size_t curTimeSlotChain;
    };

    inline TLoop::TLoop(size_t count, TLog& log)
        : log(log)
        , count(count)
        , size(0)
        , curTimeSlotChain(0) {
        timeSlotChains = new TTimeSlotChainPtr[size];
    }
    inline TLoop::~TLoop() {
        for (size_t i = 0; i < size; ++i)
            delete timeSlotChains[i];
        delete[] timeSlotChains;
    }
    inline bool TLoop::Attach(const std::initializer_list<TTimeSlot>& ts) {
        if (size >= count)
            return false;
        timeSlotChains[size++] = new TTimeSlotChain(ts);
        return true;
    }
    inline bool TLoop::Run() {
        bool result = timeSlotChains[curTimeSlotChain]->Run(log);
        curTimeSlotChain = (curTimeSlotChain + 1) % size;
        return result;
    }
}

#endif
