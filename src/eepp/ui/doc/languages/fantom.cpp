#include <eepp/ui/doc/languages/fantom.hpp>
#include <eepp/ui/doc/syntaxdefinitionmanager.hpp>

namespace EE { namespace UI { namespace Doc { namespace Language {

void addFantom() {

	auto& sd = SyntaxDefinitionManager::instance()->add(

		{ "Fantom",
		  { "%.fan$", "%.fanx$" },
		  {
			  { { "//.-\n" }, "comment" },
			  { { "/%*", "%*/" }, "comment" },
			  { { "\"", "\"", "\\" }, "string" },
			  { { "'", "'", "\\" }, "string" },
			  { { "`", "`", "\\" }, "string" },
			  { { "'\\x%x?%x?%x?%x'" }, "string" },
			  { { "'\\u%x%x%x%x'" }, "string" },
			  { { "'\\?.'" }, "string" },
			  { { "-?0x%x+" }, "number" },
			  { { "-?%d+[%d%.eE]*f?" }, "number" },
			  { { "-?%.?%d+f?" }, "number" },
			  { { "[%+%-=/%*%^%%<>!~|&]" }, "operator" },
			  { { "[%a_][%w_]*%f[(]" }, "function" },
			  { { "[%a_][%w_]*" }, "symbol" },

		  },
		  {
			  { "using", "keyword" },
			  { "native", "keyword" },
			  { "goto", "keyword" },
			  { "void", "keyword" },
			  { "serializable", "keyword" },
			  { "volatile", "keyword" },
			  { "if", "keyword" },
			  { "else", "keyword" },
			  { "switch", "keyword" },
			  { "do", "keyword" },
			  { "while", "keyword" },
			  { "for", "keyword" },
			  { "foreach", "keyword" },
			  { "each", "keyword" },
			  { "true", "literal" },
			  { "false", "literal" },
			  { "null", "literal" },
			  { "this", "keyword" },
			  { "super", "keyword" },
			  { "new", "keyword" },
			  { "is", "keyword" },
			  { "isnot", "keyword" },
			  { "as", "keyword" },
			  { "plus", "keyword" },
			  { "minus", "keyword" },
			  { "mult", "keyword" },
			  { "div", "keyword" },
			  { "mod", "keyword" },
			  { "get", "keyword" },
			  { "set", "keyword" },
			  { "slice", "keyword" },
			  { "lshift", "keyword" },
			  { "rshift", "keyword" },
			  { "and", "keyword" },
			  { "or", "keyword" },
			  { "xor", "keyword" },
			  { "inverse", "keyword" },
			  { "negate", "keyword" },
			  { "increment", "keyword" },
			  { "decrement", "keyword" },
			  { "equals", "keyword" },
			  { "compare", "keyword" },
			  { "return", "keyword" },
			  { "static", "keyword" },
			  { "const", "keyword" },
			  { "final", "keyword" },
			  { "virtual", "keyword" },
			  { "override", "keyword" },
			  { "once", "keyword" },
			  { "readonly", "keyword" },
			  { "throw", "keyword" },
			  { "try", "keyword" },
			  { "catch", "keyword" },
			  { "finally", "keyword" },
			  { "assert", "keyword" },
			  { "class", "keyword" },
			  { "enum", "keyword" },
			  { "mixin", "keyword" },
			  { "break", "keyword" },
			  { "continue", "keyword" },
			  { "default", "keyword" },
			  { "case", "keyword" },
			  { "public", "keyword" },
			  { "internal", "keyword" },
			  { "protected", "keyword" },
			  { "private", "keyword" },
			  { "abstract", "keyword" },
			  { "Void", "keyword2" },
			  { "Bool", "keyword2" },
			  { "Int", "keyword2" },
			  { "Float", "keyword2" },
			  { "Decimal", "keyword2" },
			  { "Str", "keyword2" },
			  { "Duration", "keyword2" },
			  { "Uri", "keyword2" },
			  { "Type", "keyword2" },
			  { "Range", "keyword2" },
			  { "List", "keyword2" },
			  { "Map", "keyword2" },
			  { "Obj", "keyword2" },
			  { "Err", "keyword2" },
			  { "Env", "keyword2" },
		  },
		  "//",
		  {}

		} );

	sd.setFoldRangeType( FoldRangeType::Braces ).setFoldBraces( { { '{', '}' } } );
}

}}}} // namespace EE::UI::Doc::Language
