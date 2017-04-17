#include <boost/test/unit_test.hpp>

#include "noteJsonTest.h"
#define BOOST_PT_JSON 1

#ifdef BOOST_PT_JSON
#include "../noteJsonBoost.h"
#endif // BOOST_PT_JSON
#ifdef NOTEJSONCPP
#include "../noteJson.h"
#endif // NOTEJSONCPP

noteJsonTest::noteJsonTest()
{
    //ctor
}

noteJsonTest::~noteJsonTest()
{
    //dtor
}


BOOST_AUTO_TEST_SUITE(s_noteJson_Test)
#ifdef BOOST_PT_JSON
BOOST_AUTO_TEST_CASE(s_noteJson)
{
    noteJson note;
}
BOOST_AUTO_TEST_CASE(s_noteJson_with_ss_TEST)
{
    std::stringstream d("{\"id\":\"1\",\"body\":\"test1\"}");
    noteJson note = d ;

    std::string id = note.getId();
    std::string body = note.getBody();
    BOOST_CHECK_EQUAL(id.compare("1"),0);
    BOOST_CHECK_EQUAL(body.compare("test1"),0);

}
BOOST_AUTO_TEST_CASE(s_noteJson_put_TEST)
{
    std::stringstream d("{\"id\":\"1\",\"body\":\"test1\"}");
    noteJson note;
    note.put(d);
    std::string id = note.getId();
    std::string body = note.getBody();
    BOOST_CHECK_EQUAL(id.compare("1"),0);
    BOOST_CHECK_EQUAL(body.compare("test1"),0);

}
BOOST_AUTO_TEST_CASE(s_noteJson_put2_TEST)
{
    std::string f("id");
    std::string s("1");

    noteJson note;
    note.put(f,s);
    std::string id = note.getId();
    BOOST_CHECK_EQUAL(id.compare("1"),0);
    f="body";
    s="test1";
    note.put(f,s);
    std::string body = note.getBody();
    BOOST_CHECK_EQUAL(body.compare("test1"),0);
    id = note.getId();
    BOOST_CHECK_EQUAL(id.compare("1"),0);
}

BOOST_AUTO_TEST_CASE(s_noteJson_tooutstream_TEST)
{
    std::stringstream d("{\"id\":\"1\",\"body\":\"test1\"}");
    //std::string f("{\n    \"id\": \"1\",\n    \"body\": \"test1\"\n}");
    noteJson note;
    note.put(d);
    std::stringstream a;
    note.outToStream(a);
    noteJson dup;
    dup.put(a);
    std::string id = dup.getId();
    std::string body = dup.getBody();
    BOOST_CHECK_EQUAL(id.compare("1"),0);
    BOOST_CHECK_EQUAL(body.compare("test1"),0);

}
BOOST_AUTO_TEST_CASE(s_noteJson_infromstream_TEST)
{
    std::stringstream d("{\"id\":\"1\",\"body\":\"test1\"}");
    //std::string f("{\n    \"id\": \"1\",\n    \"body\": \"test1\"\n}");
    noteJson note;
    note.inFromStream(d);
    std::string id = note.getId();
    std::string body = note.getBody();
    BOOST_CHECK_EQUAL(id.compare("1"),0);
    BOOST_CHECK_EQUAL(body.compare("test1"),0);

}
BOOST_AUTO_TEST_CASE(s_noteJson_contains_TEST)
{
    std::stringstream d("{\"id\":\"1\",\"body\":\"test1\"}");
    //std::string f("{\n    \"id\": \"1\",\n    \"body\": \"test1\"\n}");
    noteJson note;
    note.inFromStream(d);
    std::string token("id");
    BOOST_CHECK(note.contains(token));
    token="1";
    BOOST_CHECK(note.contains(token));
    token="body";
    BOOST_CHECK(note.contains(token));
    token="test1";
    BOOST_CHECK(note.contains(token));
    token="bod";
    BOOST_CHECK(note.contains(token));
    token="fred";
    BOOST_CHECK( ! note.contains(token));

}

BOOST_AUTO_TEST_CASE(s_noteJsonArray_pushback_TEST)
{
    std::stringstream d("{\"id\":\"1\",\"body\":\"test1\"}");
    std::stringstream d1("{\"id\":\"2\",\"body\":\"test2\"}");
    noteJson note,note1;
    note.put(d);
    note1.put(d1);
    noteJsonArray na;
    na.push_back(note);
    na.push_back(note1);
    na.write(std::cout);

}
#endif // BOOST_PT_JSON
#ifdef NOTEJSONCPP
BOOST_AUTO_TEST_CASE(noteJson_Fill_Test)
{
    BOOST_TEST_MESSAGE("noteJson_Fill_test");
    noteJson tnote;
    std::string text = "{ \"body\": \"James is the test dumb\" }";
    if( ! tnote.fill(1,(char*)text.c_str(),text.size())) {
        BOOST_CHECK_EQUAL(0,1);
    }

}

BOOST_AUTO_TEST_CASE(noteJson_PrintStdOut_test)
{
    BOOST_TEST_MESSAGE("noteJson_PrintStdOut_test");
    noteJson tnote;
    std::string text = "{ \"body\": \"James is the test dumb\" }";
    if( ! tnote.fill(1,(char*)text.c_str(),text.size())) {
        BOOST_CHECK_EQUAL(0,1);
    }
    tnote.printStdOut();
}
BOOST_AUTO_TEST_CASE(noteJsonPrint_test)
{
    noteJson tnote;
    std::string text1 = "{ \"id\":\"1\",\"body\": \"cppjson makes life difficult\" }";
    if( ! tnote.fill(1,(char*)text1.c_str(),text1.size())) {
        BOOST_CHECK_EQUAL(0,1);
    }
    tnote.printJson(std::cout);
}
BOOST_AUTO_TEST_CASE(noteJson_array)
{
    Json::Value ja(Json::arrayValue);
    Json::Value j1;
    Json::Value j2;
    Json::Reader jr;
    std::string text1 = "{ \"id\":\"1\",\"body\": \"James is the test dumb\" }";
    std::string text2 = "{ \"id\":\"2\",\"body\": \"Jack is the test dumb\" }";
    jr.parse(text1,j1);
    jr.parse(text2,j2);
    ja[0]=j1;
    ja[1]=j2;
    Json::StyledStreamWriter jw;
    jw.write(std::cout,ja);
}

#endif // NOTEJSONCPP


BOOST_AUTO_TEST_SUITE_END()
