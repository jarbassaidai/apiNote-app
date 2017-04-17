#include <unistd.h>

#include <boost/test/unit_test.hpp>
#include "../basicFile.h"


BOOST_AUTO_TEST_SUITE(s_basicFile_Test)
BOOST_AUTO_TEST_CASE(s_basicFile)
{
    std::string fn("testBf.log");
    basicFile bf(fn,10,1);
    BOOST_CHECK_EQUAL(bf.lastErr(),0);
    unlink(fn.c_str());
}

BOOST_AUTO_TEST_CASE(s_basicFile_write_Test)
{
    std::string fn("testBf.log");
    basicFile bf(fn,10,1);
    BOOST_CHECK_EQUAL(bf.lastErr(),0);
    const char *buf = "0123456789";
    BOOST_CHECK(bf.write((void *) buf));
    unlink(fn.c_str());
}

BOOST_AUTO_TEST_CASE(s_basicFile_read_Test)
{
    std::string fn("testBf.log");
    basicFile bf(fn,10,1);
    BOOST_CHECK_EQUAL(bf.lastErr(),0);
    const char *buf = "0123456789";
    BOOST_CHECK(bf.write((void *) buf));
    BOOST_CHECK(bf.seek(0,SEEK_SET));
    char *rbuf =  new char(12);
    BOOST_CHECK(bf.read((void*) rbuf));
    std::string b;
    b.append(rbuf,10);
    BOOST_CHECK_EQUAL(b.compare(buf),0);

    unlink(fn.c_str());
}


BOOST_AUTO_TEST_CASE(s_basicFile_seek_Test)
{
    std::string fn("testBf.log");
    basicFile bf(fn,10,1);
    BOOST_CHECK_EQUAL(bf.lastErr(),0);
    const char *buf = "0123456789";
    BOOST_CHECK(bf.write((void *) buf));
    BOOST_CHECK(bf.seek(0,SEEK_SET));
    char *rbuf =  new char(12);
    BOOST_CHECK(bf.read((void*) rbuf));
    std::string b;
    b.append(rbuf,10);
    BOOST_CHECK_EQUAL(b.compare(buf),0);
    BOOST_CHECK(bf.write((void *) buf,5));
    BOOST_CHECK(bf.write((void *) (buf+5),5));
    BOOST_CHECK(bf.seek(15,SEEK_SET));
    BOOST_CHECK(bf.read((void*) rbuf,5));
    b.clear();
    b.append(rbuf,5);
    ///std::cout <<  (buf+5) << " " << b.c_str() << std::endl;
    BOOST_CHECK_EQUAL(b.compare("56789"),0);



    unlink(fn.c_str());
}

BOOST_AUTO_TEST_CASE(s_basicFile_tell_Test)
{
    std::string fn("testBf.log");
    basicFile bf(fn,10,1);
    BOOST_CHECK_EQUAL(bf.lastErr(),0);
    const char *buf = "0123456789";
    BOOST_CHECK(bf.write((void *) buf));
    BOOST_CHECK_EQUAL(bf.tell(),10);
    BOOST_CHECK(bf.seek(0,SEEK_SET));
    char *rbuf =  new char(12);
    BOOST_CHECK(bf.read((void*) rbuf));
    BOOST_CHECK_EQUAL(bf.tell(),10);
    std::string b;
    b.append(rbuf,10);
    BOOST_CHECK_EQUAL(b.compare(buf),0);
    BOOST_CHECK(bf.write((void *) buf,5));
    BOOST_CHECK_EQUAL(bf.tell(),15);
    BOOST_CHECK(bf.write((void *) (buf+5),5));
    BOOST_CHECK_EQUAL(bf.tell(),20);
    BOOST_CHECK(bf.seek(15,SEEK_SET));
    BOOST_CHECK_EQUAL(bf.tell(),15);
    BOOST_CHECK(bf.read((void*) rbuf,5));
    BOOST_CHECK_EQUAL(bf.tell(),20);
    b.clear();
    b.append(rbuf,5);
    ///std::cout <<  (buf+5) << " " << b.c_str() << std::endl;
    BOOST_CHECK_EQUAL(b.compare("56789"),0);



    unlink(fn.c_str());
}
BOOST_AUTO_TEST_SUITE_END()
