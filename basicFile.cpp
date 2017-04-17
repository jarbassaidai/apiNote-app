#include "basicFile.h"
///\fn basicFile
///\arg std::string & filename
///\arg long default read/write size
///\arg  long transaction size (default buffer size)
basicFile::basicFile(std::string & fn, long rdwrSize, long trxCnt)
{
    m_invalid = false;
    m_lastError = 0;
    errno = 0;
    m_fptr = fopen(fn.c_str(), "a+" );
    if (m_fptr == nullptr){
        m_lastError = errno;
        std::cout << __PRETTY_FUNCTION__  <<" failed to open file:"  << fn.c_str() << " error:" << strerror(errno) << std::endl;
        m_invalid = true;
    }
    m_rdwrSize = rdwrSize;
    m_trxCnt = trxCnt;
    m_fn = fn;
}

basicFile::~basicFile()
{
    if (! m_invalid)
        fclose(m_fptr);
}
///\fn read
///\arg void * buf
///\arg long size
///\brief data data from the position file pointer is at
///

bool basicFile::read(void * buf,long size)
{
    bool rval = true;
    size = (size == 0) ? m_rdwrSize : size ;
    errno = 0;
     if (! m_invalid  && size > 0 && fread(buf,size,m_trxCnt,m_fptr) != m_trxCnt){
        m_lastError = errno;
        std::cout << __PRETTY_FUNCTION__ << " failed  on:" << m_fn.c_str() << " error: " << strerror(errno) << std::endl;
        rval = false;
     }
     return rval;
}
///\fn write
///\arg void * buf
///\arg long size
///\brief write data at the position file pointer is at
///
bool basicFile::write(void *buf, long size)
{
    bool rval = true;
     size = (size == 0) ? m_rdwrSize : size ;
     errno = 0;
    if (! m_invalid  && size > 0) {
        long x = fwrite(buf,size,m_trxCnt,m_fptr);
        if ( x != m_trxCnt) {
            m_lastError = errno;
            std::cerr << __PRETTY_FUNCTION__ << " failed  on:" << m_fn.c_str() << " error: " << strerror(errno) << std::endl;
            rval = false;
        }
        fflush(m_fptr);
    }
    return rval;
}
///\fn seek
///\arg long offset
///\arg int origin (SEEK_SET, SEEK_CUR, SEEK_END)
///\brief
bool basicFile::seek(long offset, int origin)
{
    bool rval = true;
    errno=0;

    if (!m_invalid  && fseek(m_fptr,offset,origin) != 0){
        m_lastError = errno;
        std::cout << __PRETTY_FUNCTION__ << " failed  on:" << m_fn.c_str() << " error: " << strerror(errno) << std::endl;
        rval = false;
    }
    return rval;
}

///\fn fileSize
///\arg none
///\return long file size in bytes
///\brief  returns negative on failure ;

long basicFile::fileSize()
{
    long rval = -1;
    if (!m_invalid )
    {
        fseek(m_fptr,0,SEEK_END);
        rval = ftell(m_fptr);
    }
    return rval;
}
