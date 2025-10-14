#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include "urls.h"

#define DATA_DIR "data"

/*
Download the file from the given URL and save it in the
given directory

Args:
 - url   : URL as a char array
 - dir   : Directory path where the file should be saved
 - force : Force download although file already exists

Returns:
 - Status code
    - FILE_ALREADY_EXISTS
    - FILE_NOT_CREATABLE
    - INVALID_URL
    - DOWNLOAD_SUCCESSFUL
*/
int downloadFile ( char* url, char* dir, bool force = false );

#endif
