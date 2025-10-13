#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>

class Exception {
public:
    /* CONSTRUCTORS */

    Exception ( std::string error_msg );
    Exception ();

    /* GETTER */
    std::string getErrorMessage ();

private:
    std::string error_msg;
};


class OutsideOfTileException : public Exception {
public:
    /* CONSTRUCTORS */

    OutsideOfTileException ( std::string error_msg );
    OutsideOfTileException ();

    /* GETTER */
    std::string getErrorMessage ();

private:
    std::string error_msg;
};

#endif
