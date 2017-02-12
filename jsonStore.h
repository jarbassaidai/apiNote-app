#ifndef JSONSTORE_H
#define JSONSTORE_H


#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <ios>
#include <exception>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
namespace pt = boost::property_tree;

///\todo  add exceptions on the file io
///\class jsonIndex
///\todo make it thread safe, make an iterator
///\warning NOT THREAD SAFE !
///< a helper class for the jsonStore
///< each json object is assigned and numeric id that is used to find the jason
///< after it is stored
///< none of this class is thread safe
class jsonIndex
{
    public:
    ///\struct t_jsonIndex
    ///< fields:
    ///<  long id       assigned by jsonIndex class
    ///<  long fpos     file offset from the beginning of the file passed in
    ///<  long jsonSize size of the date in each json record passed in
    typedef struct  {
            long id;
            long fpos;
            long jsonSize;
        } t_jsonIndex;

    public:
        jsonIndex(std::string & fname);
        virtual ~jsonIndex();
        inline long getLastId() { return m_jIdx.id; }
        bool setIndex(long p,long id, long jsize);
        std::shared_ptr<jsonIndex::t_jsonIndex> getFileIndex(long idx);
        void init();



    protected:
        bool writeIndex();

        jsonIndex::t_jsonIndex m_jIdx;
        std::shared_ptr<jsonIndex::t_jsonIndex> ms_jIdx;
        std::fstream m_jfi;

};

///\class jsonStore
///< takes care of persistant storage
///\todo make it so any kind of backing store can be used
///\todo make it thread safe, make an iterator
///\warning  NOT THREAD SAFE !
///
///< basic two file storage system for storing data jasonIndex/file holds the assigned json-ids and the file offset and json-length
///< The jason file only olds the jason data
///< this is not thread save !!!
class jsonStore : protected jsonIndex
{
    public:

        /** Default constructor */
        jsonStore(std::string & filename);
        std::string  getId();
        bool writeJ(std::string &id , pt::ptree & jsonStuff);
        std::shared_ptr<jsonIndex::t_jsonIndex> getJ(std::string &id,pt::ptree &jsonStuff);
        /** Default destructor */
        virtual ~jsonStore();

    protected:
        bool openJsonStore();

    private:
        std::fstream m_jfs;
        std::string m_fname;

};


#endif // JSONSTORE_H
