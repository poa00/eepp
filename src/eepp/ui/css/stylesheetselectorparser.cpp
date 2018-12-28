#include <eepp/ui/css/stylesheetselectorparser.hpp>

namespace EE { namespace UI { namespace CSS {

StyleSheetSelectorParser::StyleSheetSelectorParser(){}

StyleSheetSelectorParser::StyleSheetSelectorParser( std::string name )
{
	std::vector<std::string> sels = String::split( name, ',' );

	for ( std::vector<std::string>::iterator it = sels.begin(); it != sels.end(); ++it )
	{
		std::string cur = String::trim( *it );

		selectors.push_back( StyleSheetSelector( cur ) );
	}
}

}}}
