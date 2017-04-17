#include "jsonIndex.h"

///\fn jsonIndex constructor
///\arg string & filename  passed in from jsonStore usually
///< opens or creates the index file
///< usually called from a jsonStore and not directly
///\todo throw io exceptions
///\warning NOT THREAD SAFE
///\warning call needs to maintain a manual atomic sequence for jsonStore::getId, jsonStore::writeJ, jsonIndex::setIndex, jsonIndex::writeIndex

jsonIndex::jsonIndex(std::string & fname ) : basicFile(fname, sizeof(t_jsonIndex), 1)
{
    if (basicFile::m_invalid)
        std::cerr << "failed to open:" << fname << std::endl;
    m_endIter = nullptr;
    init();
}

///\fn ~jsonIndex
///\arg none
///\return none
/// < just close the index.
jsonIndex::~jsonIndex()
{
    if (m_endIter != nullptr)
        delete m_endIter;
}

///\fn init
///\arg none
///\return none
///< set up the file properly for first time and there after for  mutiple opens
///\warning NOT THREAD SAFE
///\todo throw io exceptions
///\todo handle a failed failed read of a structure better **
void jsonIndex::init() {

    if (! basicFile::seek(0,SEEK_END) )
    {
        std::cerr << "jsonIndex::init fatal error can't read index" << std::endl;
        return;
    }
    long indexLength = basicFile::tell();
    m_curSize = indexLength;
    if (indexLength <= 0){
        m_jIdx.fpos = 0;
        m_jIdx.id = -1;
        m_jIdx.jsonSize =0;
    }else {
        long xsize = sizeof (t_jsonIndex);
        if (basicFile::seek(- xsize,SEEK_END)  )
        {
            if ( ! basicFile::read((void *) & m_jIdx) ) {
                m_jIdx.fpos=0;
                m_jIdx.id = 0;
                m_jIdx.jsonSize = 0;
                std::cerr << "jsonIndex::init failed read reseting index could mean data loss" <<std::endl;
            }

        }else {
            std::cerr << "jsonIndex::init failed to seek on:" << m_fn.c_str() << " to :" << m_curSize - sizeof(t_jsonIndex) <<  " on jsonIndex::init " << std::endl;
        }
    }


}
///\fn setIndex
///\arg long file postion
///\arg long id
///< set the values of the index and write them out
///< called from jsonStore write
bool jsonIndex::setIndex(long p, long id, long jSize)
{
    m_writeLockI.lock();
    m_jIdx.id=id;
    m_jIdx.fpos= p;
    m_jIdx.jsonSize = jSize;
    bool rval =  writeIndex();
    m_writeLockI.unlock();
    return rval;

}

///\fn writeIndex
///\arg none
///\return bool
///< write the index out  protected called from setIndex
///< locked by setIndex
bool jsonIndex::writeIndex()
{
    bool rval;
    if ( (rval = basicFile::seek(m_jIdx.id * sizeof(t_jsonIndex),SEEK_SET) ) )
    {
        if (! (rval = basicFile::write((void *) &m_jIdx ) ) ) {
            std::cerr << "jsonIndex::writeIndex failed seek jsonIndex::writeIndex on:" << m_fn.c_str()  << " error:" << errno << std::endl;
        }else{
            m_curSize = basicFile::fileSize();
        }
    }
    return rval;
}
///\fn getfileIndex
///\arg long index value
///\return long fileIndex
///< since we are writing sequential ids without skips and no deletes we can calculate where the index
///< is by multiplying the index times the structure size and seeking to that file address
///\warning NOT THREAD SAFE

std::shared_ptr<jsonIndex::t_jsonIndex> jsonIndex::getFileIndex(long idx)
{
    long offset = idx * sizeof(t_jsonIndex);
    t_jsonIndex * rji = new t_jsonIndex;
    rji->fpos=-1;
    rji->id = -1;
    rji->jsonSize = -1;

    if (basicFile::seek(offset,SEEK_SET) ){
        if ( ! basicFile::read((void *) rji) ) {
            std::cerr << __PRETTY_FUNCTION__ <<" fread failure on:" << m_fn.c_str() << " errno:" << errno << std::endl;
            std::cerr << __PRETTY_FUNCTION__ << "args idx:" << idx << " offset:" << offset << std::endl;
            delete rji;
            rji = nullptr;
        }
    }else
        std::cout << __PRETTY_FUNCTION__ << "fseek failure on:" << m_fn.c_str() << "errno:" << errno <<  " " << strerror(errno) << std::endl;

    return std::shared_ptr<jsonIndex::t_jsonIndex>(rji);
}
///\fn getLastId();
///\arg none
///\return  long
///< seek to the end of the file backup one structure size
///< read the structure

long jsonIndex::getLastId()
{
    long rval = (m_curSize == 0) ? -1 : -2;
    if (rval == -2){
        if (basicFile::seek( basicFile::fileSize() - sizeof (t_jsonIndex), SEEK_SET)) {
            t_jsonIndex rji;
            rji.id= -1;
            if (basicFile::read((void *) &rji) )
                rval = rji.id;
            else
                std::cout << __PRETTY_FUNCTION__ << " read failed on:" << m_fn.c_str() << "errno:" << errno << std::endl;
        }else{
            std::cout << __PRETTY_FUNCTION__ << " seek failed on:" << m_fn.c_str() << "errno:" << errno << std::endl;
        }
    }

    return rval;
}
///\fn jsonIndex::end()
///\arg none
///\return jsonIndex::iterator &
jsonIndex::iterator & jsonIndex::end ()
{
    if ( m_endIter == nullptr){
        m_endIter = new jsonIndex::iterator(this);
        m_endIter->setId((int) jsonIndex::iterator::_JII_ENDVALUE_);
    }
    return (*m_endIter) ;
}

///\fn jsonIndex::iterator::operator
///\arg none
///\return jsonIndex::iterator &
jsonIndex::iterator::iterator (jsonIndex * ji)
{
    m_ptr = ji;
    if (m_ptr->fileSize() > 0){
        std::shared_ptr<jsonIndex::t_jsonIndex> sji = m_ptr->getFileIndex(0);
            if (sji.get() != nullptr)
                memcpy(&m_it_idx,(void*)sji.get(), sizeof(jsonIndex::t_jsonIndex) );
    }else{
        m_it_idx.fpos=0;
        m_it_idx.id = _JII_BEGINVALUE_;
        m_it_idx.jsonSize = 0;
    }
}
///\fn jsonIndex::iterator::operator
///\arg none
///\return jsonIndex::iterator &
jsonIndex::iterator::iterator (jsonIndex * ji,jsonIndex::t_jsonIndex & jitS)
{
    m_ptr = ji;
    jsonIndex::t_jsonIndex *x = &jitS;
    memcpy(&m_it_idx,(void*) x, sizeof(jsonIndex::t_jsonIndex) );
}
///\fn jsonIndex::iterator::operator ++
///\arg none
///\return jsonIndex::iterator &
jsonIndex::iterator & jsonIndex::iterator::operator ++()
{
   if (m_ptr->getLastId() >= (m_it_idx.id + 1) && m_it_idx.id != _JII_ENDVALUE_) {
    std::shared_ptr<jsonIndex::t_jsonIndex> sji = m_ptr->getFileIndex(m_it_idx.id + 1);
    if (sji.get() != nullptr)
        memcpy(&m_it_idx,(void*)sji.get(), sizeof(jsonIndex::t_jsonIndex) );
    else
        m_it_idx.id = _JII_ENDVALUE_;
   }else{
        m_it_idx.id = _JII_ENDVALUE_;
}
    return *this;
}

///\fn jsonIndex::iterator::operator !=
///\arg jsonIndex::iterator &
///\return bool
bool jsonIndex::iterator::operator != (jsonIndex::iterator & cjii)
{
    return ((m_it_idx.id != cjii.m_it_idx.id));
}
///\fn jsonIndex::iterator::operator ==
///\arg jsonIndex::iterator &
///\return bool
bool jsonIndex::iterator::operator == (jsonIndex::iterator & cjii)
{
    return ((m_it_idx.id == cjii.m_it_idx.id));
}
