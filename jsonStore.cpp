#include "jsonStore.h"





///\fn jsonStore
///\arg std::string & filename
///< writes two files one is an index with file positon and id the clase jsonIndex takes are of this
///< the main json data is the other file that is written
///< this allows for a very fast lookup of the last written id number


jsonStore::jsonStore(std::string &fname) :  basicFile(fname.append(".json"),0,1)
{
    m_fname = fname;
    m_jiPtr = new jsonIndex(fname.append(".index"));

}

jsonStore::~jsonStore()
{
    if (m_jiPtr != nullptr)
        delete m_jiPtr;
    if (m_endIter != nullptr)
        delete m_endIter;
}

///\fn getid
///\arg none
///\return std::string &
///< empty string means failure to open store
///< returns the next id in the sequence to be assigned to the new note
///\warning NOT THREAD SAFE
///\warning if called twice before a write happens jsonStore will have a hole and jsonIndex will be off for searches from that point
///\warning sequential access will not be affected
///\warning Call must maintain a manual atomic sequence of getId,writeJ
std::string  jsonStore::getNextId()
{
    std::string rvalue;
    long lId = m_jiPtr->getLastId();
    lId++;
    rvalue = std::to_string(lId );
    return std::string(rvalue);
}
///\fn writeJ
///\arg boost::property_tree &  jsonData
///\return bool
///< writes boost::property_tree::ptree to persitant storage
///\warning NOT THEAD SAFE
///\warning Call must maintain a manual atomic sequence of getId,writeJ
bool jsonStore::writeJ(std::string &id, pt::ptree & jsonData)
{
    bool rv = false;
    long idx = std::stol(id);
    long cPos = 0;

    m_writeLockD.lock();

     if (basicFile::seek(0,SEEK_END) ) {
        cPos = basicFile::tell();
        std::stringstream tbuf;
        pt::write_json(tbuf,jsonData);
        std::string x = tbuf.str();
        if ((rv = basicFile::write((void *) x.c_str(),x.length()) )) {
            long jsonSize = basicFile::tell() - cPos;
            rv = m_jiPtr->setIndex(cPos,idx,jsonSize);
        }else{
            std::cerr << __PRETTY_FUNCTION__ << " failed write on: " << basicFile::m_fn.c_str() << " error:" << errno << std::endl;
            std::cerr << __PRETTY_FUNCTION__ << " args size:" << x.length() << std::endl;
        }
    }else {
        std::cerr << __PRETTY_FUNCTION__ <<" failed seek with on:" << basicFile::m_fn.c_str() << "error:" << errno << std::endl;
     }
    m_writeLockD.unlock();
    return rv;
}


///\fn getJ
///\arg boost::property_tree &  jsonData
///\return bool
///< "pt::read_json" wants to look at a whole file with one json element/complex-mult-level-element.
///< We will read out one element/complex-mult-level-element and put that in a stringstream and then have
///< read_json work on the single  element/complex-mult-level-element.
///<
//std::shared_ptr<jsonIndex::t_jsonIndex>   jsonStore::getJ(std::string &id, _notejson_ & jsonData)
bool jsonStore::getJ(std::string &id, _notejson_ & jsonData)
{
    bool rv = false;
    long idx = std::stol(id);
    std::shared_ptr<jsonIndex::t_jsonIndex> jidx = m_jiPtr->getFileIndex(idx);

    if ( jidx != nullptr && jidx->fpos != -1) {

        if ( (rv=basicFile::seek(jidx->fpos,SEEK_SET) ) ) {
            char * buff = (char *)calloc(1,jidx->jsonSize +1);
            if (jidx->jsonSize > 0 && basicFile::read((void *) buff,jidx->jsonSize) ) {
            /**
            *   pt::read_json will read until eof so since we only one one json object lets put
            *   it into a stringstream and have pt::read_json read that.
            **/
                std::string x(buff,jidx->jsonSize);
                std::stringstream js(x);

                try {
                    pt::read_json(js,jsonData);
                    rv = true;
                }catch (std::exception &e) {
                    std::cerr << __PRETTY_FUNCTION__ <<" failed read_json on:" << basicFile::m_fn.c_str() << " caught exception: " << e.what() << " " << x << std::endl;
                }

            }else{
               std::cerr << __PRETTY_FUNCTION__ << "  failed read on:" << basicFile::m_fn.c_str() << "errno:" << errno << std::endl;
            }
            cfree(buff);
        }
        if (!rv){
            jidx = nullptr;
        }
    }
    return rv;
}

///\fn jsonStore::end
///\arg none
///\return jsonStore::iterator &
jsonStore::iterator &jsonStore::end()
{
    if (m_endIter == nullptr)
    {
        jsonIndex::iterator & jii = m_jiPtr->end();
        m_endIter= new jsonStore::iterator(this,jii);
    }
    return (*m_endIter);
}

///\fn jsonStore::iterator
///\arg jsonStore *
jsonStore::iterator::iterator(jsonStore * js)
{
    m_js = js;
    m_jiit =  new jsonIndex::iterator(js->m_jiPtr->begin());
}
///\fn jsonStore::iterator
///\arg jsonStore *
jsonStore::iterator::iterator(jsonStore * js,jsonIndex::iterator & jii)
{
    m_js = js;
    m_jiit =  new jsonIndex::iterator(jii);
}

///\fn jsonStore::iterator::operator ++
///\arg none
///\return jsonStore::iterator &
jsonStore::iterator & jsonStore::iterator::operator ++()
{

    jsonIndex::iterator & jii = *m_jiit;
    jii++;
    return *this;
}
///\fn jsonStore::iterator::operator *
///\arg none
///\return _notejson_ &
_notejson_ & jsonStore::iterator::operator *()
{
    std::string sid = std::to_string(m_jiit->id());
    m_js->getJ(sid,m_data);
    return (m_data);

}
