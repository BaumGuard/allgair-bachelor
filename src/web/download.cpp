#include "download.h"

#include <curl/curl.h>
#include <cstdio>
#include <unistd.h>

#include "../status_codes.h"
#include "../utils.h"


/*---------------------------------------------------------------*/

size_t saveFile ( char* buf, size_t itemsize, size_t n_items, void* arg ) {
    size_t err = fwrite( buf, itemsize, n_items, (FILE*)arg );
    return err;
} /* saveFile() */

/*---------------------------------------------------------------*/

int downloadFile ( const char* url, const char* dir, bool force ) {
    char file_name [32];
    char out_path [256];

    extractFilepath( file_name, (char*)url );
    buildFilepath( out_path, (char*)dir, file_name );

    if ( access(out_path, F_OK) == 0 && !force ) {
        return FILE_ALREADY_EXISTS;
    }

    FILE* out_file = fopen( out_path, "wb" );
    if ( !out_file ) {
        return FILE_NOT_CREATABLE;
    }

    CURL* curl = curl_easy_init();

    curl_easy_setopt( curl, CURLOPT_URL, url );
    curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, saveFile );
    curl_easy_setopt( curl, CURLOPT_WRITEDATA, out_file );


    CURLcode err = curl_easy_perform( curl );

    long http_code = 0;
    curl_easy_getinfo ( curl, CURLINFO_RESPONSE_CODE, &http_code );

    if ( http_code != 200 || err != CURLE_OK ) {
        printMessage( ERROR, "ERROR: Could not download file from '%s'\n", url );
        return FILE_NOT_FOUND;
    }

    curl_easy_cleanup( curl );

    fclose(out_file);

    return DOWNLOAD_SUCCESSFUL;
} /* downloadFile() */
