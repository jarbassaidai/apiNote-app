#include <boost/test/unit_test.hpp>
#include "../serverlb.h"
#include "../noteApp.h"
#include "../noteJsonBoost.h"
#include <boost/thread.hpp>
#include <functional>

#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/fusion/adapted.hpp>

#include "restc-cpp/restc-cpp.h"
#include "restc-cpp/RequestBuilder.h"
#include "restc-cpp/IteratorFromJsonSerializer.h"



using namespace std;
using namespace boost;
using namespace restc_cpp;

static const std::string defaddr = "127.0.0.1";
static const std::set<std::string> defurl = {"/api/notes","/api/notes/{id: [0-9]*}"};
static const std::string http_url = "http://localhost:2015/api/notes";
static const std::string noteX = "note post put get delete options ";


class setup_server : public serverlb
{
public:

    setup_server() : serverlb(2015,(std::string &)defaddr,(std::set<std::string>&)defurl) {};
    virtual ~setup_server() {};

};

void serverThread ()
{
    setup_server serv;
}
static bool runningThread = false;
struct fixtureThread {
    fixtureThread()  {
        if (! runningThread){
          t_shnoteAppptr appPtr = noteApp::getInstance();
          std::string filespec;
          appPtr->openStore(filespec);
          boost::thread thread1( serverThread);
          runningThread = true;
        }
    }
    ~fixtureThread() {}


};
// C++ structure that match the Json entries received
// from http://jsonplaceholder.typicode.com/posts/{id}
struct Note
{

    string id ;
    string body;
};

// Add C++ reflection to the Post structure.
// This allows our Json (de)serialization to do it's magic.
BOOST_FUSION_ADAPT_STRUCT(
    Note,
    (string, id)
    (string, body)
)

//BOOST_AUTO_TEST_SUITE(s_serverlb_Test)
BOOST_FIXTURE_TEST_SUITE(s_serverlb_Test,fixtureThread)

BOOST_AUTO_TEST_CASE(c_serverlb_Test)
{
    ///setup_server serv;
}


BOOST_AUTO_TEST_CASE(c_serverlb_post_Test)
{
     BOOST_TEST_MESSAGE("c_serverlb_post_Test");
    for (int cnt = 0; cnt < 5; cnt++)
    {
        auto rest_client = RestClient::Create();
        rest_client->ProcessWithPromise([&](Context& ctx)
        {

            Note post;
            post.body = noteX;
            post.body.append(std::to_string(cnt));
            auto reply = RequestBuilder(ctx)
            .Post(http_url) // URL
            .Data(post)                                 // Data object to send
            .Execute();                                 // Do it!
            ///debug
            Note svr_post;

            SerializeFromJson(svr_post, *reply);
            BOOST_CHECK(!svr_post.id.empty());
            BOOST_CHECK(!svr_post.body.empty());
            cout << " post test Got: " << svr_post.id << " "<< svr_post.body << endl;


        }).get();

    }

}

BOOST_AUTO_TEST_CASE(c_serverlb_get_Test)
{
    BOOST_TEST_MESSAGE("c_servrlb_get_test");
   /**
    t_shnoteAppptr appPtr = noteApp::getInstance();
    std::string filespec;
    appPtr->openStore(filespec);
    boost::thread thread1( serverThread);
   **/
        /// second phase read them in and check id
    auto rest_client = RestClient::Create();
    rest_client->ProcessWithPromise([&](Context& ctx)
    {
        // Here we are again in a co-routine, running in a worker-thread.

        // Asynchronously connect to a server and fetch some data.
        auto reply = RequestBuilder(ctx)
        .Get("http://localhost:2015/api/notes/1")
        //.Header("content type","application/json");


        // Send the request.
        .Execute();

        // Dump the well protected data

        Note svr_post;
        SerializeFromJson(svr_post, *reply);
        BOOST_CHECK(!svr_post.id.empty());
        BOOST_CHECK(svr_post.body.find("options") != std::string::npos);
        cout << " get test Got: " << svr_post.id << " "<< svr_post.body << endl;

    }).get();
}

BOOST_AUTO_TEST_CASE(c_serverlb_get_all_Test)
{
    BOOST_TEST_MESSAGE("c_servrlb_get_all_test");
   /**
    t_shnoteAppptr appPtr = noteApp::getInstance();
    std::string filespec;
    appPtr->openStore(filespec);
    boost::thread thread1( serverThread);
   **/
        /// second phase read them in and check id
    auto rest_client = RestClient::Create();
    rest_client->ProcessWithPromise([&](Context& ctx)
    {
        // Here we are again in a co-routine, running in a worker-thread.

        // Asynchronously connect to a server and fetch some data.
        auto reply = RequestBuilder(ctx)
        .Get("http://localhost:2015/api/notes")
        //.Header("content type","application/json");


        // Send the request.
        .Execute();

        // Dump the well protected data
        std::string json = reply->GetBodyAsString() ;
        stringstream ss;
        ss << json;
        noteJsonArray nja(ss);
        //debug
        //cout << json << "\nsize:" << nja.size() << " esize:" << nja.esize() << endl;
        //nja.dump(cout);

        BOOST_CHECK(nja.size() >= 1);
    }).get();
}

BOOST_AUTO_TEST_CASE(c_serverlb_query_Test)
{
    BOOST_TEST_MESSAGE("c_servrlb_query_test");
   /**
    t_shnoteAppptr appPtr = noteApp::getInstance();
    std::string filespec;
    appPtr->openStore(filespec);
    boost::thread thread1( serverThread);
   **/
        /// second phase read them in and check id
    auto rest_client = RestClient::Create();
    rest_client->ProcessWithPromise([&](Context& ctx)
    {
        // Here we are again in a co-routine, running in a worker-thread.

        // Asynchronously connect to a server and fetch some data.
        auto reply = RequestBuilder(ctx)
        .Get("http://localhost:2015/api/notes")
        .Argument("query","options 3")
        //.Header("content type","application/json");


        // Send the request.
        .Execute();

        // Dump the well protected data

        Note svr_post;
        SerializeFromJson(svr_post, *reply);
        BOOST_CHECK(!svr_post.id.empty());
        BOOST_CHECK(svr_post.body.find("options 3") != std::string::npos);
        cout << " get test Got: " << svr_post.id << " "<< svr_post.body << endl;

    }).get();
}

BOOST_AUTO_TEST_SUITE_END()
