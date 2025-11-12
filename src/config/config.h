#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <unordered_map>

/*---------------------------------------------------------------*/

// Available datatypes to use for values in the config file
enum ConfigDatatypes {
    S_INT,
    U_INT,
    FLOAT,
    BOOL,
    STRING
};

/*---------------------------------------------------------------*/

// Object to save a config value in
// When setting a value for the given value type, you should
// always set the variable 'datatype' to the corresponding
// datatype (see enum ConfigDatatypes)
typedef struct {
    int datatype;

    int         sint_value;
    uint        uint_value;
    float       float_value;
    bool        bool_value;
    std::string string_value;
} ConfigObject;

/*---------------------------------------------------------------*/

/*
Class for reading, parsing and writing from and to config files
and make the content of the config file accessible to the program.

The configuration line is a key/value pair:
 KEY = VALUE
where KEY is always a string and VALUE can have one of the datatypes
defined in the enum ConfigDatatypes
*/
class Config {
public:
    /*
    Read and parse a config file with the options and their
    corresponding datatypes defined in 'src/config/config_datatypes.h'

    Args:
     - file_path : Path to the config file to be read and parsed

    Returns:
     - Status code
        - CONFIG_FILE_NOT_FOUND
        - CONFIG_TYPES_FILE_NOT_FOUND
        - UNKNOWN_CONFIG_DATATYPE
        - UNKNOWN_CONFIG_OPTION
        - INVALID_TYPE_FOR_OPTION
        - CONFIG_PARSED_SUCCESS (Success)
    */
    int readConfigFile ( std::string file_path );

    /*
    Save a config option in the file
    The given config file has to be loaded first using readConfigFile()
    If the option cannot be found in config_datatypes.txt or if the
    datatype of the value does not match the one from config_datatypes.txt
    the function will be aborted

    Args:
     - opt_name : Name of the config option
     - value    : Reference to the ConfigObject that holds the value

    Returns:
     - Status code:
        - CONFIG_FILE_NOT_FOUND
        - UNKNOWN_CONFIG_OPTION
        - INVALID_TYPE_FOR_OPTION
        - CONFIG_WRITE_FAILURE
        - CONFIG_WRITE_SUCCESS (Success)
    */
    int saveConfigOption ( std::string opt_name, ConfigObject& value );

    /*
    Return a reference to value of the corresponding option

    Args:
     - option : Name of the config option

    Returns:
     - Reference to the corresponding value object of the given option
    */
    ConfigObject& getOptionValue ( std::string option );

private:
    std::unordered_map<std::string, int> config_datatypes;
    std::unordered_map<std::string, ConfigObject> config_objects;
    std::string config_file_path;

    bool config_loaded = false;
};

#endif
