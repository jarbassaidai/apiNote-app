#include "jsonStore.h"



///\fn jsonIndex constructor
///\arg string & filename  passed in from jsonStore usually
///< opens or creates the index file
///< usually called from a jsonStore and not directly
///\todo throw io exceptions
///\warning NOT THREAD SAFE
///\warning call needs to maintain a manual atomic sequence for jsonStore::getId, jsonStore::writeJ, jsonIndex::setIndex, jsonIndex::writeIndex

jsonIndex::jsonIndex(std::string & fname )
{

    std::string name = fname;
    name.append(".index");
    m_jfi.open(name,std::fstream::in | std::fstream::out ); //| std::fstream::ate
    if (!m_jfi.is_open()){
        m_jfi.open(name,std::fstream::out);
        if (!m_jfi.is_open())
            std::cerr << "failed to open:" << name << std::endl;
    }
    init();
}

///\fn ~jsonIndex
///\arg none
///\return none
/// < just close the index.
jsonIndex::~jsonIndex()
{
    m_jfi.close();
}

///\fn init
///\arg none
///\return none
///< set up the file properly for first time and there after for  mutiple opens
///\warning NOT THREAD SAFE
///\todo throw io exceptions
///\todo handle a failed failed read of a structure better **
void jsonIndex::init() {

    m_jfi.seekg(0,m_jfi.end);
    if (m_jfi.bad() && ! m_jfi.eof()) {
        std::cout << "fatal error can't read index" << std::endl;
        return;
    }
    long indexLength = m_jfi.tellg();
    if (indexLength <= 0){
        m_jIdx.fpos = 0;
        m_jIdx.id = -1;
        m_jIdx.jsonSize =0;
    }else {
        long xsize = sizeof (t_jsonIndex);
        m_jfi.seekp( - xsize,m_jfi.end);
        if (m_jfi.tellp() >= 0 && ! m_jfi.bad()) {
            m_jfi.read((char *) & m_jIdx,sizeof(t_jsonIndex));
            /** if read is bad reset to begining of file could mean data loss **/
            if (m_jfi.bad()) {
                m_jIdx.fpos=0;
                m_jIdx.id = 0;
                m_jIdx.jsonSize = 0;
                m_jfi.seekp(0,m_jfi.beg);
                std::cerr << "failed read reseting index could mean data loss" <<std::endl;
            }
        }else {
            std::cerr << "failed to seek to :" << m_jfi.tellp() <<  " on jsonIndex " << std::endl;
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
    m_jIdx.id=id;
    m_jIdx.fpos= p;
    m_jIdx.jsonSize = jSize;
    return writeIndex();

}

///\fn writeIndex
///\arg none
///\return bool
///< write the index out  protected called from setIndex
bool jsonIndex::writeIndex()
{
    if (m_jfi.eof() && m_jfi.fail())
        m_jfi.clear();
    m_jfi.seekp(0,m_jfi.end); /** set up for next write **/
    if (m_jfi.good())
        m_jfi.write((const char*) &m_jIdx, sizeof(t_jsonIndex));

    if (! m_jfi.good())
     std::cerr << "failed seek jsonIndex::writeIndex eof:" << m_jfi.eof() << " bad:" << m_jfi.bad()  << " fail:"
                  << m_jfi.fail() << " pos:" << m_jfi.tellp() << std::endl;
    return (m_jfi.good());

}
///\fn getfileIndex
///\arg long index value
///\return long fileIndex
///< since we are writing sequencial ids without skips and no deletes we can calculate where the index
///< is by multiplying the index times the structure size and seeking to that file address
///\warning NOT THREAD SAFE

std::shared_ptr<jsonIndex::t_jsonIndex> jsonIndex::getFileIndex(long idx)
{
    long offset = idx * sizeof(t_jsonIndex);
    t_jsonIndex * rji = new t_jsonIndex;

    if(m_jfi.eof() && m_jfi.fail()){
        m_jfi.clear();
    }
    m_jfi.seekp(offset);

    if (m_jfi.good()){
        if (m_jfi.tellp()  != offset)
            std::cerr << "fubar jsonIndex::getFileIndex wanted position:"<< offset << " got postion " << m_jfi.tellp() << std::endl;
        m_jfi.read((char *) rji,sizeof(t_jsonIndex));
        if (!m_jfi.good() || rji->id != idx){
            delete rji;
            rji= nullptr;

        }
    }else {
        std::cerr << "failed seek jsonIndex::getfileIndex eof:" << m_jfi.eof() << " bad:" << m_jfi.bad()  << " fail:"
                  << m_jfi.fail() << "pos:" << m_jfi.tellp() << std::endl;

        delete rji;
        rji = nullptr;
    }
    m_jfi.seekp(0,m_jfi.end); /** set up for next write **/
    return std::shared_ptr<jsonIndex::t_jsonIndex>(rji);
}



///\fn jsonStore
///\arg std::string & filename
///< writes two files one is an index with file positon and id the clase jsonIndex takes are of this
///< the main json data is the other file that is written
///< this allows for a very fast lookup of the last written id number


jsonStore::jsonStore(std::string &fname) : jsonIndex(fname)
{
    m_fname = fname;
    openJsonStore();
}

jsonStore::~jsonStore()
{
    m_jfs.close();
}
///\fn openJsonStore
///\arg none
///\return bool
///< open the main json storage file
///\todo add io exceptions
bool jsonStore::openJsonStore()
{
    bool rv = true;
    if (! m_jfs.is_open() ) {
        m_jfs.open(m_fname,std::fstream::in | std::fstream::out ); //| std::fstream::ate
    }
    if (! m_jfs.good()) {
        m_jfs.open(m_fname,std::fstream::out);
        if (!m_jfs.is_open()) {
            std::cerr << " failed to open:" << m_fname << std::endl;
            rv = false;
        }
    }
    return rv;
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
std::string  jsonStore::getId()
{
    std::string rvalue;
    long lId = jsonIndex::getLastId();
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
    if (m_jfs.eof() && m_jfs.fail())
        m_jfs.clear();

     m_jfs.seekp(0,m_jfs.end) ;
     cPos = m_jfs.tellp();

    if (m_jfs.good()){
        pt::write_json(m_jfs,jsonData);
        if (m_jfs.good()) {
            long jsonSize = m_jfs.tellp() - cPos;
            rv = jsonIndex::setIndex(cPos,idx,jsonSize);
        }
    }else {
        std::cout << "failed to write data jsonStore::writeJ eof:" << m_jfi.eof() << " bad:" << m_jfi.bad()  << " fail:"
                  << m_jfi.fail() << "pos:" << m_jfi.tellp() << std::endl;


    }
    return rv;
}


///\fn getJ
///\arg boost::property_tree &  jsonData
///\return bool
///< "pt::read_json" wants to look at a whole file with one json element/complex-mult-level-element.
///< We will read out one element/complex-mult-level-element and put that in a stringstream and then have
///< read_json work on the single  element/complex-mult-level-element.
///<
std::shared_ptr<jsonIndex::t_jsonIndex>   jsonStore::getJ(std::string &id, pt::ptree & jsonData)
{
    bool rv = false;
    long idx = std::stol(id);
    std::shared_ptr<jsonIndex::t_jsonIndex> jidx = jsonIndex::getFileIndex(idx);

    if (jidx != nullptr && jidx->fpos != -1) {
        if (m_jfs.eof() && m_jfs.fail())
            m_jfs.clear();
        m_jfs.seekp(jidx->fpos,m_jfs.beg);
        if (m_jfs.good()){
            char * buff = (char *)calloc(1,jidx->jsonSize +1);
            m_jfs.read(buff,jidx->jsonSize);

            if (m_jfs.good()){
                std::string x(buff,jidx->jsonSize);
                std::stringstream js(x);

                try {
                    pt::read_json(js,jsonData);
                    rv = true;
                }catch (std::exception &e) {
                    std::cerr << "jsonStore::getJ caught exception: " << e.what() << " " << x << std::endl;
                }

            }else{
               std::cerr << "failed read jsonIndex::getJ eof:" << m_jfi.eof() << " bad:" << m_jfi.bad()  << " fail:"
                  << m_jfi.fail() << "pos:" << m_jfi.tellp() << std::endl;
            }
            cfree(buff);
        }
        if (!rv){
            jidx = nullptr;
        }
    }
    return std::shared_ptr<jsonIndex::t_jsonIndex>(jidx);
}

