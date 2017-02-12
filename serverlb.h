#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <cstdlib>
#include <memory>
#include <cstdlib>
#include <restbed>
#include <functional>


using namespace std;
using namespace restbed;
///\class serverClient
///< a class that allows the sever to things with the data received and to get responses to send back
///< a pure virutal class


template <class T>
class srvlbClient
{
    public:
        virtual void writeStuff(int size, char * dataz) = 0;
        virtual void getResponse(std::string& returnV) = 0;
        virtual void retriveItemsMatching(std::string & returnV,std::string & queryV) =0;
        virtual void retriveSpecific(std::string & returnV,std::string &queryV) = 0;
        virtual void retriveAll(std::string &returnV) =0;
    protected:
        srvlbClient() {};

};


///\class serverlb
///< notes: needs to be compiled with c++11 or higher to allowd for callbacks to
///< instance methods (not class static methods)
///< this was built initially wiht c++14

class serverlb
{
    public:
        /** Default constructor */
        serverlb(int port=1984,
                std::string ipaddr = std::string("127.0.0.1"),
                std::set<std::string> urladdr= std::set<std::string>({"/api/notes","/api/notes/{id: [0-9]*}"}));
        /** Default destructor */
        virtual ~serverlb();

    protected:
        void post_method_handler(const shared_ptr< Session > session );
        void post_json_method_handler(const shared_ptr< Session > session );
        void get_json_method_handler(const shared_ptr< Session > session );
        void json_handle_query(const shared_ptr< Session > session );
        void allNotes_te_chunk(const shared_ptr<Session>session);


    private:
};

#endif // SERVER_H
