#define BOOST_TEST_MODULE testMTLoop
//#include <boost/test/unit_test.hpp>
#include <boost/test/included/unit_test.hpp>
#include "MTLoop.h"
#include <string>
#include <memory>


using namespace MT;

class TMockTask: public TTask {
    public:
    bool taskIsRun = false;
    TMockTask(std::string name): TTask(name) {}
    void Run(TLog& log) {
        std::cout << "RUN!!!" << std::endl;
        taskIsRun = true;
    }
};

BOOST_AUTO_TEST_SUITE(testSuiteMTLoop)

    struct TTaskFixture {

        std::shared_ptr<TLog> log;
        std::shared_ptr<TTimer> timer;
        std::shared_ptr<TMockTask> myTask;

        TTaskFixture() {
            log = std::shared_ptr<TLog>(new TLog);
            timer = std::shared_ptr<TTimer>(new TTimer);
            myTask = std::shared_ptr<TMockTask>(new TMockTask("TestTask"));
        }
       ~TTaskFixture() {
       }
    };


    BOOST_FIXTURE_TEST_CASE( testTTimeSlot, TTaskFixture ) {
        {
            TTimeSlot slot(myTask, 100);
            slot.SetStartTime(1);

            slot.Tick(*timer, *log);

            BOOST_CHECK_EQUAL(myTask->GetName(), "TestTask");
            BOOST_CHECK_EQUAL(myTask->taskIsRun, true);
        }
    }

BOOST_AUTO_TEST_SUITE_END()
