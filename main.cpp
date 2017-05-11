
#define BOOST_TEST_DYN_LINK
// either Suits or Fixtures
#define BOOST_TEST_MODULE noteJson_Test

#include <boost/test/unit_test.hpp>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <csignal>

/**
class sigCatch {

public:
    sigCatch() {
        signal(SIGSEGV,&sigCatch::bang);
        }
    void static bang(int) {
        std::cout << stackTrace().str() ;
    }
};
**/

//#define BOOST_TEST_MODULE Fixtures

//#define NORMREG DATEREG SEVEREG
//#define DEBUGREG NORMREG MEDREG THREADREG
/**
//
//  BOOST_WARN_EQUAL, BOOST_CHECK_EQUAL, BOOST_REQUIRE_EQUAL
//
//    BOOST_<level>
//    BOOST_<level>_BITWISE_EQUAL
//    BOOST_<level>_CLOSE
//    BOOST_<level>_CLOSE_FRACTION
//    BOOST_<level>_EQUAL
//    BOOST_<level>_EQUAL_COLLECTION
//    BOOST_<level>_EXCEPTION
//    BOOST_<level>_GE
//    BOOST_<level>_GT
//    BOOST_<level>_LE
//    BOOST_<level>_LT
//    BOOST_<level>_MESSAGE
//    BOOST_<level>_NE
//    BOOST_<level>_NO_THROW
//    BOOST_<level>_PREDICATE
//    BOOST_<level>_SMALL
//    BOOST_<level>_THROW
//    BOOST_ERROR
//    BOOST_FAIL
//    BOOST_IS_DEFINED
//
////
**/
/**
main(int argc, char * argv[])
{
    std::cout << "entered main and not boost test" << std::endl;

}
*/

extern "C"
{

BOOST_AUTO_TEST_SUITE(noteJson_Test)
/** should pickup other boost unit test files **/

BOOST_AUTO_TEST_CASE(junk)
{
    std::cout << "ending test run" << std::endl;
}


BOOST_AUTO_TEST_SUITE_END()

}
