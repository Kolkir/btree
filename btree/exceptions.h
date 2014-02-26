#ifndef _EXCEPTIONS_H_
#define _EXCEPTIONS_H_

#include <stdexcept>
#include <exception>

namespace btree
{
    class BTreeException : public std::exception
    {
    public:
        explicit BTreeException(const std::string& msg)
            : msg(msg.c_str())
        {
        }

        explicit BTreeException(const char *msg)
            : msg(msg)
        {
        }
        virtual const char* what()
        {
            return msg.c_str();
        }
    private:
        std::string msg;
    };

    class InvalidKeyLength : public BTreeException
    {
    public:
        explicit InvalidKeyLength(const std::string& msg)
            : BTreeException(msg.c_str())
        {
        }

        explicit InvalidKeyLength(const char *msg)
            : BTreeException(msg)
        {
        }
    };

    class IndexLocationChanged : public BTreeException
    {
    public:
        explicit IndexLocationChanged(const std::string& msg)
            : BTreeException(msg.c_str())
        {
        }

        explicit IndexLocationChanged(const char *msg)
            : BTreeException(msg)
        {
        }
    };

    class IllegalState : public BTreeException
    {
    public:
        explicit IllegalState(const std::string& msg)
            : BTreeException(msg.c_str())
        {
        }

        explicit IllegalState(const char *msg)
            : BTreeException(msg)
        {
        }
    };
}

#endif

