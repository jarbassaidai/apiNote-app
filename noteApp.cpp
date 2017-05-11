#include "noteApp.h"
#include "noteJsonBoost.h"


///\fn noteApp
///\arg std::string filespec
///
///< creates a store to save the notes into
///< the glue for the raw data to the note-json and then to the store-json
///
noteApp::noteApp() : srvlbClient<noteApp>()
{



    //ctor
}
///\fn openStore
///\arg filespec
///\return none
///<
void noteApp::openStore(std::string &filespec){
    m_fname = (filespec.empty()) ? "noteApp" : filespec;
    ms_js.reset(new jsonStore(m_fname));
}
///\fn ~noteApp
///\arg none
///\return none
///< deletes the jsonStore pointer
noteApp::~noteApp()
{
    //delete m_js;
    ms_js =nullptr;

}
///\fn writeNote
///\arg int length
///\arg char * data
///\return none
///< takes the raw data from a post puts it in a stringstream
///< adds a json {id:NNN} to the data and then
///< then sends boost::property_tree::ptree to the jsonStore for persistent storage
///<
void noteApp::writeNote(int length, char * data)
{

    std::string b;
    std::stringstream jsonStream;

    b.append(data,length);
    jsonStream << b;
    std::string sid = ms_js->getNextId();
    try {
        std::string ix("id");
        m_note.put(ix,sid);
        m_note.put(jsonStream);

        ms_js->writeJ(sid,m_note.getJson());
    }catch (std::exception &e){
        std::cerr << __PRETTY_FUNCTION__ << "noteApp::writeNote caught exception" << e.what() << std::endl;
    }
}
///\fn virtual writeStuff for srvlbClient interface
///\arg int length
///\arg char * data last character is zero
///\ return none
void noteApp::writeStuff(int length, char * data){
    writeNote(length,data);
}
///\fn response
///\arg std::stringstream & jsonResponse
///\return  fills in the stringStream with the ptree
///\pre writeNote called before
///\warning relies on the fact that writeNote was called previously
///\warning returns values from last call to writeNote
///\warning NOT THREAD SAFE
void noteApp::response(std::stringstream & jsonResponse)
{
    m_note.put(jsonResponse);

}
///\fn virtual getResponse for srvlbClient interface
///\arg string & returnV
///\return none
///< passed in string returned full
void noteApp::getResponse(std::string & returnV){
    std::stringstream jsonResponse;
    m_note.outToStream(jsonResponse);
    returnV.append(jsonResponse.str());
}

///\fn retrive note
///\arg std::string note id to fetch
///\arg std::stringstream & jsonRespone
///\return  fills in the note associated with passed in request id
///\warning NOT THREAD SAFE
bool noteApp::retriveNote(std::string sid, std::stringstream & jsonResponse)
{
    noteJson nj;
    bool sji =  ms_js->getJ(sid,nj.getJson());
    if (! nj.empty()){
        nj.outToStream(jsonResponse);
    }
return (sji);
}
///\fn virtual getSpecific for srvlbClient interface
///\arg string & returnV
///\arg string & query api note
///\return none
///< passed in string returned full
void noteApp::retriveSpecific(std::string &returnV,std::string & queryV)
{
    std::stringstream jsonResponse;
    jsonResponse.str("");
    retriveNote(queryV,jsonResponse);
    returnV.append(jsonResponse.str());
}
///\fn fetchAllNotes
///\arg stringstream &
///\arg std::string & token
///\return fills stringstream with all notes in an array of json elements
///< if token is not defined return all notes
///< if token is defined only return those notes whose body has the some form of the token
///\warning works with a small number of notes
///\todo  change to send in starting token and a max size so json records can be chunked
///\warning NOT THREAD SAFE
void noteApp::fetchAllNotes(std::stringstream &allnotes,std::string &token)
{
    bool getNotes = true;
    std::string sid;
    long id =0;
    while (getNotes) {
        sid = std::to_string(id);
        id++;
        std::stringstream ssrnote;
        ssrnote.str(""); // clear string in case complier is optimizing too much
        bool spji = retriveNote(sid,ssrnote);
        if (!token.empty() && spji) {
            if (! Contains(token,ssrnote))
                continue;
        }
        if (spji){
            allnotes << ssrnote.str();
        }else {
            getNotes = false;
        }
    }
    return;
}
///\fn virtual retriveItemsMatching for srvlbClient interface
///\arg string & returnV
///\arg string & query   search token
///\return none
///< passed in string returned full could be multiple notes
void noteApp::retriveItemsMatching(std::string &returnV, std::string &queryV)
{
    std::stringstream multipleNotes;
    fetchAllNotes(multipleNotes,queryV);
    returnV.append (multipleNotes.str() );
}
///\fn virtual retriveAll for srvlbClient interface
///\arg string & returnV
///\return none
///< passed in string returned full with all notes
///
void noteApp::retriveAll(std::string& returnV){
    std::stringstream allNotes;
    buildJsonArray(allNotes);
    returnV.append(allNotes.str());
}
///\fn buildJsonArray
///\arg std::stringstream &
///\return  filled stringstream
///\warning large notes will consume as much memory as needed
///< build a jsonArray from all notes in the system
///\todo  possibly write to separate jsonStorage so
///< you can chunk the answer back to the response
void noteApp::buildJsonArray(std::stringstream &ssrnote)
{
    std::stringstream buildx;
    bool getNotes = true;
    std::string sid;
    long id =0;
    pt::ptree jArray;
    pt::ptree jAElement;


    while (getNotes) {
        sid = std::to_string(id);
        id++;
        //std::stringstream ssrnote;
        bool spji = retriveNote(sid,buildx);
        if (spji){
            try {
                pt::ptree child;
                pt::read_json(buildx,child);
                for (pt::ptree::iterator pit  = child.begin(); pit != child.end(); ){
                    pt::ptree field;
                    for (int i=0 ; i < 2 ; i++ ) {
                        field.put(pit->first,pit->second.get_value<std::string>() );
                        /**
                        * debug
                        *std::cout << pit->first << ":" << pit->second.get_value<std::string>() << std::endl;
                        */
                        pit++;
                    }
                    jAElement.push_back(std::make_pair("",field));

                }

            }catch (std::exception &e){
                std::cerr << e.what() << " from noteApp::BuildJsonArray" << std::endl;
            }

        }else {
            getNotes = false;
        }
    }
    try{
        std::stringstream elm;
        pt::write_json(elm,jAElement);
        jArray.add_child("all_notes",jAElement);

        pt::write_json(ssrnote,jArray);
        /**
        * debug
        * std::cout << ssrnote.str() << std::endl;
        */

    }catch (std::exception & e){
        std::cerr << e.what() << " from noteApp::BuildJsonArray" << std::endl;
    }
}

///\fn Contains
///\arg std::string token [in]
///\arg std::stringstream data [in]
///\return true
///\brief  true if the token is in the body or the key of the json
bool noteApp::Contains(std::string & token, std::stringstream & data)
{
        std::string sdata = data.str();

        boost::smatch found;
        boost::regex expr(token.c_str(),boost::regex::perl|boost::match_partial);
        return (boost::regex_search(sdata,found,expr )) ;


}
