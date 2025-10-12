#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#define DATA_DIR "data"

/*
Download the file from the given URL and save it in the
given directory

Args:
 - url : URL as a char array
*/
int downloadFile ( char* url );

#endif
