
#ifndef EXCEPTION_HPP
# define EXCEPTION_HPP
#include <exception>


class IncorrectPort : public std::exception 
{
    public:
        virtual const char *what() const throw()
        {
            return ("Invalid port: needs only digits.");
        }
};


class IncorrectPortRange : public std::exception
{
    public:
        virtual const char *what() const throw()
        {
            return ("Invalid port: has to be between 49152 and 65535.");
        }
};

#endif