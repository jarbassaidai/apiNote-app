#include <unistd.h>

#include <boost/test/unit_test.hpp>
#include "../jsonIndex.h"


class setupIdx
{
    public:
    setupIdx() {


                fnI="testJson.index";

                unlink(fnI.c_str());
                cleanup = true;
            }
    ~setupIdx() {
                if (cleanup){

                    unlink(fnI.c_str());
                }
            }

    std::string fnI;

    bool cleanup;

};

class jitest : public jsonIndex
{
    public:
    jitest(std::string fn = "jitest.index"):jsonIndex(fn) { cleanup = true;}
    ~jitest () { if (cleanup) unlink("jitest.index"); }

     bool setIndex(long p,long id, long jsize) { return jsonIndex::setIndex(p,id,jsize);}
    long getLast() { return jsonIndex::getLastId();  }
    bool cleanup;

};

BOOST_AUTO_TEST_SUITE(s_jsonIndex_Test)

BOOST_AUTO_TEST_CASE(s_jsonIndex)
{
   BOOST_TEST_MESSAGE("s_jsonIndex");
    setupIdx stuff;
    jsonIndex ji(stuff.fnI);
}
BOOST_AUTO_TEST_CASE(s_jsonIndex_getLastId)
{
    BOOST_TEST_MESSAGE("s_jsonIndex_getLastId");
    setupIdx stuff;
    jsonIndex ji(stuff.fnI);
    long id = ji.getLastId();
    BOOST_CHECK_EQUAL(id,-1);

}

BOOST_AUTO_TEST_CASE(s_jsonIndex_setIndex_Test)
{
    BOOST_TEST_MESSAGE("s_jsonIndex_setIndex_Test");
    setupIdx stuff;

    jitest *x = new jitest(stuff.fnI);
    jitest &jit = *x;
    jit.cleanup = false;
    long id =jit.getLastId();
    BOOST_CHECK_EQUAL(id,-1);
    BOOST_CHECK(jit.setIndex(0,0,25));
    BOOST_CHECK(jit.setIndex(25,1,25));
    BOOST_CHECK(jit.setIndex(50,2,25));
    BOOST_CHECK(jit.setIndex(75,3,25));

    std::shared_ptr<jsonIndex::t_jsonIndex> sji;
    for (int idx = 0 ; idx < 4; idx++){
        sji = jit.getFileIndex(idx);
        if (sji.get() != nullptr)
            BOOST_CHECK_EQUAL(idx,sji->id);
        else
          BOOST_CHECK_EQUAL(idx,-idx);
    }
    delete x;
}

BOOST_AUTO_TEST_CASE(s_jsonIndex_getFileIndex_Test)
{
    BOOST_TEST_MESSAGE("s_jsonIndex_getFileIndex_Test");
    setupIdx stuff;
    jitest *x = new jitest(stuff.fnI);
    jitest &jit = *x;
    long id =jit.getLastId();
    BOOST_CHECK_EQUAL(id,-1);
    BOOST_CHECK(jit.setIndex(0,0,25));
    BOOST_CHECK(jit.setIndex(25,1,25));
    BOOST_CHECK(jit.setIndex(50,2,25));
    BOOST_CHECK(jit.setIndex(75,3,25));

    std::shared_ptr<jsonIndex::t_jsonIndex> sji;
    for (int idx = 0 ; idx < 4; idx++){
        sji = jit.getFileIndex(idx);
        if (sji.get() != nullptr)
            BOOST_CHECK_EQUAL(idx,sji->id);
        else
          BOOST_CHECK_EQUAL(idx,-idx);
    }
    BOOST_CHECK(jit.setIndex(100,4,25));
    sji = jit.getFileIndex(4);
    BOOST_CHECK_EQUAL(4,sji->id);

    delete x;
}

BOOST_AUTO_TEST_CASE(s_jsonIndex_multiple_open_Test)
{
    BOOST_TEST_MESSAGE("s_jsonIndex_multiple_open_Test");
    setupIdx stuff;
    stuff.cleanup = false;
    jitest *x = new jitest(stuff.fnI);
    jitest &jit = *x;
    long id =jit.getLastId();
    BOOST_CHECK_EQUAL(id,-1);
    BOOST_CHECK(jit.setIndex(0,0,25));
    BOOST_CHECK(jit.setIndex(25,1,25));
    BOOST_CHECK(jit.setIndex(50,2,25));
    BOOST_CHECK(jit.setIndex(75,3,25));

    std::shared_ptr<jsonIndex::t_jsonIndex> sji;
    for (int idx = 0 ; idx < 4; idx++){
        sji = jit.getFileIndex(idx);
        if (sji.get() != nullptr)
            BOOST_CHECK_EQUAL(idx,sji->id);
        else
          BOOST_CHECK_EQUAL(idx,-idx);
    }
    jit.setIndex(100,4,25);
    sji = jit.getFileIndex(4);
    BOOST_CHECK_EQUAL(4,sji->id);
    id = jit.getLastId();
    BOOST_CHECK_EQUAL(id,4);
    delete x;

    x = new jitest(stuff.fnI);
    jitest &jit1 = *x;
    id = jit1.getLastId();
    BOOST_CHECK_EQUAL(id,4);
    BOOST_CHECK(jit1.setIndex(125,5,25));
    for (int idx = 0 ; idx < 6; idx++){
        sji = jit1.getFileIndex(idx);
        if (sji.get() != nullptr)
            BOOST_CHECK_EQUAL(idx,sji->id);
        else
          BOOST_CHECK_EQUAL(idx,-idx);
    }
}

BOOST_AUTO_TEST_CASE(s_jsonIndex_all_Test)
{
    BOOST_TEST_MESSAGE("s_jsonIndex_all_Test");
    setupIdx stuff;
    jitest *x = new jitest(stuff.fnI);
    jitest &jit = *x;
    long id =jit.getLastId();
    BOOST_CHECK_EQUAL(id,-1);
    BOOST_CHECK(jit.setIndex(0,0,25));
    BOOST_CHECK(jit.setIndex(25,1,25));
    BOOST_CHECK(jit.setIndex(50,2,25));
    BOOST_CHECK(jit.setIndex(75,3,25));

    std::shared_ptr<jsonIndex::t_jsonIndex> sji;
    for (int idx = 0 ; idx < 4; idx++){
        sji = jit.getFileIndex(idx);
        if (sji.get() != nullptr)
            BOOST_CHECK_EQUAL(idx,sji->id);
        else
          BOOST_CHECK_EQUAL(idx,-idx);
    }
    BOOST_CHECK(jit.setIndex(100,4,25));
    sji = jit.getFileIndex(4);
    BOOST_CHECK_EQUAL(4,sji->id);
    id = jit.getLastId();
    BOOST_CHECK_EQUAL(id,4);

    delete x;
}

BOOST_AUTO_TEST_CASE(s_jsonIndexIterator_Test)
{
    BOOST_TEST_MESSAGE("s_jsonIndexIterator_Test");
     setupIdx stuff;
    jitest *x = new jitest(stuff.fnI);
    jitest &jit = *x;
    jsonIndex::iterator jiter = jit.begin();
    jsonIndex::t_jsonIndex& nj = *jiter;
    BOOST_CHECK_EQUAL(nj.id, jsonIndex::iterator::_JII_BEGINVALUE_);
}
BOOST_AUTO_TEST_CASE(s_jsonIndexIteratorInc_Test)
{
    BOOST_TEST_MESSAGE("s_jsonIndexIteratorInc_Test");
    setupIdx stuff;
    jitest *x = new jitest(stuff.fnI);
    jitest &jit = *x;
    long id =jit.getLastId();
    BOOST_CHECK_EQUAL(id,-1);
    BOOST_CHECK(jit.setIndex(0,0,25));
    BOOST_CHECK(jit.setIndex(25,1,25));
    BOOST_CHECK(jit.setIndex(50,2,25));
    BOOST_CHECK(jit.setIndex(75,3,25));
    int cnt = 0;
    for ( jsonIndex::iterator jiter = jit.begin(); jiter != jit.end() ; jiter++ )
    {
        jsonIndex::t_jsonIndex& nj = *jiter;
        BOOST_CHECK_EQUAL(nj.id,cnt);
        cnt++;
    }
}
BOOST_AUTO_TEST_CASE(s_jsonIndexIteratorEqual_Test)
{
    BOOST_TEST_MESSAGE("s_jsonIndexIteratorEqual_Test");
    setupIdx stuff;
    jitest *x = new jitest(stuff.fnI);
    jitest &jit = *x;
    jsonIndex::iterator jiterB = jit.begin();
    jsonIndex::iterator jiterE = jit.begin();
    BOOST_CHECK(jiterB==jiterE);
    BOOST_CHECK(jit.setIndex(0,0,25));
    jiterB = jit.begin();
    BOOST_CHECK( jiterB != jit.end());
}
BOOST_AUTO_TEST_CASE(s_jsonIndexIteratorNotEqual_Test)
{
    BOOST_TEST_MESSAGE("s_jsonIndexIteratorNotEqual_Test");
    setupIdx stuff;
    jitest *x = new jitest(stuff.fnI);
    jitest &jit = *x;
    jsonIndex::iterator jiterB = jit.begin();
    jsonIndex::iterator jiterE = jit.end();
    BOOST_CHECK(jiterB != jiterE);
    BOOST_CHECK(jit.setIndex(0,0,25));
    jiterB = jit.begin();
    BOOST_CHECK( jiterB != jit.end());
}

BOOST_AUTO_TEST_SUITE_END()
