#include "download.h"

#include <curl/curl.h>
#include <stdio.h>
#include <string.h>

#include "../status_codes.h"
#include "../utils.h"


/*---------------------------------------------------------------*/

size_t saveFile ( char* buf, size_t itemsize, size_t n_items, void* arg ) {
    size_t err = fwrite( buf, itemsize, n_items, (FILE*)arg );
    return err;
}

/*---------------------------------------------------------------*/

int downloadFile ( char* url ) {
    char data_dir [256] = DATA_DIR;

    char file_name [32];
    char out_path [256];

    extractFilepath( file_name, url );
    buildFilepath( out_path, data_dir, file_name );

    FILE* out_file = fopen( out_path, "wb" );
    if ( !out_file ) {
        return FILE_NOT_CREATABLE;
    }

    CURL* curl = curl_easy_init();

    curl_easy_setopt( curl, CURLOPT_URL, url );
    curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, saveFile );
    curl_easy_setopt( curl, CURLOPT_WRITEDATA, out_file );

    CURLcode err = curl_easy_perform( curl );

    if ( err != CURLE_OK ) {
        fprintf( stderr, "ERROR: Invalid URL '%s'\n", url );
        return INVALID_URL;
    }


    curl_easy_cleanup( curl );

    return DOWNLOAD_SUCCESSFUL;
}
