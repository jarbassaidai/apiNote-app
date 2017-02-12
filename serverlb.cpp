
#include <exception>
#include "serverlb.h"
#include "apiNote.h"
using namespace std::placeholders; // for `_1`


///\fn serverlb
///\arg port to listen on
///\arg ip address to listen on
///< mini webserver configured with one thread
///< compiler c++ 14 that allows for callbacks to instance methods (not static class methods)
///
serverlb::serverlb(int port,std::string ipaddr,std::set<std::string> pathList )
{

    auto resource = make_shared< Resource >( );
    resource->set_paths( pathList );
    resource->set_method_handler( "POST", std::bind (&serverlb::post_json_method_handler,this,std::placeholders::_1) );
    resource->set_method_handler("GET",std::bind (&serverlb::get_json_method_handler,this,std::placeholders::_1) );
    auto settings = make_shared< Settings >( );
    settings->set_port( port );
    settings->set_bind_address( ipaddr );

    try {
        Service service;
        service.publish( resource );
        service.start(settings );
    }
    catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }
}

serverlb::~serverlb()
{
    //dtor
}

///\fn post_method_hander
///\arg const shared_ptr<Session> session
///\return none
///< callback from serverlb for to send to to the api handler
///< not currently used
void serverlb::post_method_handler(const shared_ptr<Session > session){

    const auto request = session->get_request( );

    size_t content_length = request->get_header( "Content-Length", 0 );

    session->fetch( content_length, [ request ]( const shared_ptr< Session > session, const Bytes & body )
    {

        t_shApiNoteptr anote = apiNote::getInstance();
        //anote.writeNote(body.size(), (char *) body.data());
        anote->writeStuff(body.size(),(char *) body.data());
        session->close( OK, "Hello, World!", { { "Content-Length", "13" }, { "Connection", "close" } } );
    }
    );

}
///\fn post_json_method_hander
///\arg const shared_ptr<Session> session
///\return none
///< call back function for processing json post
///< Creates anote which intern persistantly stores note
///< and then we get a response from anote in the form of a stringstream { id:n body: ss}.
void serverlb::post_json_method_handler(const shared_ptr<Session> session){
    const auto request = session->get_request( );

    size_t content_length = request->get_header( "Content-Length", 0 );

    session->fetch( content_length, [ request ]( const shared_ptr< Session > session, const Bytes & body )
    {
         t_shApiNoteptr anote = apiNote::getInstance();
        //anote.writeNote(body.size(),(char *)body.data());
        anote->writeStuff(body.size(),(char*) body.data());
        //std::stringstream jsonResponse;
        //anote.response(jsonResponse);
        //std::string jresp = jsonResponse.str();
        std::string jresp;
        anote->getResponse(jresp);
        std::string jRlength = std::to_string(jresp.length());
        session->close( OK, jresp.c_str(), { { "Content-Length", jRlength.c_str() }, { "Connection", "close" } } );
    }
    );


}
///\fn get_json_method_hander
///\arg const shared_ptr<Session> session
///\return none
///< call back function for processing json get
///< creates anote which intern persistantly stores note
///< return { id:n body: ss}
void serverlb::get_json_method_handler(const shared_ptr<Session> session){


    const auto& request = session->get_request( );
    std::string id = request->get_path_parameter("id");
    string query = request->get_query_parameter( "query", "" );
   t_shApiNoteptr anote = apiNote::getInstance();

    std::string rsnote;
    std::string rsLength;
    std::stringstream rnote;
    bool afewnotes = false;
    if (!query.empty()){
        //anote.fetchAllNotes(rnote,query);
        anote->retriveItemsMatching(rsnote,query);
        afewnotes = true;
    }
    if (!id.empty()) {
        //anote.retriveNote(id,rnote);
        anote->retriveSpecific(rsnote,id);
        afewnotes = true;
    }
    if( afewnotes){
        //rsnote = rnote.str();
        rsLength = std::to_string(rsnote.length());
        if (rsnote.length() == 0){
            std::string nfMsg("note id >>> " + id + query + " <<< was not found\n");
            rsLength = std::to_string(nfMsg.length());
            session->close( NOT_FOUND, nfMsg , { { "Content type","application/json"},{ "Content-Length", rsLength.c_str() }, { "Connection", "close"} });
        }else {
            session->close( OK, rsnote.c_str(), { { "Content type","application/json"},{ "Content-Length", rsLength.c_str() }, { "Connection", "close"} });
        }
    }else {
        /** all notes built into an array **/
        //anote.buildJsonArray(rnote);
        //   rsnote = rnote.str();
        anote->retriveAll(rsnote);
        rsLength = std::to_string(rsnote.length());
        if (rsnote.length() == 0){
            std::string nfMsg("No notes have been found\n");
            rsLength = std::to_string(nfMsg.length());
            session->close( NOT_FOUND, nfMsg , { { "Content type","application/json"},{ "Content-Length", rsLength.c_str() }, { "Connection", "close"} });
        }else {
            session->close( OK, rsnote.c_str(), { { "Content type","application/json"},{ "Content-Length", rsLength.c_str() }, { "Connection", "close"} });
        }
        /** todo **/
        /** all notes sent individual as transfer encoding **/
         /** allNotes_te_chunk(session);  **/  /** this does work :) **/
    }


} /** end json_get_method_hander **/
///\fn te_chunk
///\arg const shared_ptr<Session> session
///\return none
///< Called from a serverlb call back function to stream large amounts of data.
///< In this  case we will be streaming individual notes when all notes have been
///< requested.
///< The main action is to format the propper trasnfer-encoding chunked headers and body, then send them.

void serverlb::allNotes_te_chunk(const shared_ptr<Session>session)
{
    std::multimap<std::string,std::string> teHeader = {{"Transfer-Encoding", "chunked"} };

        session->yield( OK, "9\r\nallNotes\n\r\n", teHeader, [ ]( const shared_ptr< Session > session )
        {
            bool getNotes = true;
            long id = 0;
            string sid;
            //std::string rsnote;
            //std::string rsLength;
            t_shApiNoteptr anote = apiNote::getInstance();

            while (getNotes) {
                sid = std::to_string(id);
                id++;
                std::string snote;
                std::stringstream ssdata;
                anote->retriveSpecific(sid,snote);
                if (!snote.empty()){
                ssdata <<std::hex << snote.length() << "\r\n" << snote  << "\r\n";
                std::string spdu = ssdata.str();
                session->yield(spdu,[ ]( const shared_ptr< Session > session ){} );
                /**
                std::stringstream ssrnote;
                std::shared_ptr<t_shApiNoteptr::t_t_shApiNoteptrInfo> noteInfo= anote.retriveNote(sid,ssrnote);
                if (noteInfo.get() != nullptr){
                    std::stringstream package ;
                    //rsnote = ssrnote.str();
                    ssrnote.seekp(0,ssrnote.end);

                    package << std::hex << ssrnote.tellp() << "\r\n" << ssrnote.str() << "\r\n" ;
                    std::string spdu = package.str();
                    session->yield(spdu,[ ]( const shared_ptr< Session > session ){} );
                */
                }else{
                    getNotes = false;
                    session->close("5\r\ndone\n\r\n0\r\n\r\n");
                }
            }
    } ); /** end of session->yield( OK... ) **/
}
