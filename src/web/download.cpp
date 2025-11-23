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

int downloadFile ( std::string url, std::string dir, bool force ) {
    std::string file_name;
    std::string out_path;

    file_name = extractFilename( url );
    out_path = buildFilepath( dir, file_name );

    if ( access(out_path.data(), F_OK) == 0 && !force ) {
        return FILE_ALREADY_EXISTS;
    }

    FILE* out_file = fopen( out_path.data(), "wb" );
    if ( !out_file ) {
        return FILE_NOT_CREATABLE;
    }

    CURL* curl = curl_easy_init();

    curl_easy_setopt( curl, CURLOPT_URL, url.data() );
    curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, saveFile );
    curl_easy_setopt( curl, CURLOPT_WRITEDATA, out_file );


    CURLcode err = curl_easy_perform( curl );

    long http_code = 0;
    curl_easy_getinfo ( curl, CURLINFO_RESPONSE_CODE, &http_code );

    if ( http_code != 200 || err != CURLE_OK ) {
        printMessage( ERROR, "ERROR: Could not download file from '%s'\n", url.data() );
        std::remove( out_path.data() );
        return FILE_NOT_FOUND;
    }

    curl_easy_cleanup( curl );

    fclose(out_file);

    return SUCCESS;
} /* downloadFile() */
