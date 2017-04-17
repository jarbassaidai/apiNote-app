#ifndef noteJson_H
#define noteJson_H
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/regex.hpp>
#include <boost/system/system_error.hpp>
#include <iostream>
#include <fstream>

namespace pt = boost::property_tree;

typedef pt::ptree _notejson_;
class noteJsonArray;

class noteJson
{
    public:
        friend noteJsonArray;

        /** Default constructor */
        noteJson();
        noteJson(_notejson_ &nj) { m_jsonNote = nj;}
        noteJson(std::stringstream &ss) { put(ss);}
        _notejson_ & getJson() { return m_jsonNote; }
        bool empty() { return m_jsonNote.empty();}
        /** Default destructor */
        virtual ~noteJson();
        /** Copy constructor
         *  \param other Object to copy from
         */
        noteJson(const noteJson& other);
        /** Assignment operator
         *  \param other Object to assign from
         *  \return A reference to this
         */
        noteJson& operator=(const noteJson& other);

        noteJson & put(std::string &first,std::string &second);
        noteJson& put(std::stringstream& ss) ;
        ///\fn operator <<
        ///\arg std::ostream &
        ///\return std::ostream &
        ///\brief throws json_parser_error
        inline std::ostream& outToStream (std::ostream& outstream)  { pt::write_json(outstream,m_jsonNote); return outstream; }
        ///\fn operator >>
        ///\arg std::istream &
        ///\return std::istream &
        ///\brief throws json_parser_error
        inline std::istream& inFromStream (std::istream & instream)  { pt::read_json(instream,m_jsonNote); return instream;}
        std::string getId();
        std::string getBody();
        bool contains(std::string &token);



    protected:
        _notejson_ m_jsonNote;

    protected:

    private:
};
/**
*  "key":"body" basic json pair
*   multiple pairs make up an object
*  "array name" : [ "key" : "body", pair, pair,...]
*  "array name" : [  object, object, .... ]
*
*  so the array can be of single pairs or multiple pairs/objects
*  built off a property tree
*
*  the noteJson array is set property tree of the following
*  "": property tree containing
*         object: noteJson &
*  the extra property tree is the only way to build an array in a json format
*
**/


class noteJsonArray
{
    public:
        noteJsonArray();
        noteJsonArray(std::istream &);
        ~noteJsonArray();
        noteJsonArray & push_back(noteJson &gj);
        _notejson_::iterator begin() {return m_noteArray.begin(); }
        _notejson_::iterator  end() { return m_noteArray.end(); }
        void write(std::ostream& );
    protected:
        _notejson_ m_elements;
        _notejson_ m_noteArray;
};
#endif // noteJson_H
