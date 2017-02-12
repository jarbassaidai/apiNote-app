#ifndef APINOTE_H
#define APINOTE_H


#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/regex.hpp>
#include <boost/system/system_error.hpp>
#include <boost/filesystem.hpp>
#include "serverlb.h"
#include "jsonStore.h"

// Short alias for this namespace
namespace pt = boost::property_tree;
///\class apiNote
///\warning NOT THREAD SAFE
///< takes the json data passed in from a post request and
///< sends it to a jsonStore
///< retrives individual notes or all stored notes
///< because of the singleton every thing would be stored under on user at this point
///
const std::string c_none;

template < class T>
class singleton{
public:
    std::shared_ptr<T> static getInstance()
    {
        static std::shared_ptr<T> s_instance;
        if (s_instance.get() == nullptr){
            s_instance.reset(new T );
        }
        return s_instance;
    }
};

class apiNote : public srvlbClient<apiNote>
{
    public:
        typedef jsonIndex::t_jsonIndex t_apiNoteInfo;

        /** Default constructor */
        apiNote();
        /** set of calls that make up the srvlbClient **/
        std::shared_ptr<apiNote> static getInstance() { singleton<apiNote> s_instance ; return s_instance.getInstance();};
        virtual void writeStuff(int length, char * data);
        virtual void getResponse(std::string & returnV);
        virtual void retriveItemsMatching(std::string &returnV, std::string &queryV);
        virtual void retriveSpecific(std::string &returnV,std::string & queryV);
        virtual void retriveAll(std::string& returnV);

        /** Default destructor */
        virtual ~apiNote();
        void openStore(std::string &filename);

    protected:

        void writeNote(int length, char * data);
        void response(std::stringstream & jsonResponse );
        std::shared_ptr<jsonIndex::t_jsonIndex> retriveNote(std::string sid, std::stringstream & jsonResponse);
        void fetchAllNotes(std::stringstream & jsonResponse, std::string &token = (std::string &)c_none);
        bool Contains(std::string & token, std::stringstream & data);
        void buildJsonArray(std::stringstream & );
    protected:
        std::string m_fname;
        pt::ptree  m_note;
        //jsonStore *m_js;
        std::shared_ptr<jsonStore> ms_js;

    private:
};

typedef std::shared_ptr<apiNote> t_shApiNoteptr;
#endif // APINOTE_H
