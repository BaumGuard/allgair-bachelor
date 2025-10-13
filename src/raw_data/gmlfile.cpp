#include <fstream>

#include "gmlfile.h"
#include "surface.h"
#include "../status_codes.h"

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
} /* std::string getTextBetweenXmlTags( std::string line ) */

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

    start += attribute.length() + 3;

    std::string attr_value;

    int len = line.length();
    for ( int i=start; i<len; i++ ) {
        if ( line[i] == '"' ) {
            break;
        }
        attr_value += line[i];
    }

    return attr_value;
} /* std::string getXmlAttribute( std::string line, std::string attribute ) */

/*---------------------------------------------------------------*/

/*
Return the next line from the XML file that starts with the given XML tag

Args:
 - file    : Reference to the file object
 - xml_tag : Tag in the beginning of the line
*/
std::string getNextLineWithXmlTag( std::ifstream& file, std::string xml_tag ) {
    std::string line = "";

    while ( getline(file, line) ) {
        if ( line.starts_with(xml_tag) ) {
            return line;
        }
    }

    return line;
} /* std::string getNextLineWithXmlTag( std::ifstream& file, std::string xml_tag ) */

/*---------------------------------------------------------------*/

/*
Convert a sequence of float values seperated by spaces to an
std::vector of Vector objects

Args:
 - value_list : String with the float values seperated by spaces
*/
std::vector<Vector> valueListToVectorList ( std::string value_list ) {
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
    values.push_back( stod(current_value) );

    if ( values.size() % 3 != 0 ) {
        // TODO: Fehler
    }

    std::vector<Vector> vector_list;
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

    return vector_list;
} /* std::vector<Vector> valueListToVectorList ( std::string value_list ) */

/*---------------------------------------------------------------*/


GmlFile::GmlFile ( std::string file_path ) {
    std::ifstream gmlfile ( file_path );
    if ( !gmlfile.is_open() ) {
        std::cerr << "ERROR: Cannot open the GML file '" << file_path << "'\n";
        return;
    }

    std::string
        line,
        line_content;

    line = getNextLineWithXmlTag( gmlfile, "<gml:lowerCorner>" );
    line_content = getTextBetweenXmlTags( line );

    lower_corner = valueListToVectorList( line_content )[0];

    line = getNextLineWithXmlTag( gmlfile, "<gml:upperCorner>" );
    line_content = getTextBetweenXmlTags( line );

    upper_corner = valueListToVectorList( line_content )[0];


    Surface surface;


    while ( true ) {
        /*
        getNextLineWithXmlTag( "<gen:stringAttribute name=\"DatenquelleBodenhoehe\">" );
        line = getNextLineWithXmlTag( "<gen:value>" );
        line_content = getTextBetweenXmlTags( line );
        DataSrcGroundLevel = stoi( line_content );

        getNextLineWithXmlTag( "<gen:stringAttribute name=\"DatenquelleDachhoehe\">" );
        line = getNextLineWithXmlTag( "<gen:value>" );
        line_content = getTextBetweenXmlTags( line );
        DataSrcRoofLevel = stoi( line_content );

        getNextLineWithXmlTag( "<gen:stringAttribute name=\"DatenquelleLage\">" );
        line = getNextLineWithXmlTag( "<gen:value>" );
        line_content = getTextBetweenXmlTags( line );
        DataSrcPosition = stoi( line_content );

        getNextLineWithXmlTag( "<gen:stringAttribute name=\"Gemeindeschluessel\">" );
        line = getNextLineWithXmlTag( "<gen:value>" );
        line_content = getTextBetweenXmlTags( line );
        MunicipalityCode = stoi( line_content );

        getNextLineWithXmlTag( "<gen:stringAttribute name=\"HoeheDach\">" );
        line = getNextLineWithXmlTag( "<gen:value>" );
        line_content = getTextBetweenXmlTags( line );
        roofHeight = stod( line_content );

        getNextLineWithXmlTag( "<gen:stringAttribute name=\"HoeheGrund\">" );
        line = getNextLineWithXmlTag( "<gen:value>" );
        line_content = getTextBetweenXmlTags( line );
        groundHeight = stod( line_content );

        getNextLineWithXmlTag( "<gen:stringAttribute name=\"Methode\">" );
        line = getNextLineWithXmlTag( "<gen:value>" );
        line_content = getTextBetweenXmlTags( line );
        method = stoi( line_content );

        getNextLineWithXmlTag( "<gen:stringAttribute name=\"NiedrigsteTraufeDesGebaeudes\">" );
        line = getNextLineWithXmlTag( "<gen:value>" );
        line_content = getTextBetweenXmlTags( line );
        lowestBuildingEaves = stod( line_content );

        getNextLineWithXmlTag( "<gen:stringAttribute name=\"Geometrietyp2DReferenz\">" );
        line = getNextLineWithXmlTag( "<gen:value>" );
        line_content = getTextBetweenXmlTags( line );
        geo2dReference = stoi( line_content );

        line = getNextLineWithXmlTag( "<bldg:roofType>" );
        line_content = getTextBetweenXmlTags( line );
        roofType = stoi( line_content );

        line = getNextLineWithXmlTag( "<bldg:measuredHeight>" );
        line_content = getTextBetweenXmlTags( line );
        measuredHeight = stod( line_content );
        */


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

        surface.id = getXmlAttribute( line, "gml:id" );


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


        line = getNextLineWithXmlTag( gmlfile, "<gml:posList>" );
        line_content = getTextBetweenXmlTags( line );
        surface.pos_list = valueListToVectorList( line_content );
        surface.pos_list.pop_back();


        surfaces.push_back( surface );
    }

    /*
    int len = surfaces.size();
    for ( int i=0; i<len; i++ ) {
        printSurfaceDescription( &surfaces[i] );
    }
    */

} /* GmlFile::GmlFile ( std::string file_path ) */

/*---------------------------------------------------------------*/

std::vector<Surface>& GmlFile::getSurfaces () {
    return surfaces;
} /* std::vector<Surface>& GmlFile::getSurfaces () */

/*---------------------------------------------------------------*/

Vector GmlFile::getLowerCorner () {
    return lower_corner;
} /* Vector GmlFile::getLowerCorner () */

/*---------------------------------------------------------------*/

Vector GmlFile::getUpperCorner () {
    return upper_corner;
} /* Vector GmlFile::getUpperCorner () */
