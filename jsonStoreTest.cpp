#include <unistd.h>

#include <boost/test/unit_test.hpp>
#include "../jsonStore.h"
class setup
{
    public:
    setup() {

                data1 << "{\"id\":\"0\",\"body\":\"test1\"}" ;
                data2 << "{\"id\":\"1\",\"body\":\"test2\"}" ;
                fn="testJson";
                fnI="testJson.index";
                unlink(fn.c_str());
                unlink(fnI.c_str());
                cleanup = true;
            }
    ~setup() {
                if (cleanup){
                    unlink(fn.c_str());
                    unlink(fnI.c_str());
                }
            }
    std::string fn;
    std::string fnI;
    std::stringstream data1,data2;
    bool cleanup;

};




BOOST_AUTO_TEST_SUITE(s_jsonStore_Test)

BOOST_AUTO_TEST_CASE(s_jsonStore_c_Test)
{
    BOOST_TEST_MESSAGE("s_jsonStore_c_Test");
    setup stuff;
    jsonStore js(stuff.fn);
}

BOOST_AUTO_TEST_CASE(s_jsonStore_getNextId_Test)
{
    BOOST_TEST_MESSAGE("s_jsonStore_getNextId_Test");
    setup stuff;

    jsonStore js(stuff.fn);
    std::string id = js.getNextId();
    BOOST_CHECK_EQUAL(id.compare("0"),0);
}

BOOST_AUTO_TEST_CASE(s_jsonStore_writeJ_Test)
{
    BOOST_TEST_MESSAGE("s_jsonStore_writeJ_Test");
    setup stuff;
    stuff.cleanup = false;
    jsonStore js(stuff.fn);
    std::string id = js.getNextId();
    BOOST_CHECK_EQUAL(id.compare("0"),0);

    _notejson_ jsd;
    pt::read_json(stuff.data1,jsd);
    js.writeJ(id,jsd);
    id = js.getNextId();
    BOOST_CHECK_EQUAL(id.compare("1"),0);
    pt::read_json(stuff.data2,jsd);
    js.writeJ(id,jsd);

    _notejson_ jsDup;
    js.getJ(id,jsDup);
    std::stringstream x1,x2;
    pt::write_json(x1,jsd);
    pt::write_json(x2,jsDup);
    /** debug
    std::cout << " x1:" << x1.str() << std::endl;
    std::cout << " x2:" << x2.str() << std::endl;
    **/

    BOOST_CHECK(jsd == jsDup);
}


BOOST_AUTO_TEST_CASE(s_jsonStoreIterator_Test)
{
    BOOST_TEST_MESSAGE("s_jsonStoreIterator_Test");
    setup stuff;
    //stuff.cleanup = false;
    jsonStore js(stuff.fn);
    jsonStore::iterator &jsib = js.begin();
    jsonStore::iterator &jsie = js.end();

    BOOST_CHECK(jsib != jsie);

}
BOOST_AUTO_TEST_CASE(s_jsonStoreIteratorNotEqual_Test)
{
    BOOST_TEST_MESSAGE("s_jsonStoreIteratorNotEqual_Test");
    setup stuff;
    //stuff.cleanup = false;
    jsonStore js(stuff.fn);
    _notejson_ jsd;
    jsonStore::iterator jsi = js.begin();
    bool x = (jsi != js.end() );
    BOOST_CHECK(x);
    BOOST_CHECK(jsi != js.end() );


    pt::read_json(stuff.data1,jsd);
    std::string id = "0";
    js.writeJ(id,jsd);
    jsi++;
    x = (jsi == js.begin());
    BOOST_CHECK(x);
    x = (jsi != js.end());
    BOOST_CHECK(x);
    jsi++;
    x= (jsi == js.end());
    BOOST_CHECK(x);
}
BOOST_AUTO_TEST_CASE(s_jsonStoreIteratorEqual_Test)
{
    BOOST_TEST_MESSAGE("s_jsonStoreIteratorEqual_Test");
    setup stuff;
    //stuff.cleanup = false;
    jsonStore js(stuff.fn);
    _notejson_ jsd;
    jsonStore::iterator jsi = js.begin();
    bool x = (jsi == js.begin() );
    BOOST_CHECK(x);
    BOOST_CHECK(jsi == js.begin() );
}

BOOST_AUTO_TEST_CASE(s_jsonStoreIteratorInc_Test)
{
    BOOST_TEST_MESSAGE("s_jsonStoreInteratorInc_Test");
    setup stuff;
    stuff.cleanup = false;
    jsonStore js(stuff.fn);
    _notejson_ jsd;
    pt::read_json(stuff.data1,jsd);
    std::string id = "0";
    js.writeJ(id,jsd);
    _notejson_ jsd1;
    pt::read_json(stuff.data2,jsd1);
    id = "1";
    js.writeJ(id,jsd1);
    int cnt = 0;
    jsonStore::iterator & jsie = js.end();
    jsonStore::iterator & jsie1 = js.end();
    bool x = (jsie ==jsie1) ? true : false;
    BOOST_CHECK(x);
    BOOST_CHECK(jsie == jsie1);
    x = (jsie == js.begin() );
    jsie1= js.end();
    x = (jsie1 == js.end());

    for (jsonStore::iterator jsit = js.begin();
          jsit != js.end() ;
          jsit++){
        _notejson_ & nj = *jsit;
        pt::ptree::iterator pit  = nj.begin();
        BOOST_CHECK_EQUAL(cnt,pit->second.get_value<int>());
        cnt++;
    }
}

BOOST_AUTO_TEST_SUITE_END()
