#include "download.h"

#include "../status_codes.h"
#include "../utils.h"

#include <curl/curl.h>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>


void createWorkspace ( std::string data_dir ) {
    struct stat st = {0};

    std::string dgm1_dir  = data_dir + "/DGM1";
    std::string dom20_dir = data_dir + "/DOM20";
    std::string lod2_dir  = data_dir + "/LOD2";

    if ( stat(data_dir.data(), &st) == -1 ) {
        mkdir( data_dir.data(), 0700 );
    }

    if ( stat(dgm1_dir.data(), &st) == -1 ) {
        mkdir( dgm1_dir.data(), 0700 );
    }
    if ( stat(dom20_dir.data(), &st) == -1 ) {
        mkdir( dom20_dir.data(), 0700 );
    }
    if ( stat(lod2_dir.data(), &st) == -1 ) {
        mkdir( lod2_dir.data(), 0700 );
    }
}

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
