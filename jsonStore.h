#ifndef JSONSTORE_H
#define JSONSTORE_H


#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <ios>
#include <exception>
#include <thread>
#include <mutex>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "noteJsonBoost.h"
#include "basicFile.h"
#include "jsonIndex.h"
#include "noteJsonBoost.h"

namespace pt = boost::property_tree;

///\todo  add exceptions on the file io

///\class jsonStore
///< takes care of persistant storage
///\todo make it so any kind of backing store can be used
///\todo make it thread safe, make an iterator
///\warning  NOT THREAD SAFE !
///
///< basic two file storage system for storing data jasonIndex/file holds the assigned json-ids and the file offset and json-length
///< The jason file only holds the jason data
///< this is not thread save !!!
class jsonStore : protected basicFile
{
    public:

        /** Default constructor */
        jsonStore(std::string & filename);
        std::string  getNextId();
        bool writeJ(std::string &id , _notejson_ & jsonStuff);
        ///std::shared_ptr<jsonIndex::t_jsonIndex> getJ(std::string &id,pt::ptree &jsonStuff);
        bool getJ(std::string &id, _notejson_ & jsonStuff);
        /** Default destructor */
        virtual ~jsonStore() ;

        class iterator
        {
        public:
            iterator(jsonStore *);
            iterator(jsonStore *,jsonIndex::iterator & );

            iterator(iterator & p) :m_jiit(p.m_jiit), m_js(p.m_js){}
            iterator & operator++();
            iterator & operator++(int) {return (operator++()); }
            _notejson_ & operator *() ;
            bool operator != (jsonStore::iterator & cjsi) {jsonIndex::iterator & x =*m_jiit;
                                                            jsonIndex::iterator &y = *cjsi.m_jiit;
                                                            return (x != y); }
            bool operator == (jsonStore::iterator & cjsi) {jsonIndex::iterator & x =*m_jiit;
                                                            jsonIndex::iterator &y = *cjsi.m_jiit;
                                                            return (x == y);}
            typedef enum {
             _ENDVALUE_ = -200,
             _BEGINVALUE_ = -1
            } e_jsIbe;

            protected:
                jsonIndex::iterator *m_jiit;
                jsonStore  *m_js;
                _notejson_ m_data;

        };

        jsonStore::iterator & begin(){ return * (new jsonStore::iterator(this));}
        jsonStore::iterator & end();


    protected:
        jsonIndex *m_jiPtr = nullptr;
        jsonStore::iterator *m_endIter = nullptr;
    private:
        std::string m_fname;
        std::mutex m_writeLockD;

};


#endif // JSONSTORE_H
