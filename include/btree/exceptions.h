#ifndef _EXCEPTIONS_H_
#define _EXCEPTIONS_H_

#include <stdexcept>

namespace btree
{
    class BTreeException : public std::exception
    {
    public:
        explicit BTreeException(const std::string& msg)
            : std::exception(msg.c_str())
        {
        }

        explicit BTreeException(const char *msg)
            : std::exception(msg)
        {
        }
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
}

#endif

