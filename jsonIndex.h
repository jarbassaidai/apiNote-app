#ifndef JSONINDEX_H
#define JSONINDEX_H

#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <ios>
#include <exception>
#include <thread>
#include <mutex>
#include "basicFile.h"

class jsonStore;



///\class jsonIndex
///\todo make it thread safe, make an iterator
///\warning NOT THREAD SAFE !
///< a helper class for the jsonStore
///< each json object is assigned and numeric id that is used to find the jason
///< after it is stored
///< none of this class is thread safe
class jsonIndex : public basicFile
{
    friend jsonStore;

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
        long getLastId();


        std::shared_ptr<jsonIndex::t_jsonIndex> getFileIndex(long idx);
        void init();
        class iterator
        {

            public:
                iterator (jsonIndex *x);
                iterator(jsonIndex *x, jsonIndex::t_jsonIndex & );
                iterator (const iterator & it) : m_ptr(it.m_ptr) , m_it_idx(it.m_it_idx) {}
                iterator & operator++();
                iterator & operator++(int) {return (operator++()); }
                bool operator ==( iterator& ri);
                bool operator !=( iterator& ri) ;
                long id() { return m_it_idx.id; }
                void setId(int x) { m_it_idx.id = x; }
                jsonIndex::t_jsonIndex & operator *() {return m_it_idx; }

                typedef enum {
                    _JII_ENDVALUE_ = -100,
                    _JII_BEGINVALUE_ = -1} e_jiIbe;

            protected:

                jsonIndex *m_ptr;
                jsonIndex::t_jsonIndex m_it_idx;

        };

        jsonIndex::iterator & begin() { return * (new jsonIndex::iterator(this));  }
        jsonIndex::iterator & end() ;
    protected:
        bool setIndex(long p,long id, long jsize);
        bool writeIndex();
        long m_curSize;

        jsonIndex::t_jsonIndex m_jIdx;
        std::shared_ptr<jsonIndex::t_jsonIndex> ms_jIdx;
        std::mutex m_writeLockI;
        jsonIndex::iterator *m_endIter;
};




#endif // JSONINDEX_H
