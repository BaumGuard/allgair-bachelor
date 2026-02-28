#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <string>

/*
Download the file from the given URL and save it in the
given directory

Args:
 - url   : URL of the file to be downloaded
 - dir   : Directory path where the file should be saved
 - force : Force download although file already exists

Returns:
 - Status code
    - SUCCESS

    - FILE_ALREADY_EXISTS
    - FILE_NOT_CREATABLE
    - INVALID_URL
*/
int downloadFile ( std::string url, std::string dir, bool force = false );

#endif
