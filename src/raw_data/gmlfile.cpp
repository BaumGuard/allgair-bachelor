#include <fstream>

#include "gmlfile.h"

#include "surface.h"
#include "../geometry/polygon.h"
#include "../utils.h"
#include "../status_codes.h"

#include <iostream>

/*---------------------------------------------------------------*/

/*
Return the string between the opening and the closing XML tag

Args:
 - line : String with a single line of the XML file with only
          one opening and one closing tag
*/
std::string getTextBetweenXmlTags( std::string line ) {
    std::string text = "";

    int i=0;

    int len = line.length();
    for ( ; i<len; i++ ) {
        if ( line[i] == '>' ) {
            break;
        }
    }

    i++;

    for ( ; i<len; i++ ) {
        if ( line[i] == '<' ) {
            break;
        }
        text += line[i];
    }

    return text;
} /* getTextBetweenXmlTags() */

/*---------------------------------------------------------------*/

/*
Return the string of an XML attribute value

Args:
 - line      : String with a single line of the XML file
 - attribute : Attribute of which the value should be returned
*/
std::string getXmlAttribute( std::string line, std::string attribute ) {
    unsigned long start = line.find( attribute );

    if ( start == std::string::npos ) {
        return "";
    }

    start += attribute.length() + 2;

    std::string attr_value;

    int len = line.length();
    for ( int i=start; i<len; i++ ) {
        if ( line[i] == '"' ) {
            break;
        }
        attr_value += line[i];
    }

    return attr_value;
} /* getXmlAttribute() */

/*---------------------------------------------------------------*/

/*
Return the next line from the XML file that starts with the given XML tag

Args:
 - file    : Reference to the file object
 - xml_tag : Tag in the beginning of the line
*/
std::string getNextLineWithXmlTag( std::ifstream& file, std::string xml_tag ) {
    std::string line = "";

    if ( xml_tag.back() == '>' ) {
        xml_tag.pop_back();
    }

    while ( getline(file, line) ) {
        trimString( line );

        if ( line.starts_with(xml_tag) ) {
            return line;
        }
    }

    return line;
} /* getNextLineWithXmlTag() */

/*---------------------------------------------------------------*/

/*
Convert a sequence of double values seperated by spaces to an
std::vector of Vector objects

Args:
 - value_list  : String with the double values seperated by spaces
 - vector_list : Reference to the instance of std::vector<Vector>
                 to store the extracted vectors in
*/
int valueListToVectorList (
    std::string value_list, std::vector<Vector>& vector_list
) {
    std::vector<double> values;

    std::string current_value;

    int len = value_list.length();
    for ( int i=0; i<len; i++ ) {
        if ( value_list[i] != ' ' ) {
            current_value += value_list [i];
        }
        else {
            values.push_back( stod(current_value) );
            current_value = "";
        }
    }
    if ( current_value != "" ) {
        values.push_back( stod(current_value) );
    }

    if ( values.size() % 3 != 0 ) {
        return VECTOR_INCOMPLETE;
    }

    Vector current_vector;

    len = values.size();
    for ( int i=0; i<len; i+=3 ) {
        current_vector = Vector(
            values[i],
            values[i+1],
            values[i+2]
        );

        vector_list.push_back( current_vector );
    }

    return SUCCESS;
} /* valueListToVectorList() */

/*---------------------------------------------------------------*/


int GmlFile::readGmlFile ( std::string file_path ) {
    std::ifstream gmlfile ( file_path );
    if ( !gmlfile.is_open() ) {
        return FILE_NOT_FOUND;
    }

    std::string
        line,
        line_content;

    int status;

    std::vector<Vector>
        lower_corner_list,
        upper_corner_list;

    line = getNextLineWithXmlTag( gmlfile, "<gml:lowerCorner>" );
    line_content = getTextBetweenXmlTags( line );

    status = valueListToVectorList( line_content, lower_corner_list );
    if ( status != SUCCESS ) {
        return FILE_CORRUPT;
    }
    lower_corner = lower_corner_list[0];

    line = getNextLineWithXmlTag( gmlfile, "<gml:upperCorner>" );
    line_content = getTextBetweenXmlTags( line );

    status = valueListToVectorList( line_content, upper_corner_list );
    if ( status != SUCCESS ) {
        return FILE_CORRUPT;
    }
    upper_corner = upper_corner_list[0];


    while ( true ) {

        Surface surface;

        getNextLineWithXmlTag( gmlfile, "<bldg:boundedBy>" );
        line = getNextLineWithXmlTag( gmlfile, "<bldg:" );
        if ( line == "" ) {
            break;
        }


        if ( line.contains("Ground") ) {
            surface.surface_type = GROUND;
        }
        else if ( line.contains("Roof") ) {
            surface.surface_type = ROOF;
        }
        else if ( line.contains("Wall") ) {
            surface.surface_type = WALL;
        }
        else {
            surface.surface_type = OTHER;
        }


        if ( surface.surface_type == ROOF ) {
            getNextLineWithXmlTag( gmlfile, "<gen:stringAttribute name=\"Dachneigung\">" );
            line = getNextLineWithXmlTag( gmlfile, "<gen:value>" );
            line_content = getTextBetweenXmlTags( line );
            surface.roof_tilt = stod( line_content );

            getNextLineWithXmlTag( gmlfile, "<gen:stringAttribute name=\"Dachorientierung\">" );
            line = getNextLineWithXmlTag( gmlfile, "<gen:value>" );
            line_content = getTextBetweenXmlTags( line );
            surface.roof_orientation = stod( line_content );
        }

        getNextLineWithXmlTag( gmlfile, "<gen:stringAttribute name=\"Flaeche\">" );
        line = getNextLineWithXmlTag( gmlfile, "<gen:value>" );
        line_content = getTextBetweenXmlTags( line );
        surface.area = stod( line_content );


        getNextLineWithXmlTag( gmlfile, "<gen:stringAttribute name=\"Z_MAX\">" );
        line = getNextLineWithXmlTag( gmlfile, "<gen:value>" );
        line_content = getTextBetweenXmlTags( line );
        surface.z_max = stod( line_content );

        getNextLineWithXmlTag( gmlfile, "<gen:stringAttribute name=\"Z_MAX_ASL\">" );
        line = getNextLineWithXmlTag( gmlfile, "<gen:value>" );
        line_content = getTextBetweenXmlTags( line );
        surface.z_max_asl = stod( line_content );

        getNextLineWithXmlTag( gmlfile, "<gen:stringAttribute name=\"Z_MIN\">" );
        line = getNextLineWithXmlTag( gmlfile, "<gen:value>" );
        line_content = getTextBetweenXmlTags( line );
        surface.z_min = stod( line_content );

        getNextLineWithXmlTag( gmlfile, "<gen:stringAttribute name=\"Z_MIN_ASL\">" );
        line = getNextLineWithXmlTag( gmlfile, "<gen:value>" );
        line_content = getTextBetweenXmlTags( line );
        surface.z_min_asl = stod( line_content );

        line = getNextLineWithXmlTag( gmlfile, "<gml:Polygon" );
        line_content = getXmlAttribute( line, "gml:id" );
        surface.id = line_content;

        line = getNextLineWithXmlTag( gmlfile, "<gml:posList>" );
        line_content = getTextBetweenXmlTags( line );
        valueListToVectorList( line_content, surface.pos_list );
        surface.pos_list.pop_back();


        surfaces.push_back( surface );
    }

    if ( surfaces.size() == 0 ) {
        return NO_SURFACES;
    }
    return SUCCESS;

} /* readGmlFile() */

/*---------------------------------------------------------------*/

std::vector<Surface>& GmlFile::getSurfaces () {
    return surfaces;
} /* getSurfaces() */

/*---------------------------------------------------------------*/

Vector GmlFile::getLowerCorner () const {
    return lower_corner;
} /* getLowerCorner() */

/*---------------------------------------------------------------*/

Vector GmlFile::getUpperCorner () const {
    return upper_corner;
} /* getUpperCorner() */
