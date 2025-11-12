#include "config.h"

#include "../utils.h"
#include "../status_codes.h"
#include "datatype_check.h"

#include <stdlib.h>

#define MAX_LINE_SIZE 256


/*---------------------------------------------------------------*/

int Config::readConfigFile ( std::string file_path ) {
    char line_buf [MAX_LINE_SIZE];

    FILE* config_file = fopen( file_path.data(), "r" );
    if ( !config_file ) {
        return CONFIG_FILE_NOT_FOUND;
    }
    config_file_path = file_path;

    FILE* types_file = fopen( "src/config/config_datatypes.txt", "r" );
    if ( !types_file ) {
        return CONFIG_TYPES_FILE_NOT_FOUND;
    }

    while ( fgets(line_buf, MAX_LINE_SIZE, types_file) ) {
        std::string* strings = splitString( line_buf, ' ' );

        if ( strings[1] == "S_INT" ) {
            config_datatypes[strings[0]] = S_INT;
        }
        else if ( strings[1] == "U_INT" ) {
            config_datatypes[strings[0]] = U_INT;
        }
        else if ( strings[1] == "FLOAT" ) {
            config_datatypes[strings[0]] = FLOAT;
        }
        else if ( strings[1] == "BOOL" ) {
            config_datatypes[strings[0]] = BOOL;
        }
        else if ( strings[1] == "STRING" ) {
            config_datatypes[strings[0]] = STRING;
        }
        else {
            return UNKNOWN_CONFIG_DATATYPE;
        }
    }

    while ( fgets(line_buf, MAX_LINE_SIZE, config_file) ) {
        if ( line_buf[0] == '#' || line_buf[0] == ' ' || line_buf[0] == '\n' ) {
            continue;
        }

        std::string* strings = splitString( line_buf, '=' );

        if ( !config_datatypes.contains(strings[0]) ) {
            return UNKNOWN_CONFIG_OPTION;
        }

        int datatype = config_datatypes[strings[0]];

        switch ( datatype ) {
            case S_INT:
                if ( stringIsSint(strings[1]) ) {
                    config_objects[strings[0]].sint_value = std::stoi( strings[1] );
                    config_objects[strings[0]].datatype = S_INT;
                }
                else
                    return INVALID_TYPE_FOR_OPTION;
                break;

            case U_INT:
                if ( stringIsUint(strings[1]) ) {
                    config_objects[strings[0]].uint_value = (uint) std::stoul( strings[1] );
                    config_objects[strings[0]].datatype = U_INT;
                }
                else
                    return INVALID_TYPE_FOR_OPTION;
                break;

            case FLOAT:
                if ( stringIsFloat(strings[1]) ) {
                    config_objects[strings[0]].float_value = std::stof( strings[1] );
                    config_objects[strings[0]].datatype = FLOAT;
                }
                else
                    return INVALID_TYPE_FOR_OPTION;
                break;

            case BOOL:
                if ( strings[1] == "true" )
                    config_objects[strings[0]].bool_value = true;
                else if ( strings[1] == "false" )
                    config_objects[strings[0]].bool_value = false;
                else if ( strings[1] == "0" )
                    config_objects[strings[0]].bool_value = false;
                else if ( strings[1] == "1" )
                    config_objects[strings[0]].bool_value = true;
                else
                    return INVALID_TYPE_FOR_OPTION;

                config_objects[strings[0]].datatype = BOOL;

                break;

            case STRING:
                config_objects[strings[0]].string_value = strings[1];
                config_objects[strings[0]].datatype = STRING;
                break;
        }
    }

    fclose( config_file );
    fclose( types_file );

    config_loaded = true;
    return CONFIG_PARSED_SUCCESS;
} /* readConfigFile () */

/*---------------------------------------------------------------*/

int Config::saveConfigOption ( std::string opt_name, ConfigObject& value ) {
    if ( !config_loaded ) {
        return CONFIG_FILE_NOT_LOADED_YET;
    }

    FILE* config_file = fopen( config_file_path.data(), "rw" );
    if ( !config_file ) {
        fclose( config_file );
        return CONFIG_FILE_NOT_FOUND;
    }

    char line_buf [256];
    bool line_found = false;
    int line_number = 0;
    while ( fgets(line_buf, MAX_LINE_SIZE, config_file) ) {
        std::string line ( line_buf );

        if ( line.starts_with(opt_name) ) {
            line_found = true;
            break;
        }

        line_number++;
    }

    fclose( config_file );

    if ( !config_datatypes.contains(opt_name) ) {
        return UNKNOWN_CONFIG_OPTION;
    }
    printMessage( NORMAL, "%d\n", value.datatype );
    if ( config_datatypes[opt_name] != value.datatype ) {
        return INVALID_TYPE_FOR_OPTION;
    }

    char config_line [256];
    switch( value.datatype ) {
        case S_INT:
            snprintf(
                config_line, 256,
                "%s = %d",
                opt_name.data(), value.sint_value
            );
            break;
        case U_INT:
            snprintf(
                config_line, 256,
                "%s = %d",
                opt_name.data(), value.uint_value
            );
            break;
        case FLOAT:
            snprintf(
                config_line, 256,
                "%s = %f",
                opt_name.data(), value.float_value
            );
            break;
        case BOOL:
            snprintf(
                config_line, 256,
                "%s = %s",
                opt_name.data(), value.bool_value ? "true" : "false"
            );
            break;
        case STRING:
            snprintf(
                config_line, 256,
                "%s = %s",
                opt_name.data(), value.string_value.data()
            );
            break;
    }

    int system_status;
    if ( line_found ) {
        char sed_command [256];
        line_number++;
        snprintf(
            sed_command, 256,
            "sed -i '%ds/.*/%s/' %s",
            line_number, config_line, config_file_path.data()
        );

        system_status = system( sed_command );
    }
    else {
        char append_command [256];
        snprintf(
            append_command, 256,
            "echo '%s' >> %s",
            config_line, config_file_path.data()
        );

        system_status = system( append_command );
    }


    if ( system_status == 0 ) {
        return CONFIG_WRITE_SUCCESS;
    }
    return CONFIG_WRITE_FAILURE;
} /* saveConfigOption () */

/*---------------------------------------------------------------*/

ConfigObject& Config::getOptionValue ( std::string option ) {
    return config_objects[option];
} /* getOptionValue () */
