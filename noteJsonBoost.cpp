#include "noteJsonBoost.h"

noteJson::noteJson()
{
    //ctor
}

noteJson::~noteJson()
{
    //dtor
}

noteJson::noteJson(const noteJson& other)
{
    //copy ctor
    m_jsonNote = other.m_jsonNote;
}

noteJson& noteJson::operator=(const noteJson& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    //assignment operator
    return *this;
}
///\fn put
///\arg std::strgstream &
///\brief read an already formatted json string into the property tree
///< throws json_parser_error
noteJson& noteJson::put(std::stringstream & ss)
{
    _notejson_ pbody;
    pt::read_json(ss,pbody);
    for (_notejson_::iterator ibody  = pbody.begin(); ibody != pbody.end(); ibody++){
            m_jsonNote.put( ibody->first,ibody->second.get_value<std::string>() );
        }
    return *this;
}

///\fn put
///\arg std::string & identifier/key
///\arg std::string & body
///\brief add to a json structure , may throw json_parser_error
noteJson & noteJson::put(std::string &first,std::string &second)
{

    m_jsonNote.put(first,second);
    return *this;
}

///\fn contains
///\arg std::string &
///\return bool
bool noteJson::contains(std::string & token )
{
    bool rvalue = false;
    std::stringstream ss;
    outToStream(ss);
    std::string sdata = ss.str();
    if (!token.empty() && !sdata.empty()) {
        boost::smatch found;
        boost::regex expr(token.c_str(),boost::regex::perl|boost::match_partial);
        rvalue = boost::regex_search(sdata,found,expr ) ;
    }
    return rvalue;
}
///\fn getvalue
///\arg std::string key paired with some value
///\return std::string value
std::string noteJson::getId()
{
    _notejson_::iterator it = m_jsonNote.begin();
    std::string value;
    if (it != m_jsonNote.end())
        value = it->second.get_value<std::string>() ;

    return value;
}
/** \brief  getBody
 *
 * \param none
 * \return std::string
 *
 */
 std::string noteJson::getBody()
 {
    _notejson_::iterator it = m_jsonNote.begin();
    it++;
    std::string value;
    if (it != m_jsonNote.end())
        value = it->second.get_value<std::string>() ;

    return value;
 }

///\fn noteJsonArray
noteJsonArray::noteJsonArray()
{

}
///\fn noteJsonArray
///\arg std::istream &
///\brief get an array from an istream throws exceptions
noteJsonArray::noteJsonArray( std::istream & stuff)
{
    pt::read_json(stuff,m_noteArray);
}
///\fn ~ noteJsonArray
noteJsonArray::~noteJsonArray()
{

}

///\fn push_back
///\arg noteJson &
///\return noteJsonArray &
///< the only thing that can be pushed on to a property tree is another property tree
noteJsonArray & noteJsonArray::push_back(noteJson & nj)
{

    m_elements.push_back(std::make_pair("",nj.m_jsonNote));

    return *this;
}

///\fn write
///\arg std::ostream &
///\brief write out array  throws exceptions
void noteJsonArray::write( std::ostream & target)
{
    _notejson_ a;
    a.add_child("array",m_elements);
    pt::write_json(target,a);
}
std::ostream & noteJsonArray::dump(std::ostream & os)
{
    os << "m_elements:\n";
    pt::write_json(os,m_elements);
    os << "\nm_noteArray:\n";
    pt::write_json(os,m_noteArray);
    return os;
}
