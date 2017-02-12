#include "apiNote.h"



///\fn apiNote
///\arg std::string filespec
///
///< creates a store to save the notes into
///
apiNote::apiNote() : srvlbClient<apiNote>()
{



    //ctor
}
///\fn openStore
///\arg filespec
///\return none
///<
void apiNote::openStore(std::string &filespec){
    m_fname = (filespec.empty()) ? "apiNote.json" : filespec;
    ms_js.reset(new jsonStore(m_fname));
}
///\fn ~apiNote
///\arg none
///\return none
///< deletes the jsonStore pointer
apiNote::~apiNote()
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
///< then sends boost::property_tree::ptree to the jsonStore for presistant storage
///<
void apiNote::writeNote(int length, char * data)
{

    std::string b;
    std::stringstream jsonStream;

    b.append(data,length);
    jsonStream << b;
    std::string sid = ms_js->getId();
    try {
        pt::ptree pbody;
        pt::read_json(jsonStream,pbody);
        m_note.put("id",sid);
        for (pt::ptree::iterator ibody  = pbody.begin(); ibody != pbody.end(); ibody++){
            m_note.put( ibody->first,ibody->second.get_value<std::string>() );
        }
        ms_js->writeJ(sid,m_note);
    }catch (std::exception &e){
        std::cerr << "apiNote::writeNote caught exception" << e.what() << std::endl;
    }
}
///\fn virtual writeStuff for srvlbClient interface
///\arg int length
///\arg char * data last character is zero
///\ return none
void apiNote::writeStuff(int length, char * data){
    writeNote(length,data);
}
///\fn response
///\arg std::stringstream & jsonResponse
///\return  fills in the stringStream with the ptree
///\pre writeNote called before
///\warning relies on the fact that writeNote was called previously
///\warning returns values from last call to writeNote
///\warning NOT THREAD SAFE
void apiNote::response(std::stringstream & jsonResponse)
{
    pt::write_json(jsonResponse,m_note);

}
///\fn virtual getResponse for srvlbClient interface
///\arg string & returnV
///\return none
///< passed in string returned full
void apiNote::getResponse(std::string & returnV){
    std::stringstream jsonResponse;
    pt::write_json(jsonResponse,m_note);
    returnV.append(jsonResponse.str());
}

///\fn retrive note
///\arg std::string not id to fetch
///\arg std::stringstream & jsonRespone
///\return  fills in the note associated with passed in request id
///\warning NOT THREAD SAFE
std::shared_ptr<apiNote::t_apiNoteInfo> apiNote::retriveNote(std::string sid, std::stringstream & jsonResponse)
{
    pt::ptree jsonData;
    std::shared_ptr<jsonIndex::t_jsonIndex> sji =  ms_js->getJ(sid,jsonData);
    if (! jsonData.empty()){
        pt::write_json(jsonResponse,jsonData);
    }
return (sji);
}
///\fn virtual getSpecific for srvlbClient interface
///\arg string & returnV
///\arg string & query api note
///\return none
///< passed in string returned full
void apiNote::retriveSpecific(std::string &returnV,std::string & queryV)
{
    std::stringstream jsonResponse;
    jsonResponse.str("");
    retriveNote(queryV,jsonResponse);
    returnV.append(jsonResponse.str());
}
///\fn fetchAllNotes
///\arg stringstream &
///\return fills stringstream with all notes in an array of json elements
///\warning works with a small number of notes
///\todo  change to send in starting token and a max size so json records can be chunked
///\warning NOT THREAD SAFE
void apiNote::fetchAllNotes(std::stringstream &allnotes,std::string &token)
{
    bool getNotes = true;
    std::string sid;
    long id =0;
    while (getNotes) {
        sid = std::to_string(id);
        id++;
        std::stringstream ssrnote;
        ssrnote.str(""); // clear string in case complier is optimizing too much
        std::shared_ptr<jsonIndex::t_jsonIndex> spji = retriveNote(sid,ssrnote);
        if (!token.empty() && spji.get() != nullptr) {
            if (! Contains(token,ssrnote))
                continue;
        }
        if (spji.get() !=nullptr){
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
void apiNote::retriveItemsMatching(std::string &returnV, std::string &queryV)
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
void apiNote::retriveAll(std::string& returnV){
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
void apiNote::buildJsonArray(std::stringstream &ssrnote)
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
        std::shared_ptr<jsonIndex::t_jsonIndex> spji = retriveNote(sid,buildx);
        if (spji.get() !=nullptr){
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
                std::cerr << e.what() << " from apiNote::BuildJsonArray" << std::endl;
            }

        }else {
            getNotes = false;
        }
    }
    try{
        std::stringstream elm;
        pt::write_json(elm,jAElement);
        jArray.add_child("all notes",jAElement);

        pt::write_json(ssrnote,jArray);
        /**
        * debug
        * std::cout << ssrnote.str() << std::endl;
        */

    }catch (std::exception & e){
        std::cerr << e.what() << " from apiNote::BuildJsonArray" << std::endl;
    }
}

///\fn Contains
///\arg std::string token [in]
///\arg std::stringstream data [in]
///\return true
///\brief  true if the token is in the body or the key of the json
bool apiNote::Contains(std::string & token, std::stringstream & data)
{
        std::string sdata = data.str();

        boost::smatch found;
        boost::regex expr(token.c_str(),boost::regex::perl|boost::match_partial);
        return (boost::regex_search(sdata,found,expr )) ;


}
