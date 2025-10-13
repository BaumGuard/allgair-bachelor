#include "exception.h"

std::string Exception::getErrorMessage () {
    return error_msg;
}

Exception::Exception ( std::string error_msg ):
    error_msg( error_msg )
{}

Exception::Exception () {}


std::string OutsideOfTileException::getErrorMessage () {
    return error_msg;
}

OutsideOfTileException::OutsideOfTileException ( std::string error_msg ):
    error_msg( error_msg )
{}

OutsideOfTileException::OutsideOfTileException () {}
