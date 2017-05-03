#define BOOST_TEST_MODULE testMTLoop
#include <boost/test/unit_test.hpp>
#include "MTLoop.h"
#include <string>


using namespace MT;

BOOST_AUTO_TEST_SUITE(testSuiteMTLoop)

    BOOST_AUTO_TEST_CASE( testMTLoop ) {
        {
            BOOST_CHECK_EQUAL(1, 1);
        }
    }

BOOST_AUTO_TEST_SUITE_END()
