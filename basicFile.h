#ifndef BASICFILE_H
#define BASICFILE_H

#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string.h>




class basicFile
{
    public:
        /** Default constructor */
        basicFile(std::string & fn, long rdwrSize, long trxCnt);
        /** Default destructor */
        virtual ~basicFile();
        bool read(void *buf, long size = 0);
        bool write(void *buf,long size = 0);
        inline long lastErr() { return m_lastError; }
        bool seek(long offset,int origin );
        long tell() { errno = 0; return ftell(m_fptr);}
        long fileSize () ;
    protected:
        FILE *m_fptr;
        long m_rdwrSize;
        long m_trxCnt;
        long m_lastError;
        bool m_invalid;
        std::string m_fn;

    private:
};

#endif // BASICFILE_H
