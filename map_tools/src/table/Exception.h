/** @file Exception.h
    @brief exception classes for Table

*/

#ifndef TABLE_EXCEPTION_H
#define TABLE_EXCEPTION_H

#include <exception>
#include <iostream>
#include <string>



namespace table
{
    class Exception : public std::exception 
    {
    public:
        Exception() {}
        Exception(std::string errorString, int code=0) 
            : m_what(errorString), m_code(code) 
        {}

        virtual ~Exception() throw() {}
        virtual const char *what() const throw() {return m_what.c_str();}
        virtual const int code() const {return m_code;}
    protected:
        std::string m_what;
        int m_code;
    };



    class InvalidKey : public std::exception
    {
    public:
        InvalidKey()   {}
        InvalidKey(const std::string &msg, int code=0)
            : m_what(msg + "Invalid key.\n"), m_code(code)
        {}

        virtual ~InvalidKey() throw() {}
        virtual const char *what() const throw() {return m_what.c_str();}
        virtual const int code() const {return m_code;}
    protected:
        std::string m_what;
        int m_code;
    };



    class OutOfRange : public std::exception
    {
    public:
        OutOfRange()   {}
        OutOfRange(const std::string &msg, int code=0)
            : m_what(msg), m_code(code)
        {}

        virtual ~OutOfRange() throw() {}
        virtual const char *what() const throw() {return m_what.c_str();}
        virtual const int code() const {return m_code;}
    protected:
        std::string m_what;
        int m_code;
    };


}   // namespace Goodi

#endif

