#ifndef _READER_H
#define _READER_H

struct file_problem{};

class Reader
{
    public:
    virtual int read(char * buff, int buffsize) = 0;
    virtual bool eof() const = 0;
};
#endif
