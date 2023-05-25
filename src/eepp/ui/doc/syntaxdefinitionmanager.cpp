#include <algorithm>
#include <eepp/system/filesystem.hpp>
#include <eepp/system/iostreammemory.hpp>
#include <eepp/system/luapattern.hpp>
#include <eepp/system/packmanager.hpp>
#include <eepp/ui/doc/languages/hlsl.hpp>
#include <eepp/ui/doc/languages/htaccess.hpp>
#include <eepp/ui/doc/languages/objeck.hpp>
#include <eepp/ui/doc/languages/pascal.hpp>
#include <eepp/ui/doc/languages/pico-8.hpp>
#include <eepp/ui/doc/languages/postgresql.hpp>
#include <eepp/ui/doc/languages/vb.hpp>
#include <eepp/ui/doc/languages/x86assembly.hpp>
#include <eepp/ui/doc/syntaxdefinitionmanager.hpp>
#include <eepp/ui/uiwidgetcreator.hpp>
#include <nlohmann/json.hpp>

using namespace EE::System;
using namespace EE::UI::Doc::Language;

using json = nlohmann::json;

namespace EE { namespace UI { namespace Doc {

SINGLETON_DECLARE_IMPLEMENTATION( SyntaxDefinitionManager )

static void addPlainText() {
	SyntaxDefinitionManager::instance()->add(
		{ "Plain Text", { "%.txt$" }, {}, {}, "", {}, "plaintext" } );
}

static void addXML() {
	SyntaxDefinitionManager::instance()
		->add( { "XML",
				 { "%.xml$", "%.svg$" },
				 {
					 { { "<%s*[sS][tT][yY][lL][eE]%s*>", "<%s*/%s*[sS][tT][yY][lL][eE]%s*>" },
					   "function",
					   "CSS" },
					 { { "<!%-%-", "%-%->" }, "comment" },
					 { { "%f[^>][^<]", "%f[<]" }, "normal" },
					 { { "\"", "\"", "\\" }, "string" },
					 { { "'", "'", "\\" }, "string" },
					 { { "0x[%da-fA-F]+" }, "number" },
					 { { "-?%d+[%d%.]*f?" }, "number" },
					 { { "-?%.?%d+f?" }, "number" },
					 { { "%f[^<]![%a_][%w%_%-]*" }, "keyword2" },
					 { { "%f[^<][%a_][%w%_%-]*" }, "function" },
					 { { "%f[^<]/[%a_][%w%_%-]*" }, "function" },
					 { { "[%a_][%w_]*" }, "keyword" },
					 { { "[/<>=]" }, "operator" },
				 },
				 {},
				 "",
				 { "<%?xml" } } )
		.setAutoCloseXMLTags( true );
}

static void addHTML() {
	SyntaxDefinitionManager::instance()
		->add( { "HTML",
				 { "%.html?$", "%.phtml", "%.handlebars" },
				 {
					 { { "<%s*[sS][cC][rR][iI][pP][tT]%s+[tT][yY][pP][eE]%s*=%s*['\"]%a+/"
						 "[jJ][aA][vV][aA][sS][cC][rR][iI][pP][tT]['\"]%s*>",
						 "<%s*/[sS][cC][rR][iI][pP][tT]>" },
					   "function",
					   "JavaScript" },
					 { { "<%s*[sS][cC][rR][iI][pP][tT]%s*>", "<%s*/%s*[sS][cC][rR][iI][pP][tT]>" },
					   "function",
					   "JavaScript" },
					 { { "<%s*[sS][tT][yY][lL][eE][^>]*>", "<%s*/%s*[sS][tT][yY][lL][eE]%s*>" },
					   "function",
					   "CSS" },
					 { { "<%?p?h?p?", "%?>" }, "function", "PHPCore" },
					 { { "<!%-%-", "%-%->" }, "comment" },
					 { { "%f[^>][^<]", "%f[<]" }, "normal" },
					 { { "\"", "\"", "\\" }, "string" },
					 { { "'", "'", "\\" }, "string" },
					 { { "0x[%da-fA-F]+" }, "number" },
					 { { "-?%d+[%d%.]*f?" }, "number" },
					 { { "-?%.?%d+f?" }, "number" },
					 { { "%f[^<]![%a_][%w%_%-]*" }, "keyword2" },
					 { { "%f[^<][%a_][%w%_%-]*" }, "function" },
					 { { "%f[^<]/[%a_][%w%_%-]*" }, "function" },
					 { { "[%a_][%w_]*" }, "keyword" },
					 { { "[/<>=]" }, "operator" },
				 },
				 {},
				 "",
				 { "<html", "<![Dd][Oo][Cc][Tt][Yy][Pp][Ee]%s[Hh][Tt][Mm][Ll]>" } } )
		.setAutoCloseXMLTags( true );
}

static void addCSS() {
	SyntaxDefinitionManager::instance()
		->add( { "CSS",
				 { "%.css$" },
				 {
					 { { "\\." }, "normal" },
					 { { "//.-\n" }, "comment" },
					 { { "/%*", "%*/" }, "comment" },
					 { { "\"", "\"", "\\" }, "string" },
					 { { "'", "'", "\\" }, "string" },
					 { { "^%s*#[%a%_%-][%w%_%-]*" }, "keyword2" },
					 { { ",%s*#[%a%_%-][%w%_%-]*" }, "keyword2" },
					 { { "#%x%x?%x?%x?%x?%x?%x?%x?" }, "string" },
					 { { "#[%a%_%-][%w%_%-]*" }, "keyword2" },
					 { { "%-%-%a[%w%-%_]*" }, "keyword2" },
					 { { "-?%d+[%d%.]*p[xt]" }, "number" },
					 { { "-?%d+[%d%.]*deg" }, "number" },
					 { { "-?%d+[%d%.]*" }, "number" },
					 { { "@[%a][%w%_%-]*" }, "keyword2" },
					 { { "%.[%a%_%-][%w%_%-]*" }, "keyword2" },
					 { { "(:)(hover)" }, { "normal", "operator", "literal" } },
					 { { "(:)(focus%-within)" }, { "normal", "operator", "literal" } },
					 { { "(:)(focus)" }, { "normal", "operator", "literal" } },
					 { { "(:)(selected)" }, { "normal", "operator", "literal" } },
					 { { "(:)(pressed)" }, { "normal", "operator", "literal" } },
					 { { "(:)(disabled)" }, { "normal", "operator", "literal" } },
					 { { "(:)(checked)" }, { "normal", "operator", "literal" } },
					 { { "(:)(root)" }, { "normal", "operator", "link" } },
					 { { "(%a+)(%()" }, { "normal", "function", "normal" } },
					 { { "[%a][%w-]*%s*%f[:]" }, "keyword" },
					 { { "[{}:>~!]" }, "operator" },
					 { { "[%a_][%w_]*" }, "symbol" },
				 },
				 {
					 { "true", "literal" },
					 { "false", "literal" },
					 { "transparent", "literal" },
					 { "none", "literal" },
					 { "center", "literal" },
					 { "arrow", "literal" },
					 { "hand", "literal" },
					 { "ibeam", "literal" },
					 { "wait", "literal" },
					 { "crosshair", "literal" },
					 { "waitarrow", "literal" },
					 { "sizenwse", "literal" },
					 { "sizenesw", "literal" },
					 { "sizewe", "literal" },
					 { "sizens", "literal" },
					 { "sizeall", "literal" },
					 { "nocursor", "literal" },
					 { "important", "literal" },
					 { "aliceblue", "literal" },
					 { "antiquewhite", "literal" },
					 { "aqua", "literal" },
					 { "aquamarine", "literal" },
					 { "azure", "literal" },
					 { "beige", "literal" },
					 { "bisque", "literal" },
					 { "black", "literal" },
					 { "blanchedalmond", "literal" },
					 { "blue", "literal" },
					 { "blueviolet", "literal" },
					 { "brown", "literal" },
					 { "burlywood", "literal" },
					 { "cadetblue", "literal" },
					 { "chartreuse", "literal" },
					 { "chocolate", "literal" },
					 { "coral", "literal" },
					 { "cornflowerblue", "literal" },
					 { "cornsilk", "literal" },
					 { "crimson", "literal" },
					 { "cyan", "literal" },
					 { "darkblue", "literal" },
					 { "darkcyan", "literal" },
					 { "darkgoldenrod", "literal" },
					 { "darkgray", "literal" },
					 { "darkgreen", "literal" },
					 { "darkgrey", "literal" },
					 { "darkkhaki", "literal" },
					 { "darkmagenta", "literal" },
					 { "darkolivegreen", "literal" },
					 { "darkorange", "literal" },
					 { "darkorchid", "literal" },
					 { "darkred", "literal" },
					 { "darksalmon", "literal" },
					 { "darkseagreen", "literal" },
					 { "darkslateblue", "literal" },
					 { "darkslategray", "literal" },
					 { "darkslategrey", "literal" },
					 { "darkturquoise", "literal" },
					 { "darkviolet", "literal" },
					 { "deeppink", "literal" },
					 { "deepskyblue", "literal" },
					 { "dimgray", "literal" },
					 { "dimgrey", "literal" },
					 { "dodgerblue", "literal" },
					 { "firebrick", "literal" },
					 { "floralwhite", "literal" },
					 { "forestgreen", "literal" },
					 { "fuchsia", "literal" },
					 { "gainsboro", "literal" },
					 { "ghostwhite", "literal" },
					 { "gold", "literal" },
					 { "goldenrod", "literal" },
					 { "gray", "literal" },
					 { "green", "literal" },
					 { "greenyellow", "literal" },
					 { "grey", "literal" },
					 { "honeydew", "literal" },
					 { "hotpink", "literal" },
					 { "indianred", "literal" },
					 { "indigo", "literal" },
					 { "ivory", "literal" },
					 { "khaki", "literal" },
					 { "lavender", "literal" },
					 { "lavenderblush", "literal" },
					 { "lawngreen", "literal" },
					 { "lemonchiffon", "literal" },
					 { "lightblue", "literal" },
					 { "lightcoral", "literal" },
					 { "lightcyan", "literal" },
					 { "lightgoldenrodyellow", "literal" },
					 { "lightgray", "literal" },
					 { "lightgreen", "literal" },
					 { "lightgrey", "literal" },
					 { "lightpink", "literal" },
					 { "lightsalmon", "literal" },
					 { "lightseagreen", "literal" },
					 { "lightskyblue", "literal" },
					 { "lightslategray", "literal" },
					 { "lightslategrey", "literal" },
					 { "lightsteelblue", "literal" },
					 { "lightyellow", "literal" },
					 { "lime", "literal" },
					 { "limegreen", "literal" },
					 { "linen", "literal" },
					 { "magenta", "literal" },
					 { "maroon", "literal" },
					 { "mediumaquamarine", "literal" },
					 { "mediumblue", "literal" },
					 { "mediumorchid", "literal" },
					 { "mediumpurple", "literal" },
					 { "mediumseagreen", "literal" },
					 { "mediumslateblue", "literal" },
					 { "mediumspringgreen", "literal" },
					 { "mediumturquoise", "literal" },
					 { "mediumvioletred", "literal" },
					 { "midnightblue", "literal" },
					 { "mintcream", "literal" },
					 { "mistyrose", "literal" },
					 { "moccasin", "literal" },
					 { "navajowhite", "literal" },
					 { "navy", "literal" },
					 { "oldlace", "literal" },
					 { "olive", "literal" },
					 { "olivedrab", "literal" },
					 { "orange", "literal" },
					 { "orangered", "literal" },
					 { "orchid", "literal" },
					 { "palegoldenrod", "literal" },
					 { "palegreen", "literal" },
					 { "paleturquoise", "literal" },
					 { "palevioletred", "literal" },
					 { "papayawhip", "literal" },
					 { "peachpuff", "literal" },
					 { "peru", "literal" },
					 { "pink", "literal" },
					 { "plum", "literal" },
					 { "powderblue", "literal" },
					 { "purple", "literal" },
					 { "red", "literal" },
					 { "rosybrown", "literal" },
					 { "royalblue", "literal" },
					 { "saddlebrown", "literal" },
					 { "salmon", "literal" },
					 { "sandybrown", "literal" },
					 { "seagreen", "literal" },
					 { "seashell", "literal" },
					 { "sienna", "literal" },
					 { "silver", "literal" },
					 { "skyblue", "literal" },
					 { "slateblue", "literal" },
					 { "slategray", "literal" },
					 { "slategrey", "literal" },
					 { "snow", "literal" },
					 { "springgreen", "literal" },
					 { "steelblue", "literal" },
					 { "tan", "literal" },
					 { "teal", "literal" },
					 { "thistle", "literal" },
					 { "tomato", "literal" },
					 { "turquoise", "literal" },
					 { "violet", "literal" },
					 { "wheat", "literal" },
					 { "white", "literal" },
					 { "whitesmoke", "literal" },
					 { "yellow", "literal" },
					 { "yellowgreen", "literal" },
				 } } )
		.addSymbols( UIWidgetCreator::getWidgetNames(), "keyword" );
}

static void addMarkdown() {
	SyntaxDefinitionManager::instance()->add(
		{ "Markdown",
		  { "%.md$", "%.markdown$" },
		  {
			  { { "\\." }, "normal" },
			  { { "```[Xx][Mm][Ll]", "```" }, "function", "XML" },
			  { { "```[Hh][Tt][Mm][Ll]", "```" }, "function", "html" },
			  { { "```[Cc]++", "```" }, "function", "C++" },
			  { { "```[Cc][Pp][Pp]", "```" }, "function", "C++" },
			  { { "```[Cc]sharp", "```" }, "function", "C#" },
			  { { "```[Cc][Ss][Ss]", "```" }, "function", "CSS" },
			  { { "```[Cc]", "```" }, "function", "C" },
			  { { "```[Dd][Aa][Rr][Tt]", "```" }, "function", "Dart" },
			  { { "```[Dd]", "```" }, "function", "D" },
			  { { "```[Ll]ua", "```" }, "function", "Lua" },
			  { { "```[Jj][Ss][Oo][Nn]", "```" }, "function", "JSON" },
			  { { "```[Ja]va[Ss]cript", "```" }, "function", "JavaScript" },
			  { { "```[Tt]ype[Ss]cript", "```" }, "function", "TypeScript" },
			  { { "```[Pp]ython", "```" }, "function", "Python" },
			  { { "```[Bb]ash", "```" }, "function", "Bash" },
			  { { "```[Pp][Hh][Pp]", "```" }, "function", "PHPCore" },
			  { { "```[Ss][Qq][Ll]", "```" }, "function", "SQL" },
			  { { "```[Gg][Ll][Ss][Ll]", "```" }, "function", "GLSL" },
			  { { "```[Ii][Nn][Ii]", "```" }, "function", "Config File" },
			  { { "```[Mm]akefile", "```" }, "function", "Makefile" },
			  { { "```[Gg][Oo]", "```" }, "function", "Go" },
			  { { "```[Rr]ust", "```" }, "function", "Rust" },
			  { { "```[Rr]uby", "```" }, "function", "Ruby" },
			  { { "```[Gg][Dd][Ss]cript", "```" }, "function", "GSCript" },
			  { { "```[Jj]ava", "```" }, "function", "java" },
			  { { "```[Ss]wift", "```" }, "function", "Swift" },
			  { { "```[Oo]bjective[Cc]", "```" }, "function", "Objective-C" },
			  { { "```[Yy][Aa][Mm][Ll]", "```" }, "function", "YAML" },
			  { { "```[Kk]otlin", "```" }, "function", "Kotlin" },
			  { { "```[Ss]olidity", "```" }, "function", "Solidity" },
			  { { "```[Hh]askell", "```" }, "function", "Haskell" },
			  { { "```[Oo]din", "```" }, "function", "Odin" },
			  { { "```[Nn]im", "```" }, "function", "Nim" },
			  { { "```[Zz]ig", "```" }, "function", "Zig" },
			  { { "<!%-%-", "%-%->" }, "comment" },
			  { { "```", "```" }, "string" },
			  { { "``", "``" }, "string" },
			  { { "`", "`" }, "string" },
			  { { "~~", "~~", "\\" }, "keyword2" },
			  { { "%-%-%-+" }, "comment" },
			  { { "%*%s+" }, "operator" },
			  { { "%*", "[%*\n]", "\\" }, "operator" },
			  { { "%s%_", "[%_\n]", "\\" }, "keyword2" },
			  { { "^%_", "[%_\n]", "\\" }, "keyword2" },
			  { { "^#.-\n" }, "keyword" },
			  { { "%[!%[([^%]].-)%]%((https?://[%w_.~!*:@&+$/?%%#-]-%w[-.%w]*%.%w%w%w?%w?:?%d*/"
				  "?[%w_.~!*:@&+$/?%%#=-]*)%)%]%((https?://[%w_.~!*:@&+$/"
				  "?%%#-]-%w[-.%w]*%.%w%w%w?%w?:?%d*/?[%w_.~!*:@&+$/?%%#=-]*)%)" },
				{ "keyword", "function", "link", "link" } },
			  { { "!?%[([^%]].-)%]%((https?://[%w_.~!*:@&+$/?%%#-]-%w[-.%w]*%.%w%w%w?%w?:?%d*/"
				  "?[%w_.~!*:@&+$/?%%#=-]*)%)" },
				{ "keyword", "function", "link" } },
			  { { "!?%[([^%]].-)%]%((%#+[%w-]*)%)" }, { "keyword", "function", "link" } },
			  { { "https?://[%w_.~!*:@&+$/?%%#-]-%w[-.%w]*%.%w%w%w?%w?:?%d*/?[%w_.~!*:@&+$/"
				  "?%%#=-]*" },
				"link" },
		  } } );
}

static void addC() {
	SyntaxDefinitionManager::instance()->add(
		{ "C",
		  { "%.c$", "%.C", "%.h$", "%.icc" },
		  {
			  { { "//.-\n" }, "comment" },
			  { { "/%*", "%*/" }, "comment" },
			  { { "^%s*(#include)%s+([<%\"][%w%d%.%\\%/%_%-]+[>%\"])" },
				{ "keyword", "keyword", "literal" } },
			  { { "^%s*(#e?l?n?d?ifn?d?e?f?)%s+" }, { "keyword", "keyword", "literal" } },
			  { { "^%s*(#define)%s*" }, { "keyword", "keyword", "literal" } },
			  { { "^%s*(#else)%s*" }, { "keyword", "keyword", "literal" } },
			  { { "^%s*#", "[^\\]\n" }, "comment" },
			  { { "\"", "[\"\n]", "\\" }, "string" },
			  { { "'", "'", "\\" }, "string" },
			  { { "-?0x%x+" }, "number" },
			  { { "-?%d+[%d%.eE]*f?" }, "number" },
			  { { "-?%.?%d+f?" }, "number" },
			  { { "[%+%-=/%*%^%%<>!~|&]" }, "operator" },
			  { { "[%a_][%w_]*%f[(]" }, "function" },
			  { { "[%a_][%w_]*" }, "symbol" },
		  },
		  { { "if", "keyword" },		{ "then", "keyword" },		{ "else", "keyword" },
			{ "elseif", "keyword" },	{ "do", "keyword" },		{ "while", "keyword" },
			{ "for", "keyword" },		{ "break", "keyword" },		{ "continue", "keyword" },
			{ "return", "keyword" },	{ "goto", "keyword" },		{ "struct", "keyword" },
			{ "union", "keyword" },		{ "typedef", "keyword" },	{ "enum", "keyword" },
			{ "extern", "keyword" },	{ "static", "keyword" },	{ "volatile", "keyword" },
			{ "const", "keyword" },		{ "inline", "keyword" },	{ "switch", "keyword" },
			{ "case", "keyword" },		{ "default", "keyword" },	{ "auto", "keyword" },
			{ "const", "keyword" },		{ "void", "keyword" },		{ "int", "keyword2" },
			{ "short", "keyword2" },	{ "long", "keyword2" },		{ "float", "keyword2" },
			{ "double", "keyword2" },	{ "char", "keyword2" },		{ "unsigned", "keyword2" },
			{ "bool", "keyword2" },		{ "true", "literal" },		{ "false", "literal" },
			{ "NULL", "literal" },		{ "uint64_t", "keyword2" }, { "uint32_t", "keyword2" },
			{ "uint16_t", "keyword2" }, { "uint8_t", "keyword2" },	{ "int64_t", "keyword2" },
			{ "int32_t", "keyword2" },	{ "int16_t", "keyword2" },	{ "int8_t", "keyword2" } },
		  "//" } );
}

static void addLua() {
	SyntaxDefinitionManager::instance()->add(
		{ "Lua",
		  { "%.lua$" },
		  {
			  { { "\"", "\"", "\\" }, "string" },
			  { { "'", "'", "\\" }, "string" },
			  { { "%[%[", "%]%]" }, "string" },
			  { { "%-%-%[%[", "%]%]" }, "comment" },
			  { { "%-%-.-\n" }, "comment" },
			  { { "-?0x%x+" }, "number" },
			  { { "-?%d+[%d%.eE]*" }, "number" },
			  { { "-?%.?%d+" }, "number" },
			  { { "%.%.%.?" }, "operator" },
			  { { "[<>~=]=" }, "operator" },
			  { { "[%+%-=/%*%^%%#<>]" }, "operator" },
			  { { "[%a_][%w_]*%s*%f[(\"{]" }, "function" },
			  { { "[%a_][%w_]*" }, "symbol" },
			  { { "::[%a_][%w_]*::" }, "function" },
		  },
		  {
			  { "if", "keyword" },		 { "then", "keyword" },	  { "else", "keyword" },
			  { "elseif", "keyword" },	 { "end", "keyword" },	  { "do", "keyword" },
			  { "function", "keyword" }, { "repeat", "keyword" }, { "until", "keyword" },
			  { "while", "keyword" },	 { "for", "keyword" },	  { "break", "keyword" },
			  { "return", "keyword" },	 { "local", "keyword" },  { "in", "keyword" },
			  { "not", "keyword" },		 { "and", "keyword" },	  { "or", "keyword" },
			  { "goto", "keyword" },	 { "self", "keyword2" },  { "true", "literal" },
			  { "false", "literal" },	 { "nil", "literal" },
		  },
		  "--",
		  { "^#!.*[ /]lua" } } );
}

static void addJavaScript() {
	SyntaxDefinitionManager::instance()->add(
		{ "JavaScript",
		  { "%.js$" },
		  {
			  { { "//.-\n" }, "comment" },
			  { { "/%*", "%*/" }, "comment" },
			  { { "\"", "\"", "\\" }, "string" },
			  { { "'", "'", "\\" }, "string" },
			  { { "`", "`", "\\" }, "string" },
			  { { "/[%+%-%*%^%!%=%&%|%?%:%;%,%(%[%{%<%>%\\].*%f[/]",
				  "/[igmsuyd\n]?[igmsuyd\n]?[igmsuyd\n]?", "\\" },
				"string" },
			  { { "0x[%da-fA-F]+" }, "number" },
			  { { "-?%d+[%d%.eE]*" }, "number" },
			  { { "-?%.?%d+" }, "number" },
			  { { "[%+%-=/%*%^%%<>!~|&]" }, "operator" },
			  { { "([%w_][%w_]+)%.([%w_][%w%d_]*)%s*(=)%s*(function)" },
				{ "normal", "keyword2", "function", "operator", "keyword" } },
			  { { "([%w_][%w_]+)%.([%w_][%w%d_]*)%s*(=)%s*(async%s*function)" },
				{ "normal", "keyword2", "function", "operator", "keyword" } },
			  { { "([%w_][%w_]+)%.([%w_][%w%d_]*)%s*(=)%s*%f[(]" },
				{ "normal", "keyword2", "function", "operator" } },
			  { { "([%w_][%w_]+)%.([%w_][%w%d_]*)%s*(=)%s*(async)%s*%f[(]" },
				{ "normal", "keyword2", "function", "operator", "function" } },
			  { { "[%a_][%w_]*%f[(]" }, "function" },
			  { { "[%a_][%w_]*" }, "symbol" },
		  },
		  { { "arguments", "keyword2" }, { "async", "keyword" },	 { "await", "keyword" },
			{ "break", "keyword" },		 { "case", "keyword" },		 { "catch", "keyword" },
			{ "class", "keyword" },		 { "const", "keyword" },	 { "continue", "keyword" },
			{ "debugger", "keyword" },	 { "default", "keyword" },	 { "delete", "keyword" },
			{ "do", "keyword" },		 { "else", "keyword" },		 { "export", "keyword" },
			{ "extends", "keyword" },	 { "false", "literal" },	 { "finally", "keyword" },
			{ "for", "keyword" },		 { "get", "keyword" },		 { "if", "keyword" },
			{ "import", "keyword" },	 { "in", "keyword" },		 { "Infinity", "keyword2" },
			{ "instanceof", "keyword" }, { "let", "keyword" },		 { "NaN", "keyword2" },
			{ "new", "keyword" },		 { "null", "literal" },		 { "return", "keyword" },
			{ "set", "keyword" },		 { "super", "keyword" },	 { "static", "keyword" },
			{ "switch", "keyword" },	 { "this", "keyword2" },	 { "throw", "keyword" },
			{ "true", "literal" },		 { "try", "keyword" },		 { "typeof", "keyword" },
			{ "undefined", "literal" },	 { "var", "keyword" },		 { "void", "keyword" },
			{ "while", "keyword" },		 { "with", "keyword" },		 { "yield", "keyword" },
			{ "implements", "keyword" }, { "Array", "keyword2" },	 { "any", "keyword" },
			{ "from", "keyword" },		 { "public", "keyword" },	 { "private", "keyword" },
			{ "declare", "keyword" },	 { "namespace", "keyword" }, { "protected", "keyword" },
			{ "enum", "keyword" },		 { "function", "keyword" },	 { "of", "keyword" } },
		  "//" } );
}

static void addJSON() {
	SyntaxDefinitionManager::instance()->add(
		{ "JSON",
		  { "%.json$", "%.cson$", "%.webmanifest" },
		  {
			  { { "(%b\"\")(:)" }, { "normal", "keyword", "operator" } },
			  { { "\"", "\"", "\\" }, "string" },
			  { { "'", "'", "\\" }, "string" },
			  { { "`", "`", "\\" }, "string" },
			  { { "0x[%da-fA-F]+" }, "number" },
			  { { "-?%d+[%d%.eE]*" }, "number" },
			  { { "-?%.?%d+" }, "number" },
			  { { "[%[%]%{%}]" }, "operator" },
			  { { "[%a_][%w_]*" }, "symbol" },
		  },
		  { { "true", "literal" }, { "false", "literal" } },
		  "//" } );
}

static void addTypeScript() {
	SyntaxDefinition& ts = SyntaxDefinitionManager::instance()->add(
		{ "TypeScript",
		  { "%.ts$", "%.d.ts$" },
		  {
			  { { "//.-\n" }, "comment" },
			  { { "/%*", "%*/" }, "comment" },
			  { { "\"", "\"", "\\" }, "string" },
			  { { "'", "'", "\\" }, "string" },
			  { { "`", "`", "\\" }, "string" },
			  { { "/[%+%-%*%^%!%=%&%|%?%:%;%,%(%[%{%<%>%\\].*%f[/]",
				  "/[igmsuyd\n]?[igmsuyd\n]?[igmsuyd\n]?", "\\" },
				"string" },
			  { { "0x[%da-fA-F]+" }, "number" },
			  { { "-?%d+[%d%.eE]*" }, "number" },
			  { { "-?%.?%d+" }, "number" },
			  { { "[%+%-=/%*%^%%<>!~|&]" }, "operator" },
			  { { "(interface%s)([%a_][%w_]*)" }, { "normal", "keyword", "keyword2" } },
			  { { "(type%s)([%a_][%w_]*)" }, { "normal", "keyword", "keyword2" } },
			  { { "[%a_][%w_$]*%f[(]" }, "function" },
			  { { "[%a_][%w_]*" }, "symbol" },
		  },
		  { { "any", "keyword2" },		 { "arguments", "keyword2" }, { "as", "keyword" },
			{ "async", "keyword" },		 { "await", "keyword" },	  { "boolean", "keyword2" },
			{ "break", "keyword" },		 { "case", "keyword" },		  { "catch", "keyword" },
			{ "class", "keyword" },		 { "const", "keyword" },	  { "constructor", "function" },
			{ "continue", "keyword" },	 { "debugger", "keyword" },	  { "declare", "keyword" },
			{ "default", "keyword" },	 { "delete", "keyword" },	  { "do", "keyword" },
			{ "else", "keyword" },		 { "enum", "keyword" },		  { "export", "keyword" },
			{ "extends", "keyword" },	 { "false", "literal" },	  { "finally", "keyword" },
			{ "for", "keyword" },		 { "from", "keyword" },		  { "function", "keyword" },
			{ "get", "keyword" },		 { "if", "keyword" },		  { "implements", "keyword" },
			{ "import", "keyword" },	 { "in", "keyword" },		  { "Infinity", "keyword2" },
			{ "instanceof", "keyword" }, { "interface", "keyword" },  { "let", "keyword" },
			{ "module", "keyword" },	 { "new", "keyword" },		  { "null", "literal" },
			{ "number", "keyword2" },	 { "of", "keyword" },		  { "package", "keyword" },
			{ "private", "keyword" },	 { "protected", "keyword" },  { "public", "keyword" },
			{ "require", "keyword" },	 { "return", "keyword" },	  { "set", "keyword" },
			{ "static", "keyword" },	 { "string", "keyword2" },	  { "super", "keyword" },
			{ "switch", "keyword" },	 { "symbol", "keyword2" },	  { "this", "keyword2" },
			{ "throw", "keyword" },		 { "true", "literal" },		  { "try", "keyword" },
			{ "type", "keyword2" },		 { "typeof", "keyword" },	  { "undefined", "literal" },
			{ "var", "keyword" },		 { "void", "keyword" },		  { "while", "keyword" },
			{ "with", "keyword" },		 { "yield", "keyword" },	  { "unknown", "keyword2" } },
		  "//" } );

	const SyntaxDefinition& html = SyntaxDefinitionManager::instance()->getByLSPName( "xml" );
	SyntaxDefinition tsxml( html );
	tsxml.setLanguageName( "TSXML" );
	tsxml.setFileTypes( {} );
	tsxml.setVisible( false );
	tsxml.addPatternToFront( { { "%{", "%}", "\\" }, "normal", "JavaScript" } );
	SyntaxDefinitionManager::instance()->add( std::move( tsxml ) );

	SyntaxDefinition tsx( ts );
	tsx.setLanguageName( "TSX" );
	tsx.setFileTypes( { "%.tsx$" } );
	tsx.addPatternToFront( { { "return%s+%(", "%s*);" }, "keyword", "TSXML" } );
	SyntaxDefinitionManager::instance()->add( std::move( tsx ) );
}

static void addPython() {
	SyntaxDefinitionManager::instance()->add(
		{ "Python",
		  { "%.py$", "%.pyw$" },
		  {
			  { { "#", "\n" }, "comment" },
			  { { "[ruU]?\"", "\"", "\\" }, "string" },
			  { { "[ruU]?'", "'", "\\" }, "string" },
			  { { "\"\"\"", "\"\"\"" }, "string" },
			  { { "0x[%da-fA-F]+" }, "number" },
			  { { "-?%d+[%d%.eE]*" }, "number" },
			  { { "-?%.?%d+" }, "number" },
			  { { "[%+%-=/%*%^%%<>!~|&]" }, "operator" },
			  { { "[%a_][%w_]*%f[(]" }, "function" },
			  { { "[%a_][%w_]*" }, "symbol" },
		  },
		  {
			  { "class", "keyword" },  { "finally", "keyword" },  { "is", "keyword" },
			  { "return", "keyword" }, { "continue", "keyword" }, { "for", "keyword" },
			  { "lambda", "keyword" }, { "try", "keyword" },	  { "def", "keyword" },
			  { "from", "keyword" },   { "nonlocal", "keyword" }, { "while", "keyword" },
			  { "and", "keyword" },	   { "global", "keyword" },	  { "not", "keyword" },
			  { "with", "keyword" },   { "as", "keyword" },		  { "elif", "keyword" },
			  { "if", "keyword" },	   { "or", "keyword" },		  { "else", "keyword" },
			  { "import", "keyword" }, { "pass", "keyword" },	  { "break", "keyword" },
			  { "except", "keyword" }, { "in", "keyword" },		  { "del", "keyword" },
			  { "raise", "keyword" },  { "yield", "keyword" },	  { "assert", "keyword" },
			  { "self", "keyword2" },  { "None", "literal" },	  { "True", "literal" },
			  { "False", "literal" },
		  },
		  "#",
		  { "^#!.*[ /]python", "^#!.*[ /]python3" } } );
}

static void addBash() {
	SyntaxDefinitionManager::instance()->add(

		{ "Shell script",
		  { "%.sh$", "%.bash$", "^%.bashrc$", "^%.bash_profile$", "^%.profile$" },
		  {
			  { { "$[%a_@*#][%w_]*" }, "keyword2" },
			  { { "#.*\n" }, "comment" },
			  { { "\"", "\"", "\\" }, "string" },
			  { { "'", "'", "\\" }, "string" },
			  { { "`", "`", "\\" }, "string" },
			  { { "%f[%w_%.%/]%d[%d%.]*%f[^%w_%.]" }, "number" },
			  { { "[!<>|&%[%]:=*]" }, "operator" },
			  { { "%f[%S][%+%-][%w%-_:]+" }, "function" },
			  { { "%f[%S][%+%-][%w%-_]+%f[=]" }, "function" },
			  { { "(%s%-%a[%w_%-]*%s+)(%d[%d%.]+)" }, { "normal", "function", "number" } },
			  { { "(%s%-%a[%w_%-]*%s+)(%a[%a%-_:=]+)" }, { "normal", "function", "symbol" } },
			  { { "[_%a][%w_]+%f[%+=]" }, "keyword2" },
			  { { "${.-}" }, "keyword2" },
			  { { "$[%d$%a_@*][%w_]*" }, "keyword2" },
			  { { "[%a_%-][%w_%-]*[%s]*%f[(]" }, "function" },
			  { { "[%a_][%w_]*" }, "symbol" },
			  { { "%s+" }, "normal" },
			  { { "%w+%f[%s]" }, "normal" },

		  },
		  {
			  { "until", "keyword" },	  { "unset", "keyword" },	 { "unalias", "keyword" },
			  { "type", "keyword" },	  { "time", "keyword" },	 { "test", "keyword" },
			  { "source", "keyword" },	  { "true", "literal" },	 { "shift", "keyword" },
			  { "set", "keyword" },		  { "then", "keyword" },	 { "select", "keyword" },
			  { "readarray", "keyword" }, { "pwd", "keyword" },		 { "mapfile", "keyword" },
			  { "local", "keyword" },	  { "return", "keyword" },	 { "let", "keyword" },
			  { "fi", "keyword" },		  { "getopts", "keyword" },	 { "echo", "keyword" },
			  { "do", "keyword" },		  { "eval", "keyword" },	 { "elif", "keyword" },
			  { "declare", "keyword" },	  { "cd", "keyword" },		 { "case", "keyword" },
			  { "printf", "keyword" },	  { "break", "keyword" },	 { "exec", "keyword" },
			  { "alias", "keyword" },	  { "exit", "keyword" },	 { "esac", "keyword" },
			  { "export", "keyword" },	  { "for", "keyword" },		 { "enable", "keyword" },
			  { "jobs", "keyword" },	  { "function", "keyword" }, { "while", "keyword" },
			  { "read", "keyword" },	  { "hash", "keyword" },	 { "help", "keyword" },
			  { "history", "keyword" },	  { "done", "keyword" },	 { "if", "keyword" },
			  { "false", "literal" },	  { "in", "keyword" },		 { "else", "keyword" },
			  { "continue", "keyword" },  { "kill", "keyword" },

		  },
		  "#",
		  { "^#!.*[ /]bash", "^#!.*[ /]sh" },
		  "shellscript" } );
}

static void addCPP() {
	SyntaxDefinitionManager::instance()->add(
		{ "C++",
		  { "%.cpp$", "%.cc$", "%.cxx$", "%.c++$", "%.hh$", "%.inl$", "%.hxx$", "%.hpp$",
			"%.h++$" },
		  {
			  { { "R%\"xml%(", "%)xml%\"" }, "function", "XML" },
			  { { "R%\"css%(", "%)css%\"" }, "function", "CSS" },
			  { { "R%\"html%(", "%)html%\"" }, "function", "HTML" },
			  { { "R%\"json%(", "%)json%\"" }, "function", "JSON" },
			  { { "R\"[%a-\"]+%(", "%)[%a-\"]+%\"" }, "string" },
			  { { "R\"%(", "%)\"" }, "string" },
			  { { "//.-\n" }, "comment" },
			  { { "/%*", "%*/" }, "comment" },
			  { { "\"", "[\"\n]", "\\" }, "string" },
			  { { "'", "'", "\\" }, "string" },
			  { { "^%s*(#include)%s+([<%\"][%w%d%.%\\%/%_%-]+[>%\"])" },
				{ "keyword", "keyword", "literal" } },
			  { { "^%s*(#e?l?n?d?ifn?d?e?f?)%s+" }, { "keyword", "keyword", "literal" } },
			  { { "^%s*(#define)%s*" }, { "keyword", "keyword", "literal" } },
			  { { "^%s*(#else)%s*" }, { "keyword", "keyword", "literal" } },
			  { { "^%s*#", "[^\\]\n" }, "comment" },
			  { { "-?0x%x+" }, "number" },
			  { { "-?%d+[%d%.eE]*f?" }, "number" },
			  { { "-?%.?%d+f?" }, "number" },
			  { { "[%+%-=/%*%^%%<>!~|&]" }, "operator" },
			  { { "[%a_][%w_]*%f[(]" }, "function" },
			  { { "std%:%:[%w_]*" }, "keyword2" },
			  { { "[%a_][%w_]*" }, "symbol" },
		  },
		  {
			  { "alignof", "keyword" },
			  { "alignas", "keyword" },
			  { "and", "keyword" },
			  { "and_eq", "keyword" },
			  { "not", "keyword" },
			  { "not_eq", "keyword" },
			  { "or", "keyword" },
			  { "or_eq", "keyword" },
			  { "xor", "keyword" },
			  { "xor_eq", "keyword" },
			  { "private", "keyword" },
			  { "protected", "keyword" },
			  { "public", "keyword" },
			  { "register", "keyword" },
			  { "nullptr", "keyword" },
			  { "operator", "keyword" },
			  { "asm", "keyword" },
			  { "bitand", "keyword" },
			  { "bitor", "keyword" },
			  { "catch", "keyword" },
			  { "throw", "keyword" },
			  { "try", "keyword" },
			  { "class", "keyword" },
			  { "compl", "keyword" },
			  { "explicit", "keyword" },
			  { "export", "keyword" },
			  { "concept", "keyword" },
			  { "consteval", "keyword" },
			  { "constexpr", "keyword" },
			  { "constinit", "keyword" },
			  { "const_cast", "keyword" },
			  { "dynamic_cast", "keyword" },
			  { "reinterpret_cast", "keyword" },
			  { "static_cast", "keyword" },
			  { "static_assert", "keyword" },
			  { "template", "keyword" },
			  { "this", "keyword" },
			  { "thread_local", "keyword" },
			  { "requires", "keyword" },
			  { "co_wait", "keyword" },
			  { "co_return", "keyword" },
			  { "co_yield", "keyword" },
			  { "decltype", "keyword" },
			  { "delete", "keyword" },
			  { "export", "keyword" },
			  { "friend", "keyword" },
			  { "typeid", "keyword" },
			  { "typename", "keyword" },
			  { "mutable", "keyword" },
			  { "virtual", "keyword" },
			  { "using", "keyword" },
			  { "namespace", "keyword" },
			  { "new", "keyword" },
			  { "noexcept", "keyword" },
			  { "if", "keyword" },
			  { "then", "keyword" },
			  { "else", "keyword" },
			  { "elseif", "keyword" },
			  { "do", "keyword" },
			  { "while", "keyword" },
			  { "for", "keyword" },
			  { "break", "keyword" },
			  { "continue", "keyword" },
			  { "return", "keyword" },
			  { "goto", "keyword" },
			  { "struct", "keyword" },
			  { "union", "keyword" },
			  { "typedef", "keyword" },
			  { "enum", "keyword" },
			  { "extern", "keyword" },
			  { "static", "keyword" },
			  { "volatile", "keyword" },
			  { "const", "keyword" },
			  { "inline", "keyword" },
			  { "switch", "keyword" },
			  { "case", "keyword" },
			  { "default", "keyword" },
			  { "auto", "keyword2" },
			  { "const", "keyword" },
			  { "void", "keyword" },
			  { "int", "keyword2" },
			  { "short", "keyword2" },
			  { "long", "keyword2" },
			  { "float", "keyword2" },
			  { "double", "keyword2" },
			  { "char", "keyword2" },
			  { "unsigned", "keyword2" },
			  { "bool", "keyword2" },
			  { "true", "keyword2" },
			  { "false", "keyword2" },
			  { "wchar_t", "keyword2" },
			  { "char8_t", "keyword2" },
			  { "char16_t", "keyword2" },
			  { "char32_t", "keyword2" },
			  { "size_t", "keyword2" },
			  { "int16_t", "keyword2" },
			  { "int32_t", "keyword2" },
			  { "int64_t", "keyword2" },
			  { "uint16_t", "keyword2" },
			  { "uint32_t", "keyword2" },
			  { "uint64_t", "keyword2" },
			  { "String", "keyword2" },
			  { "Int8", "keyword2" },
			  { "Uint8", "keyword2" },
			  { "Int16", "keyword2" },
			  { "Uint16", "keyword2" },
			  { "Int32", "keyword2" },
			  { "Uint32", "keyword2" },
			  { "Int64", "keyword2" },
			  { "Uint64", "keyword2" },
			  { "Float", "keyword2" },
			  { "Color", "keyword2" },
			  { "Vector2f", "keyword2" },
			  { "Vector2i", "keyword2" },
			  { "Recti", "keyword2" },
			  { "Rectf", "keyword2" },
			  { "NULL", "literal" },
		  },
		  "//",
		  {},
		  "cpp" } );
}

static void addPHP() {
	SyntaxDefinitionManager::instance()
		->add( { "PHP",
				 { "%.php$", "%.php3$", "%.php4$", "%.php5$" },
				 {
					 { { "<%s*[sS][cC][rR][iI][pP][tT]%s+[tT][yY][pP][eE]%s*=%s*['\"]%a+/"
						 "[jJ][aA][vV][aA][sS][cC][rR][iI][pP][tT]['\"]%s*>",
						 "<%s*/[sS][cC][rR][iI][pP][tT]>" },
					   "function",
					   "JavaScript" },
					 { { "<%s*[sS][cC][rR][iI][pP][tT]%s*>", "<%s*/%s*[sS][cC][rR][iI][pP][tT]>" },
					   "function",
					   "JavaScript" },
					 { { "<%s*[sS][tT][yY][lL][eE][^>]*>", "<%s*/%s*[sS][tT][yY][lL][eE]%s*>" },
					   "function",
					   "CSS" },
					 { { "<%?p?h?p?", "%?>" }, "function", "PHPCore" },
					 { { "<!%-%-", "%-%->" }, "comment" },
					 { { "%f[^>][^<]", "%f[<]" }, "normal" },
					 { { "\"", "\"", "\\" }, "string" },
					 { { "'", "'", "\\" }, "string" },
					 { { "0x[%da-fA-F]+" }, "number" },
					 { { "-?%d+[%d%.]*f?" }, "number" },
					 { { "-?%.?%d+f?" }, "number" },
					 { { "%f[^<]![%a_][%w%_%-]*" }, "keyword2" },
					 { { "%f[^<][%a_][%w%_%-]*" }, "function" },
					 { { "%f[^<]/[%a_][%w%_%-]*" }, "function" },
					 { { "[%a_][%w_]*" }, "keyword" },
					 { { "[/<>=]" }, "operator" },
				 },
				 {},
				 "",
				 { "^#!.*[ /]php" } } )
		.setAutoCloseXMLTags( true );

	SyntaxDefinitionManager::instance()
		->add( { "PHPCore",
				 {},
				 {
					 { { "<%?p?h?p?" }, "function" },
					 { { "%?>", "<%?p?h?p?" }, "function", "HTML" },
					 { { "//.-\n" }, "comment" },
					 { { "/%*", "%*/" }, "comment" },
					 { { "#.-\n" }, "comment" },
					 { { "\"", "\"", "\\" }, "string" },
					 { { "'", "'", "\\" }, "string" },
					 { { "%\\x[%da-fA-F]+" }, "number" },
					 { { "-?%d+[%d%.eE]*" }, "number" },
					 { { "-?%.?%d+" }, "number" },
					 { { "[%.%+%-=/%*%^%%<>!~|&]" }, "operator" },
					 { { "[%a_][%w_]*%f[(]" }, "function" },
					 { { "[%a_][%w_]*" }, "symbol" },
					 { { "%$[%a][%w_]*" }, "keyword2" },
				 },
				 { { "return", "keyword" },		 { "if", "keyword" },
				   { "else", "keyword" },		 { "elseif", "keyword" },
				   { "endif", "keyword" },		 { "declare", "keyword" },
				   { "enddeclare", "keyword" },	 { "switch", "keyword" },
				   { "endswitch", "keyword" },	 { "as", "keyword" },
				   { "do", "keyword" },			 { "for", "keyword" },
				   { "endfor", "keyword" },		 { "foreach", "keyword" },
				   { "endforeach", "keyword" },	 { "while", "keyword" },
				   { "endwhile", "keyword" },	 { "switch", "keyword" },
				   { "case", "keyword" },		 { "continue", "keyword" },
				   { "default", "keyword" },	 { "break", "keyword" },
				   { "exit", "keyword" },		 { "goto", "keyword" },

				   { "catch", "keyword" },		 { "throw", "keyword" },
				   { "try", "keyword" },		 { "finally", "keyword" },

				   { "class", "keyword" },		 { "trait", "keyword" },
				   { "interface", "keyword" },	 { "public", "keyword" },
				   { "static", "keyword" },		 { "protected", "keyword" },
				   { "private", "keyword" },	 { "abstract", "keyword" },
				   { "final", "keyword" },

				   { "function", "keyword2" },	 { "global", "keyword2" },
				   { "var", "keyword2" },		 { "const", "keyword2" },
				   { "bool", "keyword2" },		 { "boolean", "keyword2" },
				   { "int", "keyword2" },		 { "integer", "keyword2" },
				   { "real", "keyword2" },		 { "double", "keyword2" },
				   { "float", "keyword2" },		 { "string", "keyword2" },
				   { "array", "keyword2" },		 { "object", "keyword2" },
				   { "callable", "keyword2" },	 { "iterable", "keyword2" },

				   { "namespace", "keyword2" },	 { "extends", "keyword2" },
				   { "implements", "keyword2" }, { "instanceof", "keyword2" },
				   { "require", "keyword2" },	 { "require_once", "keyword2" },
				   { "include", "keyword2" },	 { "include_once", "keyword2" },
				   { "use", "keyword2" },		 { "new", "keyword2" },
				   { "clone", "keyword2" },

				   { "true", "literal" },		 { "false", "literal" },
				   { "NULL", "literal" },		 { "parent", "literal" },
				   { "self", "literal" },		 { "echo", "function" } },
				 "//",
				 {},
				 "php" } )
		.setVisible( false );
}

static void addSQL() {
	const std::vector<std::string> keywords = {
		"CREATE",	  "SELECT",		"INSERT",	 "INTO",	  "UPDATE",		  "DELETE",
		"TABLE",	  "DROP",		"VALUES",	 "NOT",		  "NULL",		  "PRIMARY",
		"KEY",		  "REFERENCES", "DEFAULT",	 "UNIQUE",	  "CONSTRAINT",	  "CHECK",
		"ON",		  "EXCLUDE",	"WITH",		 "USING",	  "WHERE",		  "GROUP",
		"BY",		  "HAVING",		"DISTINCT",	 "LIMIT",	  "OFFSET",		  "ONLY",
		"CROSS",	  "JOIN",		"INNER",	 "LEFT",	  "RIGHT",		  "FULL",
		"OUTER",	  "NATURAL",	"AND",		 "OR",		  "AS",			  "ORDER",
		"ORDINALITY", "UNNEST",		"FROM",		 "VIEW",	  "RETURNS",	  "SETOF",
		"LANGUAGE",	  "SQL",		"LIKE",		 "LATERAL",	  "INTERVAL",	  "PARTITION",
		"UNION",	  "INTERSECT",	"EXCEPT",	 "ALL",		  "ASC",		  "DESC",
		"NULLS",	  "FIRST",		"LAST",		 "IN",		  "RECURSIVE",	  "ARRAY",
		"RETURNING",  "SET",		"ALSO",		 "INSTEAD",	  "ALTER",		  "SEQUENCE",
		"OWNED",	  "AT",			"ZONE",		 "WITHOUT",	  "TO",			  "TIMEZONE",
		"TYPE",		  "ENUM",		"DOCUMENT",	 "XMLPARSE",  "XMLSERIALIZE", "CONTENT",
		"OPTION",	  "INDEX",		"ANY",		 "EXTENSION", "ISNULL",		  "NOTNULL",
		"UNKNOWN",	  "CASE",		"THEN",		 "WHEN",	  "ELSE",		  "END",
		"ROWS",		  "BETWEEN",	"UNBOUNDED", "PRECEDING", "UNBOUNDED",	  "FOLLOWING",
		"EXISTS",	  "SOME",		"COLLATION", "FOR",		  "TRIGGER",	  "BEFORE",
		"EACH",		  "ROW",		"EXECUTE",	 "PROCEDURE", "FUNCTION",	  "DECLARE",
		"BEGIN",	  "LOOP",		"RAISE",	 "NOTICE",	  "LOOP",		  "EVENT",
		"OPERATOR",	  "DOMAIN",		"VARIADIC",	 "FOREIGN" };

	const std::vector<std::string> types = {
		"BIGINT",	 "INT8",	 "BIGSERIAL",	  "SERIAL8",	 "BIT",		  "VARBIT",
		"BOOLEAN",	 "BOOL",	 "BOX",			  "BYTEA",		 "CHARACTER", "CHAR",
		"VARCHAR",	 "CIDR",	 "CIRCLE",		  "DATE",		 "DOUBLE",	  "PRECISION",
		"FLOAT8",	 "INET",	 "INTEGER",		  "INT",		 "INT4",	  "INTERVAL",
		"JSON",		 "JSONB",	 "LINE",		  "LSEG",		 "MACADDR",	  "MONEY",
		"NUMERIC",	 "DECIMAL",	 "PATH",		  "POINT",		 "POLYGON",	  "REAL",
		"FLOAT4",	 "INT2",	 "SMALLINT",	  "SMALLSERIAL", "SERIAL2",	  "SERIAL",
		"SERIAL4",	 "TEXT",	 "TIME",		  "TIMEZ",		 "TIMESTAMP", "TIMESTAMPZ",
		"TSQUERY",	 "TSVECTOR", "TXID_SNAPSHOT", "UUID",		 "XML",		  "INT4RANGE",
		"INT8RANGE", "NUMRANGE", "TSRANGE",		  "TSTZRANGE",	 "DATERANGE", "PG_LSN" };

	const std::vector<std::string> literals = {
		"FALSE",		"TRUE",		 "CURRENT_TIMESTAMP", "CURRENT_TIME",
		"CURRENT_DATE", "LOCALTIME", "LOCALTIMESTAMP" };

	std::unordered_map<std::string, std::string> symbols;
	for ( auto& keyword : keywords ) {
		symbols[String::toLower( keyword )] = "keyword";
		symbols[keyword] = "keyword";
	}

	for ( auto& type : types ) {
		symbols[String::toLower( type )] = "keyword2";
		symbols[type] = "keyword2";
	}

	for ( auto& literal : literals ) {
		symbols[String::toLower( literal )] = "literal";
		symbols[literal] = "literal";
	}

	SyntaxDefinitionManager::instance()->add( { "SQL",
												{ "%.sql$", "%.psql$" },
												{
													{ { "%-%-.-\n" }, "comment" },
													{ { "/%*", "%*/" }, "comment" },
													{ { "'", "'", "\\" }, "string" },
													{ { "-?%d+[%d%.eE]*f?" }, "number" },
													{ { "-?%.?%d+f?" }, "number" },
													{ { "[%+%-=/%*%%<>!~|&@%?$#]" }, "operator" },
													{ { "[%a_][%w_]*%f[(]" }, "function" },
													{ { "[%a_][%w_]*" }, "symbol" },
												},
												symbols,
												"--" } );
}

static void addGLSL() {
	SyntaxDefinitionManager::instance()->add(
		{ "GLSL",
		  { "%.glsl$", "%.frag$", "%.vert$", "%.fs$", "%.vs$" },
		  {
			  { { "//.-\n" }, "comment" },
			  { { "/%*", "%*/" }, "comment" },
			  { { "#", "[^\\]\n" }, "comment" },
			  { { "\"", "\"", "\\" }, "string" },
			  { { "'", "'", "\\" }, "string" },
			  { { "-?0x%x+" }, "number" },
			  { { "-?%d+[%d%.eE]*f?" }, "number" },
			  { { "-?%.?%d+f?" }, "number" },
			  { { "[%+%-=/%*%^%%<>!~|&]" }, "operator" },
			  { { "ivec[2-4]" }, "keyword2" },
			  { { "bvec[2-4]" }, "keyword2" },
			  { { "uvec[2-4]" }, "keyword2" },
			  { { "vec[2-4]" }, "keyword2" },
			  { { "dmat[2-4]x[2-4]" }, "keyword2" },
			  { { "dmat[2-4]" }, "keyword2" },
			  { { "mat[2-4]x[2-4]" }, "keyword2" },
			  { { "mat[2-4]" }, "keyword2" },
			  { { "[%a_][%w_]*%f[(]" }, "function" },
			  { { "[%a_][%w_]*" }, "symbol" },
		  },
		  {
			  { "if", "keyword" },
			  { "else", "keyword" },
			  { "do", "keyword" },
			  { "while", "keyword" },
			  { "for", "keyword" },
			  { "break", "keyword" },
			  { "continue", "keyword" },
			  { "return", "keyword" },
			  { "const", "keyword" },
			  { "switch", "keyword" },
			  { "case", "keyword" },
			  { "default", "keyword" },
			  { "const", "keyword" },
			  { "void", "keyword" },
			  { "bool", "keyword2" },
			  { "int", "keyword2" },
			  { "uint", "keyword2" },
			  { "float", "keyword2" },
			  { "double", "keyword2" },
			  { "true", "literal" },
			  { "false", "literal" },
			  { "NULL", "literal" },
			  { "attribute", "keyword" },
			  { "varying", "keyword" },
			  { "uniform", "keyword" },
			  { "buffer", "keyword" },
			  { "shared", "keyword" },
			  { "layout", "keyword" },
			  { "centroid", "keyword" },
			  { "flat", "keyword" },
			  { "smooth", "keyword" },
			  { "noperspective", "keyword" },
			  { "patch", "keyword" },
			  { "sample", "keyword" },
			  { "in", "keyword" },
			  { "out", "keyword" },
			  { "inout", "keyword" },
			  { "invariant", "keyword" },
			  { "precise", "keyword" },
			  { "lowp", "keyword" },
			  { "mediump", "keyword" },
			  { "highp", "keyword" },
			  { "precision", "keyword" },
			  { "struct", "keyword" },
			  { "subroutine", "keyword" },
			  { "coherent", "keyword" },
			  { "volatile", "keyword" },
			  { "readonly", "keyword" },
			  { "writeonly", "keyword" },
			  { "sampler1D", "keyword2" },
			  { "sampler2D", "keyword2" },
			  { "sampler3D", "keyword2" },
			  { "samplerCube", "keyword2" },
			  { "sampler1DShadow", "keyword2" },
			  { "sampler2DShadow", "keyword2" },
			  { "samplerCubeShadow", "keyword2" },
			  { "sampler1DArray", "keyword2" },
			  { "sampler2DArray", "keyword2" },
			  { "samplerCubeArray", "keyword2" },
			  { "sampler1DArrayShadow", "keyword2" },
			  { "sampler2DArrayShadow", "keyword2" },
			  { "samplerCubeArrayShadow", "keyword2" },
			  { "isampler1D", "keyword2" },
			  { "isampler2D", "keyword2" },
			  { "isampler3D", "keyword2" },
			  { "isamplerCube", "keyword2" },
			  { "sampler2DMS", "keyword2" },
			  { "isampler2DMS", "keyword2" },
			  { "usampler2DMS", "keyword2" },
			  { "sampler2DMSArray", "keyword2" },
			  { "isampler2DMSArray", "keyword2" },
			  { "usampler2DMSArray", "keyword2" },
			  { "isampler1DArray", "keyword2" },
			  { "isampler2DArray", "keyword2" },
			  { "usampler1D", "keyword2" },
			  { "usampler2D", "keyword2" },
			  { "usampler3D", "keyword2" },
			  { "usamplerCube", "keyword2" },
			  { "usampler1DArray", "keyword2" },
			  { "usampler2DArray", "keyword2" },
			  { "sampler2DRect", "keyword2" },
			  { "sampler2DRectShadow", "keyword2" },
			  { "isampler2DRect", "keyword2" },
			  { "usampler2DRect", "keyword2" },
			  { "samplerBuffer", "keyword2" },
			  { "isamplerBuffer", "keyword2" },
			  { "usamplerBuffer", "keyword2" },
			  { "image1D", "keyword2" },
			  { "iimage1D", "keyword2" },
			  { "uimage1D", "keyword2" },
			  { "image1DArray", "keyword2" },
			  { "iimage1DArray", "keyword2" },
			  { "uimage1DArray", "keyword2" },
			  { "image2D", "keyword2" },
			  { "iimage2D", "keyword2" },
			  { "uimage2D", "keyword2" },
			  { "image2DArray", "keyword2" },
			  { "iimage2DArray", "keyword2" },
			  { "uimage2DArray", "keyword2" },
			  { "image2DRect", "keyword2" },
			  { "iimage2DRect", "keyword2" },
			  { "uimage2DRect", "keyword2" },
			  { "image2DMS", "keyword2" },
			  { "iimage2DMS", "keyword2" },
			  { "uimage2DMS", "keyword2" },
			  { "image2DMSArray", "keyword2" },
			  { "iimage2DMSArray", "keyword2" },
			  { "uimage2DMSArray", "keyword2" },
			  { "image3D", "keyword2" },
			  { "iimage3D", "keyword2" },
			  { "uimage3D", "keyword2" },
			  { "imageCube", "keyword2" },
			  { "iimageCube", "keyword2" },
			  { "uimageCube", "keyword2" },
			  { "imageCubeArray", "keyword2" },
			  { "iimageCubeArray", "keyword2" },
			  { "uimageCubeArray", "keyword2" },
			  { "imageBuffer", "keyword2" },
			  { "iimageBuffer", "keyword2" },
			  { "uimageBuffer", "keyword2" },
			  { "atomic_uint", "keyword2" },
			  { "radians", "keyword" },
			  { "degrees", "keyword" },
			  { "sin", "keyword" },
			  { "cos", "keyword" },
			  { "tan", "keyword" },
			  { "asin", "keyword" },
			  { "acos", "keyword" },
			  { "atan", "keyword" },
			  { "sinh", "keyword" },
			  { "cosh", "keyword" },
			  { "tanh", "keyword" },
			  { "asinh", "keyword" },
			  { "acosh", "keyword" },
			  { "pow", "keyword" },
			  { "exp", "keyword" },
			  { "exp2", "keyword" },
			  { "log2", "keyword" },
			  { "sqrt", "keyword" },
			  { "inversesqrt", "keyword" },
			  { "abs", "keyword" },
			  { "sign", "keyword" },
			  { "floor", "keyword" },
			  { "trunc", "keyword" },
			  { "round", "keyword" },
			  { "roundEven", "keyword" },
			  { "ceil", "keyword" },
			  { "fract", "keyword" },
			  { "mod", "keyword" },
			  { "modf", "keyword" },
			  { "min", "keyword" },
			  { "max", "keyword" },
			  { "clamp", "keyword" },
			  { "mix", "keyword" },
			  { "step", "keyword" },
			  { "smoothstep", "keyword" },
			  { "isnan", "keyword" },
			  { "isinf", "keyword" },
			  { "floatBitsToInt", "keyword" },
			  { "floatBitsToUint", "keyword" },
			  { "intBitsToFloat", "keyword" },
			  { "uintBitsToFloat", "keyword" },
			  { "fma", "keyword" },
			  { "frexp", "keyword" },
			  { "ldexp", "keyword" },
			  { "packUnorm2x16", "keyword" },
			  { "packSnorm2x16", "keyword" },
			  { "packUnorm4x8", "keyword" },
			  { "packSnorm4x8", "keyword" },
			  { "unpackUnorm2x16", "keyword" },
			  { "unpackSnorm2x16", "keyword" },
			  { "unpackUnorm4x8", "keyword" },
			  { "unpackSnorm4x8", "keyword" },
			  { "packHalf2x16", "keyword" },
			  { "unpackHalf2x16", "keyword" },
			  { "packDouble2x32", "keyword" },
			  { "unpackDouble2x32", "keyword" },
			  { "length", "keyword" },
			  { "distance", "keyword" },
			  { "dot", "keyword" },
			  { "cross", "keyword" },
			  { "normalize", "keyword" },
			  { "ftransform", "keyword" },
			  { "faceforward", "keyword" },
			  { "reflect", "keyword" },
			  { "refract", "keyword" },
			  { "matrixCompMult", "keyword" },
			  { "outerProduct", "keyword" },
			  { "transpose", "keyword" },
			  { "determinant", "keyword" },
			  { "inverse", "keyword" },
			  { "lessThan", "keyword" },
			  { "lessThanEqual", "keyword" },
			  { "greaterThan", "keyword" },
			  { "greaterThanEqual", "keyword" },
			  { "equal", "keyword" },
			  { "notEqual", "keyword" },
			  { "any", "keyword" },
			  { "all", "keyword" },
			  { "not", "keyword" },
			  { "uaddCarry", "keyword" },
			  { "usubBorrow", "keyword" },
			  { "umulExtended", "keyword" },
			  { "imulExtended", "keyword" },
			  { "bitfieldExtract", "keyword" },
			  { "bitfieldInsert", "keyword" },
			  { "bitfieldReverse", "keyword" },
			  { "bitCount", "keyword" },
			  { "findLSB", "keyword" },
			  { "findMSB", "keyword" },
			  { "textureSize", "keyword" },
			  { "textureQueryLod", "keyword" },
			  { "textureQueryLevels", "keyword" },
			  { "textureSamples", "keyword" },
			  { "texture", "keyword" },
			  { "textureProj", "keyword" },
			  { "textureLod", "keyword" },
			  { "textureOffset", "keyword" },
			  { "texelFetch", "keyword" },
			  { "texelFetchOffset", "keyword" },
			  { "textureProjOffset", "keyword" },
			  { "textureLodOffset", "keyword" },
			  { "textureProjLod", "keyword" },
			  { "textureProjLodOffset", "keyword" },
			  { "textureGrad", "keyword" },
			  { "textureGradOffset", "keyword" },
			  { "textureProjGrad", "keyword" },
			  { "textureProjGradOffset", "keyword" },
			  { "textureGather", "keyword" },
			  { "textureGatherOffset", "keyword" },
			  { "textureGatherOffsets", "keyword" },
			  { "atomicCounterIncrement", "keyword" },
			  { "atomicCounterDecrement", "keyword" },
			  { "atomicCounter", "keyword" },
			  { "atomicCounterAdd", "keyword" },
			  { "atomicCounterSubtract", "keyword" },
			  { "atomicCounterMin", "keyword" },
			  { "atomicCounterMax", "keyword" },
			  { "atomicCounterAnd", "keyword" },
			  { "atomicCounterOr", "keyword" },
			  { "atomicCounterXor", "keyword" },
			  { "atomicCounterExchange", "keyword" },
			  { "atomicCounterCompSwap", "keyword" },
			  { "atomicAdd", "keyword" },
			  { "atomicMin", "keyword" },
			  { "atomicMax", "keyword" },
			  { "atomicAnd", "keyword" },
			  { "atomicOr", "keyword" },
			  { "atomicXor", "keyword" },
			  { "atomicExchange", "keyword" },
			  { "atomicCompSwap", "keyword" },
			  { "imageSize", "keyword" },
			  { "imageSamples", "keyword" },
			  { "imageLoad", "keyword" },
			  { "imageStore", "keyword" },
			  { "imageAtomicAdd", "keyword" },
			  { "imageAtomicMin", "keyword" },
			  { "imageAtomicMax", "keyword" },
			  { "imageAtomicAnd", "keyword" },
			  { "imageAtomicOr", "keyword" },
			  { "imageAtomicXor", "keyword" },
			  { "imageAtomicExchange", "keyword" },
			  { "imageAtomicCompSwap", "keyword" },
			  { "EmitStreamVertex", "keyword" },
			  { "EndStreamPrimitive", "keyword" },
			  { "EmitVertex", "keyword" },
			  { "EndPrimitive", "keyword" },
			  { "dFdx", "keyword" },
			  { "dFdy", "keyword" },
			  { "dFdxFine", "keyword" },
			  { "dFdyFine", "keyword" },
			  { "dFdxCoarse", "keyword" },
			  { "dFdyCoarse", "keyword" },
			  { "fwidth", "keyword" },
			  { "fwidthFine", "keyword" },
			  { "fwidthCoarse", "keyword" },
			  { "interpolateAtCentroid", "keyword" },
			  { "interpolateAtSample", "keyword" },
			  { "interpolateAtOffset", "keyword" },
			  { "barrier", "keyword" },
			  { "memoryBarrier", "keyword" },
			  { "memoryBarrierAtomicCounter", "keyword" },
			  { "memoryBarrierBuffer", "keyword" },
			  { "memoryBarrierShared", "keyword" },
			  { "memoryBarrierImage", "keyword" },
			  { "groupMemoryBarrier", "keyword" },
			  { "subpassLoad", "keyword" },
			  { "anyInvocation", "keyword" },
			  { "allInvocations", "keyword" },
			  { "allInvocationsEqual", "keyword" },
			  { "texture1D", "keyword" },
			  { "texture1DArray", "keyword" },
			  { "itexture1D", "keyword" },
			  { "itexture1DArray", "keyword" },
			  { "utexture1D", "keyword" },
			  { "utexture1DArray", "keyword" },
			  { "texture2D", "keyword" },
			  { "texture2DArray", "keyword" },
			  { "itexture2D", "keyword" },
			  { "itexture2DArray", "keyword" },
			  { "utexture2D", "keyword" },
			  { "utexture2DArray", "keyword" },
			  { "texture2DRect", "keyword" },
			  { "itexture2DRect", "keyword" },
			  { "utexture2DRect", "keyword" },
			  { "texture2DMS", "keyword" },
			  { "itexture2DMS", "keyword" },
			  { "utexture2DMS", "keyword" },
			  { "texture2DMSArray", "keyword" },
			  { "itexture2DMSArray", "keyword" },
			  { "utexture2DMSArray", "keyword" },
			  { "texture3D", "keyword" },
			  { "itexture3D", "keyword" },
			  { "utexture3D", "keyword" },
			  { "textureCube", "keyword" },
			  { "itextureCube", "keyword" },
			  { "utextureCube", "keyword" },
			  { "textureCubeArray", "keyword" },
			  { "itextureCubeArray", "keyword" },
			  { "utextureCubeArray", "keyword" },
			  { "textureBuffer", "keyword" },
			  { "itextureBuffer", "keyword" },
			  { "utextureBuffer", "keyword" },
			  { "sampler", "keyword2" },
			  { "samplerShadow", "keyword2" },
			  { "subpassInput", "keyword2" },
			  { "isubpassInput", "keyword2" },
			  { "usubpassInput", "keyword2" },
			  { "subpassInputMS", "keyword2" },
			  { "isubpassInputMS", "keyword2" },
			  { "usubpassInputMS", "keyword2" },
		  },
		  "//" } );
}

static void addIni() {
	SyntaxDefinitionManager::instance()->add(
		{ "Config File",
		  { "%.ini$", "%.conf$", "%.desktop$", "%.service$", "%.cfg$", "%.properties$",
			"Doxyfile" },
		  { { { "%s?#%x%x%x%x%x%x%x%x" }, "string" },
			{ { "%s?#%x%x%x%x%x%x" }, "string" },
			{ { "^#.-\n" }, "comment" },
			{ { "^;.-\n" }, "comment" },
			{ { "%s#.-\n" }, "comment" },
			{ { "[%a_][%w-+_%s%p]-%f[=]" }, "keyword" },
			{ { "\"", "\"", "\\" }, "string" },
			{ { "'", "'", "\\" }, "string" },
			{ { "^%[.-%]" }, "keyword2" },
			{ { "%s%[.-%]" }, "keyword2" },
			{ { "=" }, "operator" },
			{ { "https?://[%w_.~!*:@&+$/?%%#-]-%w[-.%w]*%.%w%w%w?%w?:?%d*/?[%w_.~!*:@&+$/"
				"?%%#=-]*" },
			  "link" },
			{ { "[a-z]+" }, "symbol" } },
		  { { "true", "literal" }, { "false", "literal" } },
		  "#",
		  { "^%[.-%]%f[^\n]" },
		  "ini" } );
}

static void addMakefile() {
	SyntaxDefinitionManager::instance()->add( { "Makefile",
												{ "Makefile", "makefile", "%.mk$", "%.make$" },
												{
													{ { "#.*\n" }, "comment" },
													{ { "[[.]]}" }, "normal" },
													{ { "$[@^<%%?+|*]" }, "keyword2" },
													{ { "$%(", "%)" }, "keyword" },
													{ { "%f[%w_][%d%.]+%f[^%w_]" }, "number" },
													{ { "%..*:" }, "keyword2" },
													{ { ".*:=" }, "function" },
													{ { ".*+=" }, "function" },
													{ { ".*%s=" }, "function" },
												},
												{},
												"#" } );
}

static void addCSharp() {
	SyntaxDefinitionManager::instance()->add(
		{ "C#",
		  { "%.cs$", "%.csx$" },
		  {
			  { { "//.-\n" }, "comment" },
			  { { "/%*", "%*/" }, "comment" },
			  { { "\"", "\"", "\\" }, "string" },
			  { { "[%$%@]?\"", "\"", "\\" }, "string" },
			  { { "'\\x%x?%x?%x?%x'" }, "string" },
			  { { "'\\u%x%x%x%x'" }, "string" },
			  { { "'\\?.'" }, "string" },
			  { { "-?0x%x+" }, "number" },
			  { { "-?%d+[%d%.eE]*f?" }, "number" },
			  { { "-?%.?%d+f?" }, "number" },
			  { { "[%+%-=/%*%^%%<>!~|&]" }, "operator" },
			  { { "%?%?" }, "operator" },
			  { { "%?%." }, "operator" },
			  { { "[%a_][%w_]*%f[(]" }, "function" },
			  { { "[%a_][%w_]*" }, "symbol" },
		  },
		  { { "abstract", "keyword" },	{ "as", "keyword" },		{ "await", "keyword" },
			{ "base", "keyword" },		{ "break", "keyword" },		{ "case", "keyword" },
			{ "catch", "keyword" },		{ "checked", "keyword" },	{ "class", "keyword" },
			{ "const", "keyword" },		{ "continue", "keyword" },	{ "default", "keyword" },
			{ "delegate", "keyword" },	{ "do", "keyword" },		{ "else", "keyword" },
			{ "enum", "keyword" },		{ "event", "keyword" },		{ "explicit", "keyword" },
			{ "extern", "keyword" },	{ "finally", "keyword" },	{ "fixed", "keyword" },
			{ "for", "keyword" },		{ "foreach", "keyword" },	{ "get", "keyword" },
			{ "goto", "keyword" },		{ "if", "keyword" },		{ "implicit", "keyword" },
			{ "in", "keyword" },		{ "interface", "keyword" }, { "internal", "keyword" },
			{ "is", "keyword" },		{ "lock", "keyword" },		{ "namespace", "keyword" },
			{ "new", "keyword" },		{ "operator", "keyword" },	{ "out", "keyword" },
			{ "override", "keyword" },	{ "params", "keyword" },	{ "private", "keyword" },
			{ "protected", "keyword" }, { "public", "keyword" },	{ "readonly", "keyword" },
			{ "ref", "keyword" },		{ "return", "keyword" },	{ "sealed", "keyword" },
			{ "set", "keyword" },		{ "sizeof", "keyword" },	{ "stackalloc", "keyword" },
			{ "static", "keyword" },	{ "struct", "keyword" },	{ "switch", "keyword" },
			{ "this", "keyword" },		{ "throw", "keyword" },		{ "try", "keyword" },
			{ "typeof", "keyword" },	{ "unchecked", "keyword" }, { "unsafe", "keyword" },
			{ "using", "keyword" },		{ "var", "keyword" },		{ "virtual", "keyword" },
			{ "void", "keyword" },		{ "volatile", "keyword" },	{ "where", "keyword" },
			{ "while", "keyword" },		{ "yield", "keyword" },		{ "bool", "keyword2" },
			{ "byte", "keyword2" },		{ "char", "keyword2" },		{ "decimal", "keyword2" },
			{ "double", "keyword2" },	{ "float", "keyword2" },	{ "int", "keyword2" },
			{ "long", "keyword2" },		{ "object", "keyword2" },	{ "sbyte", "keyword2" },
			{ "short", "keyword2" },	{ "string", "keyword2" },	{ "uint", "keyword2" },
			{ "ulong", "keyword2" },	{ "ushort", "keyword2" },	{ "true", "literal" },
			{ "false", "literal" },		{ "null", "literal" },		{ "add", "keyword" },
			{ "record", "keyword" },	{ "remove", "keyword" },	{ "partial", "keyword" },
			{ "dynamic", "keyword" },	{ "value", "keyword" },		{ "global", "keyword" },
			{ "when", "keyword" } },
		  "//",
		  {},
		  "csharp" } );
}

static void addGo() {
	SyntaxDefinitionManager::instance()->add(
		{ "Go",
		  { "%.go$" },
		  {
			  { { "//.-\n" }, "comment" },
			  { { "/%*", "%*/" }, "comment" },
			  { { "\"", "\"", "\\" }, "string" },
			  { { "`", "`", "\\" }, "string" },
			  { { "0[oO_][0-7]+" }, "number" },
			  { { "-?0x[%x_]+" }, "number" },
			  { { "-?%d+_%d" }, "number" },
			  { { "-?%d+[%d%.eE]*f?" }, "number" },
			  { { "-?%.?%d+f?" }, "number" },
			  { { "[%+%-=/%*%^%%<>!~|&]" }, "operator" },
			  { { ":=" }, "operator" },
			  { { "[%a_][%w_]*%f[(]" }, "function" },
			  { { "[%a_][%w_]*" }, "symbol" },
		  },
		  {
			  { "if", "keyword" },		 { "else", "keyword" },		  { "elseif", "keyword" },
			  { "for", "keyword" },		 { "continue", "keyword" },	  { "return", "keyword" },
			  { "struct", "keyword" },	 { "switch", "keyword" },	  { "case", "keyword" },
			  { "default", "keyword" },	 { "const", "keyword" },	  { "package", "keyword" },
			  { "import", "keyword" },	 { "func", "keyword" },		  { "var", "keyword" },
			  { "type", "keyword" },	 { "interface", "keyword" },  { "select", "keyword" },
			  { "break", "keyword" },	 { "range", "keyword" },	  { "chan", "keyword" },
			  { "defer", "keyword" },	 { "go", "keyword" },		  { "fallthrough", "keyword" },
			  { "int", "keyword2" },	 { "int64", "keyword2" },	  { "int32", "keyword2" },
			  { "int16", "keyword2" },	 { "int8", "keyword2" },	  { "uint", "keyword2" },
			  { "uint64", "keyword2" },	 { "uint32", "keyword2" },	  { "uint16", "keyword2" },
			  { "uint8", "keyword2" },	 { "uintptr", "keyword2" },	  { "float64", "keyword2" },
			  { "float32", "keyword2" }, { "map", "keyword2" },		  { "string", "keyword2" },
			  { "rune", "keyword2" },	 { "bool", "keyword2" },	  { "byte", "keyword2" },
			  { "error", "keyword2" },	 { "complex64", "keyword2" }, { "complex128", "keyword2" },
			  { "true", "literal" },	 { "false", "literal" },	  { "nil", "literal" },
		  },
		  "//" } );
}

static void addRust() {
	SyntaxDefinitionManager::instance()->add(
		{ "Rust",
		  { "%.rs$" },
		  {
			  { { "//.-\n" }, "comment" },
			  { { "/%*", "%*/" }, "comment" },
			  { { "r#\"", "\"#", "\\" }, "string" },
			  { { "\"", "\"", "\\" }, "string" },
			  { { "`", "`", "\\" }, "string" },
			  { { "'.'" }, "string" },
			  { { "'%a+" }, "keyword2" },
			  { { "0[oO_][0-7]+" }, "number" },
			  { { "-?0x[%x_]+" }, "number" },
			  { { "-?%d+_%d" }, "number" },
			  { { "-?%d+[%d%.eE]*f?" }, "number" },
			  { { "-?%.?%d+f?" }, "number" },
			  { { "[%+%-=/%*%^%%<>!~|&]" }, "operator" },
			  { { "[%a_][%w_]*%f[(]" }, "function" },
			  { { "[%a_][%w_]*" }, "symbol" },
		  },
		  {
			  { "as", "keyword" },	   { "async", "keyword" },	 { "await", "keyword" },
			  { "break", "keyword" },  { "const", "keyword" },	 { "continue", "keyword" },
			  { "crate", "keyword" },  { "dyn", "keyword" },	 { "else", "keyword" },
			  { "enum", "keyword" },   { "extern", "keyword" },	 { "false", "keyword" },
			  { "fn", "keyword" },	   { "for", "keyword" },	 { "if", "keyword" },
			  { "impl", "keyword" },   { "in", "keyword" },		 { "let", "keyword" },
			  { "loop", "keyword" },   { "match", "keyword" },	 { "mod", "keyword" },
			  { "move", "keyword" },   { "mut", "keyword" },	 { "pub", "keyword" },
			  { "ref", "keyword" },	   { "return", "keyword" },	 { "Self", "keyword" },
			  { "self", "keyword" },   { "static", "keyword" },	 { "struct", "keyword" },
			  { "super", "keyword" },  { "trait", "keyword" },	 { "true", "keyword" },
			  { "type", "keyword" },   { "unsafe", "keyword" },	 { "use", "keyword" },
			  { "where", "keyword" },  { "while", "keyword" },	 { "i32", "keyword2" },
			  { "i64", "keyword2" },   { "i128", "keyword2" },	 { "i16", "keyword2" },
			  { "i8", "keyword2" },	   { "u8", "keyword2" },	 { "u16", "keyword2" },
			  { "u32", "keyword2" },   { "u64", "keyword2" },	 { "usize", "keyword2" },
			  { "isize", "keyword2" }, { "f32", "keyword2" },	 { "f64", "keyword2" },
			  { "f128", "keyword2" },  { "String", "keyword2" }, { "char", "keyword2" },
			  { "&str", "keyword2" },  { "bool", "keyword2" },	 { "true", "literal" },
			  { "false", "literal" },  { "None", "literal" },	 { "Some", "literal" },
			  { "Option", "literal" }, { "Result", "literal" },
		  },
		  "//" } );
}

static void addGDScript() {
	SyntaxDefinitionManager::instance()->add(

		{ "GDScript",
		  { "%.gd$" },
		  {
			  { { "#.-\n" }, "comment" },
			  { { "\"", "\"", "\\" }, "string" },
			  { { "'", "'", "\\" }, "string" },
			  { { "-?0x%x*" }, "number" },
			  { { "-?%d+[%d%.e]*" }, "number" },
			  { { "-?%.?%d+" }, "number" },
			  { { "[%+%:%-=/%*%^%%<>!~|&]" }, "operator" },
			  { { "[%a_][%w_]*%f[(]" }, "function" },
			  { { "[%a_][%w_]*" }, "symbol" },
		  },
		  {
			  { "if", "keyword" },
			  { "elif", "keyword" },
			  { "else", "keyword" },
			  { "for", "keyword" },
			  { "while", "keyword" },
			  { "match", "keyword" },
			  { "break", "keyword" },
			  { "continue", "keyword" },
			  { "pass", "keyword" },
			  { "return", "keyword" },
			  { "class", "keyword" },
			  { "class_name", "keyword" },
			  { "extends", "keyword" },
			  { "is", "keyword" },
			  { "in", "keyword" },
			  { "as", "keyword" },
			  { "and", "keyword" },
			  { "or", "keyword" },
			  { "not", "keyword" },
			  { "self", "keyword" },
			  { "tool", "keyword" },
			  { "signal", "keyword" },
			  { "func", "keyword" },
			  { "static", "keyword" },
			  { "const", "keyword" },
			  { "enum", "keyword" },
			  { "var", "keyword" },
			  { "onready", "keyword" },
			  { "export", "keyword" },
			  { "setget", "keyword" },
			  { "breakpoint", "keyword" },
			  { "preload", "keyword" },
			  { "yield", "keyword" },
			  { "assert", "keyword" },
			  { "remote", "keyword" },
			  { "master", "keyword" },
			  { "puppet", "keyword" },
			  { "remotesync", "keyword" },
			  { "mastersync", "keyword" },
			  { "puppetsync", "keyword" },
			  { "void", "keyword2" },
			  { "int", "keyword2" },
			  { "float", "keyword2" },
			  { "bool", "keyword2" },
			  { "String", "keyword2" },
			  { "Vector2", "keyword2" },
			  { "Rect2", "keyword2" },
			  { "Vector3", "keyword2" },
			  { "Transform2D", "keyword2" },
			  { "Plane", "keyword2" },
			  { "Quat", "keyword2" },
			  { "AABB", "keyword2" },
			  { "Basis", "keyword2" },
			  { "Transform", "keyword2" },
			  { "Color", "keyword2" },
			  { "NodePath", "keyword2" },
			  { "RID", "keyword2" },
			  { "Object", "keyword2" },
			  { "Array", "keyword2" },
			  { "PoolByteArray", "keyword2" },
			  { "PoolIntArray", "keyword2" },
			  { "PoolRealArray", "keyword2" },
			  { "PoolStringArray", "keyword2" },
			  { "PoolVector2Array", "keyword2" },
			  { "PoolVector3Array", "keyword2" },
			  { "PoolColorArray", "keyword2" },
			  { "Dictionary", "keyword2" },
			  { "null", "literal" },
			  { "true", "literal" },
			  { "false", "literal" },
			  { "PI", "literal" },
			  { "TAU", "literal" },
			  { "INF", "literal" },
			  { "NAN", "literal" },
		  },
		  "#" } );
}

static void addD() {
	SyntaxDefinitionManager::instance()->add(

		{ "D",
		  { "%.d$", "%.di$" },
		  {
			  { { "//.-\n" }, "comment" },
			  { { "/%*", "%*/" }, "comment" },
			  { { "/%+", "%+/" }, "comment" },
			  { { "`", "`", "\\" }, "string" },
			  { { "\"", "\"", "\\" }, "string" },
			  { { "'", "'", "\\" }, "string" },
			  { { "-?0x%x+" }, "number" },
			  { { "-?%d+[%d%.eE]*f?" }, "number" },
			  { { "-?%.?%d+f?" }, "number" },
			  { { "[%+%-=/%*%^%%<>!~|&%$]+" }, "operator" },
			  { { "[%a_][%w_]*!" }, "function" },
			  { { "[%a_][%w_]*" }, "symbol" },
			  { { "@safe" }, "keyword" },
			  { { "@trusted" }, "keyword" },
			  { { "@nogc" }, "keyword" },
		  },
		  {
			  { "abstract", "keyword" },
			  { "alias", "keyword" },
			  { "align", "keyword" },
			  { "asm", "keyword" },
			  { "assert", "keyword" },
			  { "auto", "keyword" },
			  { "body", "keyword" },
			  { "bool", "keyword2" },
			  { "break", "keyword" },
			  { "byte", "keyword2" },
			  { "case", "keyword" },
			  { "cast", "keyword" },
			  { "catch", "keyword" },
			  { "cdouble", "keyword2" },
			  { "cent", "keyword2" },
			  { "cfloat", "keyword2" },
			  { "char", "keyword2" },
			  { "class", "keyword" },
			  { "const", "keyword" },
			  { "continue", "keyword" },
			  { "creal", "keyword2" },
			  { "dchar", "keyword2" },
			  { "debug", "keyword" },
			  { "default", "keyword" },
			  { "delegate", "keyword" },
			  { "deprecated", "keyword" },
			  { "do", "keyword" },
			  { "double", "keyword2" },
			  { "else", "keyword" },
			  { "enum", "keyword" },
			  { "export", "keyword" },
			  { "extern", "keyword" },
			  { "false", "literal" },
			  { "final", "keyword" },
			  { "finally", "keyword" },
			  { "float", "keyword2" },
			  { "for", "keyword" },
			  { "foreach", "keyword" },
			  { "foreach_reverse", "keyword" },
			  { "function", "keyword" },
			  { "goto", "keyword" },
			  { "idouble", "keyword2" },
			  { "if", "keyword" },
			  { "ifloat", "keyword2" },
			  { "immutable", "keyword" },
			  { "import", "keyword" },
			  { "in", "keyword" },
			  { "inout", "keyword" },
			  { "int", "keyword2" },
			  { "interface", "keyword" },
			  { "invariant", "keyword" },
			  { "ireal", "keyword2" },
			  { "is", "keyword" },
			  { "lazy", "keyword" },
			  { "long", "keyword2" },
			  { "macro", "keyword" },
			  { "mixin", "keyword" },
			  { "module", "keyword" },
			  { "new", "keyword" },
			  { "nothrow", "keyword" },
			  { "null", "literal" },
			  { "out", "keyword" },
			  { "override", "keyword" },
			  { "package", "keyword" },
			  { "pragma", "keyword" },
			  { "private", "keyword" },
			  { "protected", "keyword" },
			  { "public", "keyword" },
			  { "pure", "keyword" },
			  { "real", "keyword2" },
			  { "ref", "keyword" },
			  { "return", "keyword" },
			  { "scope", "keyword" },
			  { "shared", "keyword" },
			  { "short", "keyword2" },
			  { "static", "keyword" },
			  { "struct", "keyword" },
			  { "super", "keyword" },
			  { "switch", "keyword" },
			  { "synchronized", "keyword" },
			  { "template", "keyword" },
			  { "this", "keyword" },
			  { "throw", "keyword" },
			  { "true", "literal" },
			  { "try", "keyword" },
			  { "typeid", "keyword" },
			  { "typeof", "keyword" },
			  { "ubyte", "keyword2" },
			  { "ucent", "keyword2" },
			  { "uint", "keyword2" },
			  { "ulong", "keyword2" },
			  { "union", "keyword" },
			  { "unittest", "keyword" },
			  { "ushort", "keyword2" },
			  { "version", "keyword" },
			  { "void", "keyword" },
			  { "wchar", "keyword2" },
			  { "while", "keyword" },
			  { "with", "keyword" },
			  { "__FILE__", "keyword" },
			  { "__FILE_FULL_PATH__", "keyword" },
			  { "__MODULE__", "keyword" },
			  { "__LINE__", "keyword" },
			  { "__FUNCTION__", "keyword" },
			  { "__PRETTY_FUNCTION__", "keyword" },
			  { "__gshared", "keyword" },
			  { "__traits", "keyword" },
			  { "__vector", "keyword" },
			  { "__parameters", "keyword" },
		  },
		  "//" } );
}

static void addHaskell() {
	SyntaxDefinitionManager::instance()->add(
		{ "Haskell",
		  { "%.hs$" },
		  {
			  { { "%-%-", "\n" }, "comment" },
			  { { "{%-", "%-}" }, "comment" },
			  { { "\"", "\"", "\\" }, "string" },
			  { { "'", "'", "\\" }, "string" },
			  { { "-?0x%x+" }, "number" },
			  { { "-?%d+[%d%.eE]*f?" }, "number" },
			  { { "-?%.?%d+f?" }, "number" },
			  { { "[!%#%$%%&*+./%<=>%?@\\%^|%-~:]" }, "operator" },
			  { { "[%a_'][%w_']*" }, "symbol" },
		  },
		  {
			  { "as", "keyword" },		 { "case", "keyword" },	   { "of", "keyword" },
			  { "class", "keyword" },	 { "data", "keyword" },	   { "default", "keyword" },
			  { "deriving", "keyword" }, { "do", "keyword" },	   { "forall", "keyword" },
			  { "foreign", "keyword" },	 { "hiding", "keyword" },  { "if", "keyword" },
			  { "then", "keyword" },	 { "else", "keyword" },	   { "import", "keyword" },
			  { "infix", "keyword" },	 { "infixl", "keyword" },  { "infixr", "keyword" },
			  { "let", "keyword" },		 { "in", "keyword" },	   { "mdo", "keyword" },
			  { "module", "keyword" },	 { "newtype", "keyword" }, { "qualified", "keyword" },
			  { "type", "keyword" },	 { "where", "keyword" },
		  },
		  "%-%-" } );
}

static void addLatex() {
	SyntaxDefinitionManager::instance()->add( { "LaTeX",
												{ "%.tex$" },
												{
													{ { "%%", "\n" }, "comment" },
													{ { "&" }, "operator" },
													{ { "\\\\" }, "operator" },
													{ { "%$", "%$" }, "operator" },
													{ { "\\%[", "\\]" }, "operator" },
													{ { "{", "}" }, "keyword" },
													{ { "\\%w*" }, "keyword2" },
												},
												{},
												"%%" } );
}

static void addMeson() {
	SyntaxDefinitionManager::instance()->add( { "Meson",
												{ "meson.build$" },
												{
													{ { "#", "\n" }, "comment" },
													{ { "\"", "\"", "\\" }, "string" },
													{ { "'", "'", "\\" }, "string" },
													{ { "'''", "'''" }, "string" },
													{ { "0x[%da-fA-F]+" }, "number" },
													{ { "-?%d+%d*" }, "number" },
													{ { "[%+%-=/%%%*!]" }, "operator" },
													{ { "[%a_][%w_]*%f[(]" }, "function" },
													{ { "[%a_][%w_]*" }, "symbol" },
												},
												{
													{ "if", "keyword" },
													{ "then", "keyword" },
													{ "else", "keyword" },
													{ "elif", "keyword" },
													{ "endif", "keyword" },
													{ "foreach", "keyword" },
													{ "endforeach", "keyword" },
													{ "break", "keyword" },
													{ "continue", "keyword" },
													{ "and", "keyword" },
													{ "not", "keyword" },
													{ "or", "keyword" },
													{ "in", "keyword" },
													{ "true", "literal" },
													{ "false", "literal" },
												},
												"#" } );
}

static void addAngelScript() {
	SyntaxDefinitionManager::instance()->add(
		{ "AlgelScript",
		  { "%.as$", "%.asc$" },
		  {
			  { { "//.-\n" }, "comment" },
			  { { "/%*", "%*/" }, "comment" },
			  { { "#", "[^\\]\n" }, "comment" },
			  { { "\"", "\"", "\\" }, "string" },
			  { { "'", "'", "\\" }, "string" },
			  { { "-?0[xX]%x+" }, "number" },
			  { { "-?0[bB][0-1]+" }, "number" },
			  { { "-?0[oO][0-7]+" }, "number" },
			  { { "-?%d+[%d%.eE]*f?" }, "number" },
			  { { "-?%.?%d+f?" }, "number" },
			  { { "&inout" }, "keyword" },
			  { { "&in" }, "keyword" },
			  { { "&out" }, "keyword" },
			  { { "[%a_][%w_]*@" }, "keyword2" },
			  { { "[%-%+!~@%?:&|%^<>%*/=%%]" }, "operator" },
			  { { "[%a_][%w_]*%f[(]" }, "function" },
			  { { "[%a_][%w_]*" }, "symbol" },
		  },
		  {
			  { "shared", "keyword" },	  { "external", "keyword" },  { "private", "keyword" },
			  { "protected", "keyword" }, { "const", "keyword" },	  { "final", "keyword" },
			  { "abstract", "keyword" },  { "class", "keyword" },	  { "typedef", "keyword" },
			  { "namespace", "keyword" }, { "interface", "keyword" }, { "import", "keyword" },
			  { "enum", "keyword" },	  { "funcdef", "keyword" },	  { "get", "keyword" },
			  { "set", "keyword" },		  { "mixin", "keyword" },	  { "void", "keyword2" },
			  { "int", "keyword2" },	  { "int8", "keyword2" },	  { "int16", "keyword2" },
			  { "int32", "keyword2" },	  { "int64", "keyword2" },	  { "uint", "keyword2" },
			  { "uint8", "keyword2" },	  { "uint16", "keyword2" },	  { "uint32", "keyword2" },
			  { "uint64", "keyword2" },	  { "float", "keyword2" },	  { "double", "keyword2" },
			  { "bool", "keyword2" },	  { "auto", "keyword" },	  { "override", "keyword" },
			  { "explicit", "keyword" },  { "property", "keyword" },  { "break", "keyword" },
			  { "continue", "keyword" },  { "return", "keyword" },	  { "switch", "keyword" },
			  { "case", "keyword" },	  { "default", "keyword" },	  { "for", "keyword" },
			  { "while", "keyword" },	  { "do", "keyword" },		  { "if", "keyword" },
			  { "else", "keyword" },	  { "try", "keyword" },		  { "catch", "keyword" },
			  { "cast", "keyword" },	  { "function", "keyword" },  { "true", "literal" },
			  { "false", "literal" },	  { "null", "literal" },	  { "is", "operator" },
			  { "and", "operator" },	  { "or", "operator" },		  { "xor", "operator" },
		  },
		  "//" } );
}

static void addBatchScript() {
	std::unordered_map<std::string, std::vector<std::string>> batchSymTable = {
		{ "keyword",
		  {
			  "if",			"else",			 "elsif", "not",  "for",	"do",	"in",	 "equ",
			  "neq",		"lss",			 "leq",	  "gtr",  "geq",	"nul",	"con",	 "prn",
			  "prn",		"lpt1",			 "com1",  "com2", "com3",	"com4", "exist", "defined",
			  "errorlevel", "cmdextversion", "goto",  "call", "verify",
		  } },
		{ "function",
		  {
			  "set",	"setlocal", "endlocal", "enabledelayedexpansion",
			  "echo",	"type",		"cd",		"chdir",
			  "md",		"mkdir",	"pause",	"choice",
			  "exit",	"del",		"rd",		"rmdir",
			  "copy",	"xcopy",	"move",		"ren",
			  "find",	"findstr",	"sort",		"shift",
			  "attrib", "cmd",		"command",	"forfiles",
		  } } };

	auto prepareBatchSymbols = []( std::unordered_map<std::string, std::vector<std::string>> table )
		-> std::unordered_map<std::string, std::string> {
		std::unordered_map<std::string, std::string> symbols;
		for ( auto& el : table ) {
			for ( const auto& it : el.second ) {
				symbols[String::toLower( it )] = el.first;
				symbols[String::toUpper( it )] = el.first;
			}
		}
		return symbols;
	};

	SyntaxDefinitionManager::instance()->add( { "Batch Script",
												{ "%.bat$", "%.cmd$" },
												{
													{ { "@echo off\n" }, "keyword" },
													{ { "@echo on\n" }, "keyword" },
													{ { "rem.-\n" }, "comment" },
													{ { "REM.-\n" }, "comment" },
													{ { "%s*:[%w%-]+" }, "symbol" },
													{ { "%:%:.-\n" }, "comment" },
													{ { "%%%w+%%" }, "symbol" },
													{ { "%%%%?~?[%w:]+" }, "symbol" },
													{ { "[!=()%>&%^/\\@]" }, "operator" },
													{ { "-?%.?%d+f?" }, "number" },
													{ { "\"", "\"", "\\" }, "string" },
													{ { "[%a_][%w_]*" }, "normal" },
													{ { ":eof" }, "keyword" },
												},
												prepareBatchSymbols( batchSymTable ),
												"rem",
												{},
												"bat" } );
}

static void addDiff() {
	SyntaxDefinitionManager::instance()->add( { "Diff File",
												{ "%.diff$", "%.patch$" },
												{
													{ { "^%+%+%+%s.-\n" }, "keyword" },
													{ { "^%-%-%-%s.-\n" }, "keyword" },
													{ { "^diff%s.-\n" }, "string" },
													{ { "^index%s.-\n" }, "comment" },
													{ { "^@@.-\n" }, "number" },
													{ { "^%+.-\n" }, "function" },
													{ { "^%-.-\n" }, "keyword2" },
												},
												{},
												"",
												{},
												"diff" } );
}

static void addJava() {
	SyntaxDefinitionManager::instance()->add(
		{ "Java",
		  { "%.java$" },
		  {
			  { { "//.-\n" }, "comment" },
			  { { "/%*", "%*/" }, "comment" },
			  { { "\"", "\"", "\\" }, "string" },
			  { { "'", "'", "\\" }, "string" },
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
			  { "if", "keyword" },		 { "then", "keyword" },			{ "else", "keyword" },
			  { "elseif", "keyword" },	 { "do", "keyword" },			{ "while", "keyword" },
			  { "for", "keyword" },		 { "new", "keyword" },			{ "break", "keyword" },
			  { "continue", "keyword" }, { "return", "keyword" },		{ "goto", "keyword" },
			  { "class", "keyword" },	 { "implements", "keyword" },	{ "extends", "keyword" },
			  { "private", "keyword" },	 { "protected", "keyword" },	{ "public", "keyword" },
			  { "abstract", "keyword" }, { "interface", "keyword" },	{ "assert", "keyword" },
			  { "import", "keyword" },	 { "native", "keyword" },		{ "package", "keyword" },
			  { "super", "keyword" },	 { "synchronized", "keyword" }, { "instanceof", "keyword" },
			  { "enum", "keyword" },	 { "catch", "keyword" },		{ "throw", "keyword" },
			  { "throws", "keyword" },	 { "try", "keyword" },			{ "transient", "keyword" },
			  { "finally", "keyword" },	 { "static", "keyword" },		{ "volatile", "keyword" },
			  { "final", "keyword" },	 { "switch", "keyword" },		{ "case", "keyword" },
			  { "default", "keyword" },	 { "void", "keyword" },			{ "int", "keyword2" },
			  { "short", "keyword2" },	 { "byte", "keyword2" },		{ "long", "keyword2" },
			  { "float", "keyword2" },	 { "double", "keyword2" },		{ "char", "keyword2" },
			  { "boolean", "keyword2" }, { "true", "literal" },			{ "false", "literal" },
			  { "null", "literal" },
		  },
		  "//" } );
}

static void addYAML() {
	SyntaxDefinitionManager::instance()->add(
		{ "YAML",
		  { "%.yml$", "%.yaml$" },
		  {
			  { { "#", "\n" }, "comment" },
			  { { "\"", "\"", "\\" }, "string" },
			  { { "'", "'", "\\" }, "string" },
			  { { "%-?%.inf" }, "number" },
			  { { "%.NaN" }, "number" },
			  { { "(%&)(%g+)" }, { "keyword", "literal", "" } },
			  { { "!%g+" }, "keyword" },
			  { { "<<" }, "literal" },
			  { { "https?://[%w_.~!*:@&+$/?%%#-]-%w[-.%w]*%.%w%w%w?%w?:?%d*/?[%w_.~!*:@&+$/"
				  "?%%#=-]*" },
				"link" },
			  { { "([%s]%*)([%w%d_]+)" }, { "keyword", "keyword", "keyword2" } },
			  { { "(%*)([%w%d_]+)" }, { "keyword", "keyword", "literal" } },
			  { { "([%[%{])(%s*[%w%d]+%g+%s*)(:%s)" },
				{ "keyword", "operator", "operator", "keyword" } },
			  { { "([%s][%w%d]+%g+%s*)(:%s)" }, { "keyword", "keyword", "operator" } },
			  { { "([%w%d]+%g+%s*)(:%s)" }, { "keyword", "keyword", "operator" } },
			  { { "0%d+" }, "number" },
			  { { "0x%x+" }, "number" },
			  { { "[%+%-]?%d+[,%.eE:%+%d]*%d+" }, "number" },
			  { { "[%*%|%!>%%]" }, "keyword" },
			  { { "[%-:%?%*%{%}%[%]]" }, "operator" },
			  { { "([%d%a_][%g_]*)([%]%},])" }, { "string", "operator", "operator" } },
			  { { "[%d%a$/_][%g_]*" }, "string" },
		  },
		  { { "true", "number" }, { "false", "number" }, { "y", "number" }, { "n", "number" } },
		  "#" } );
}

static void addSwift() {
	SyntaxDefinitionManager::instance()->add(
		{ "Swift",
		  { "%.swift$" },
		  {
			  { { "//.-\n" }, "comment" },
			  { { "/%*", "%*/" }, "comment" },
			  { { "\"\"\"", "\"\"\"" }, "string" },
			  { { "\"", "\"", "\\" }, "string" },
			  { { "-?0x%x+" }, "number" },
			  { { "-?%d+[%d%.eE]*f?" }, "number" },
			  { { "-?%.?%d+f?" }, "number" },
			  { { "[%+%-=/%*%^%%<>!~|&]" }, "operator" },
			  { { "[%a_][%w_]*%f[(]" }, "function" },
			  { { "[%a_][%w_]*[?!]?" }, "symbol" },
		  },
		  {
			  { "#available", "keyword" },
			  { "#colorLiteral", "keyword" },
			  { "#column", "keyword" },
			  { "#else", "keyword" },
			  { "#elseif", "keyword" },
			  { "#endif", "keyword" },
			  { "#file", "keyword" },
			  { "#fileLiteral", "keyword" },
			  { "#function", "keyword" },
			  { "#if", "keyword" },
			  { "#imageLiteral", "keyword" },
			  { "#line", "keyword" },
			  { "#selector", "keyword" },
			  { "#sourceLocation", "keyword" },
			  { "_", "keyword" },
			  { "__COLUMN__", "keyword" },
			  { "__FILE__", "keyword" },
			  { "__FUNCTION__", "keyword" },
			  { "__LINE__", "keyword" },
			  { "Any", "keyword" },
			  { "as", "keyword" },
			  { "as!", "keyword" },
			  { "as?", "keyword" },
			  { "associatedtype", "keyword" },
			  { "associativity", "keyword" },
			  { "break", "keyword" },
			  { "case", "keyword" },
			  { "catch", "keyword" },
			  { "class", "keyword" },
			  { "continue", "keyword" },
			  { "convenience", "keyword" },
			  { "default", "keyword" },
			  { "defer", "keyword" },
			  { "deinit", "keyword" },
			  { "didSet", "keyword" },
			  { "do", "keyword" },
			  { "dynamic", "keyword" },
			  { "dynamicType", "keyword" },
			  { "else", "keyword" },
			  { "enum", "keyword" },
			  { "extension", "keyword" },
			  { "fallthrough", "keyword" },
			  { "false", "keyword" },
			  { "fileprivate", "keyword" },
			  { "final", "keyword" },
			  { "for", "keyword" },
			  { "func", "keyword" },
			  { "get", "keyword" },
			  { "guard", "keyword" },
			  { "if", "keyword" },
			  { "import", "keyword" },
			  { "in", "keyword" },
			  { "indirect", "keyword" },
			  { "infix", "keyword" },
			  { "init", "keyword" },
			  { "inout", "keyword" },
			  { "internal", "keyword" },
			  { "is", "keyword" },
			  { "lazy", "keyword" },
			  { "left", "keyword" },
			  { "let", "keyword" },
			  { "mutating", "keyword" },
			  { "nil", "keyword" },
			  { "none", "keyword" },
			  { "nonmutating", "keyword" },
			  { "open", "keyword" },
			  { "operator", "keyword" },
			  { "optional", "keyword" },
			  { "override", "keyword" },
			  { "postfix", "keyword" },
			  { "precedence", "keyword" },
			  { "prefix", "keyword" },
			  { "private", "keyword" },
			  { "protocol", "keyword" },
			  { "Protocol", "keyword" },
			  { "public", "keyword" },
			  { "repeat", "keyword" },
			  { "required", "keyword" },
			  { "rethrows", "keyword" },
			  { "return", "keyword" },
			  { "right", "keyword" },
			  { "self", "keyword" },
			  { "Self", "keyword" },
			  { "set", "keyword" },
			  { "static", "keyword" },
			  { "struct", "keyword" },
			  { "subscript", "keyword" },
			  { "super", "keyword" },
			  { "switch", "keyword" },
			  { "throw", "keyword" },
			  { "throws", "keyword" },
			  { "true", "keyword" },
			  { "try", "keyword" },
			  { "try!", "keyword" },
			  { "try?", "keyword" },
			  { "Type", "keyword" },
			  { "typealias", "keyword" },
			  { "unowned", "keyword" },
			  { "var", "keyword" },
			  { "weak", "keyword" },
			  { "where", "keyword" },
			  { "while", "keyword" },
			  { "willSet", "keyword" },
			  { "true", "literal" },
			  { "false", "literal" },
			  { "nil", "literal" },
			  { "abs", "keyword2" },
			  { "advance", "keyword2" },
			  { "alignof", "keyword2" },
			  { "alignofValue", "keyword2" },
			  { "anyGenerator", "keyword2" },
			  { "assert", "keyword2" },
			  { "assertionFailure", "keyword2" },
			  { "bridgeFromObjectiveC", "keyword2" },
			  { "bridgeFromObjectiveCUnconditional", "keyword2" },
			  { "bridgeToObjectiveC", "keyword2" },
			  { "bridgeToObjectiveCUnconditional", "keyword2" },
			  { "c", "keyword2" },
			  { "compactMap", "keyword2" },
			  { "contains", "keyword2" },
			  { "count", "keyword2" },
			  { "countElements", "keyword2" },
			  { "countLeadingZeros", "keyword2" },
			  { "debugPrint", "keyword2" },
			  { "debugPrintln", "keyword2" },
			  { "distance", "keyword2" },
			  { "dropFirst", "keyword2" },
			  { "dropLast", "keyword2" },
			  { "dump", "keyword2" },
			  { "encodeBitsAsWords", "keyword2" },
			  { "enumerate", "keyword2" },
			  { "equal", "keyword2" },
			  { "fatalError", "keyword2" },
			  { "filter", "keyword2" },
			  { "find", "keyword2" },
			  { "getBridgedObjectiveCType", "keyword2" },
			  { "getVaList", "keyword2" },
			  { "indices", "keyword2" },
			  { "insertionSort", "keyword2" },
			  { "isBridgedToObjectiveC", "keyword2" },
			  { "isBridgedVerbatimToObjectiveC", "keyword2" },
			  { "isUniquelyReferenced", "keyword2" },
			  { "isUniquelyReferencedNonObjC", "keyword2" },
			  { "join", "keyword2" },
			  { "lazy", "keyword2" },
			  { "lexicographicalCompare", "keyword2" },
			  { "map", "keyword2" },
			  { "max", "keyword2" },
			  { "maxElement", "keyword2" },
			  { "min", "keyword2" },
			  { "minElement", "keyword2" },
			  { "numericCast", "keyword2" },
			  { "overlaps", "keyword2" },
			  { "partition", "keyword2" },
			  { "posix", "keyword2" },
			  { "precondition", "keyword2" },
			  { "preconditionFailure", "keyword2" },
			  { "print", "keyword2" },
			  { "println", "keyword2" },
			  { "quickSort", "keyword2" },
			  { "readLine", "keyword2" },
			  { "reduce", "keyword2" },
			  { "reflect", "keyword2" },
			  { "reinterpretCast", "keyword2" },
			  { "reverse", "keyword2" },
			  { "roundUpToAlignment", "keyword2" },
			  { "sizeof", "keyword2" },
			  { "sizeofValue", "keyword2" },
			  { "sort", "keyword2" },
			  { "split", "keyword2" },
			  { "startsWith", "keyword2" },
			  { "stride", "keyword2" },
			  { "strideof", "keyword2" },
			  { "strideofValue", "keyword2" },
			  { "swap", "keyword2" },
			  { "toString", "keyword2" },
			  { "transcode", "keyword2" },
			  { "underestimateCount", "keyword2" },
			  { "unsafeAddressOf", "keyword2" },
			  { "unsafeBitCast", "keyword2" },
			  { "unsafeDowncast", "keyword2" },
			  { "unsafeUnwrap", "keyword2" },
			  { "unsafeReflect", "keyword2" },
			  { "withExtendedLifetime", "keyword2" },
			  { "withObjectAtPlusZero", "keyword2" },
			  { "withUnsafePointer", "keyword2" },
			  { "withUnsafePointerToObject", "keyword2" },
			  { "withUnsafeMutablePointer", "keyword2" },
			  { "withUnsafeMutablePointers", "keyword2" },
			  { "withUnsafePointer", "keyword2" },
			  { "withUnsafePointers", "keyword2" },
			  { "withVaList", "keyword2" },
			  { "zip", "keyword2" },
			  { "Int", "keyword2" },
			  { "Float", "keyword2" },
			  { "Double", "keyword2" },
			  { "String", "keyword2" },
			  { "Void", "keyword2" },
			  { "Bool", "keyword2" },
			  { "Array", "keyword2" },
			  { "Optional", "keyword2" },
		  },
		  "//" } );
}

static void addSolidity() {
	SyntaxDefinitionManager::instance()->add(
		{ "Solidity",
		  { "%.sol$" },
		  {
			  { { "//.-\n" }, "comment" },
			  { { "/%*", "%*/" }, "comment" },
			  { { "\"\"\"", "\"\"\"" }, "string" },
			  { { "\"", "\"", "\\" }, "string" },
			  { { "-?0x%x+" }, "number" },
			  { { "-?%d+[%d%.eE]*f?" }, "number" },
			  { { "-?%.?%d+f?" }, "number" },
			  { { "[%+%-=/%*%^%%<>!~|&]" }, "operator" },
			  { { "[%a_][%w_]*%f[(]" }, "function" },
			  { { "[%a_][%w_]*" }, "symbol" },
		  },
		  { { "if", "keyword" },		  { "else", "keyword" },
			{ "for", "keyword" },		  { "while", "keyword" },
			{ "do", "keyword" },		  { "break", "keyword" },
			{ "continue", "keyword" },	  { "throw", "keyword" },
			{ "returns", "keyword" },	  { "return", "keyword" },
			{ "function", "keyword" },	  { "private", "keyword2" },
			{ "public", "keyword2" },	  { "internal", "keyword2" },
			{ "external", "keyword2" },	  { "constant", "keyword2" },
			{ "immutable", "keyword2" },  { "pure", "keyword2" },
			{ "view", "keyword2" },		  { "payable", "keyword2" },
			{ "nonpayable", "keyword2" }, { "inherited", "keyword2" },
			{ "indexed", "keyword2" },	  { "storage", "keyword2" },
			{ "memory", "keyword2" },	  { "virtual", "keyword2" },
			{ "calldata", "keyword2" },	  { "override", "keyword2" },
			{ "abstract", "keyword" },	  { "library", "keyword" },
			{ "enum", "keyword" },		  { "constructor", "keyword" },
			{ "event", "keyword" },		  { "modifier", "keyword" },
			{ "assembly", "keyword" },	  { "let", "keyword" },
			{ "msg", "keyword2" },		  { "block", "keyword2" },
			{ "tx", "keyword2" },		  { "now", "keyword2" },
			{ "this", "keyword2" },		  { "super", "keyword2" },
			{ "require", "keyword" },	  { "assert", "keyword2" },
			{ "revert", "keyword2" },	  { "selfdestruct", "keyword2" },
			{ "suicide", "keyword2" },	  { "addmod", "keyword2" },
			{ "mulmod", "keyword2" },	  { "keccak256", "keyword2" },
			{ "sha256", "keyword2" },	  { "sha3", "keyword2" },
			{ "ripemd160", "keyword2" },  { "ecrecover", "keyword2" },
			{ "blockhash", "keyword2" },  { "gasleft", "keyword2" },
			{ "contract", "keyword" },	  { "interface", "keyword" },
			{ "modifier", "keyword" },	  { "mapping", "keyword2" },
			{ "address", "keyword2" },	  { "string", "keyword2" },
			{ "byte", "keyword2" },		  { "bytes", "keyword2" },
			{ "bytes1", "keyword2" },	  { "bytes2", "keyword2" },
			{ "bytes3", "keyword2" },	  { "bytes4", "keyword2" },
			{ "bytes5", "keyword2" },	  { "bytes6", "keyword2" },
			{ "bytes7", "keyword2" },	  { "bytes8", "keyword2" },
			{ "bytes9", "keyword2" },	  { "bytes10", "keyword2" },
			{ "bytes11", "keyword2" },	  { "bytes12", "keyword2" },
			{ "bytes13", "keyword2" },	  { "bytes14", "keyword2" },
			{ "bytes15", "keyword2" },	  { "bytes16", "keyword2" },
			{ "bytes17", "keyword2" },	  { "bytes18", "keyword2" },
			{ "bytes19", "keyword2" },	  { "bytes20", "keyword2" },
			{ "bytes21", "keyword2" },	  { "bytes22", "keyword2" },
			{ "bytes23", "keyword2" },	  { "bytes24", "keyword2" },
			{ "bytes25", "keyword2" },	  { "bytes26", "keyword2" },
			{ "bytes27", "keyword2" },	  { "bytes28", "keyword2" },
			{ "bytes29", "keyword2" },	  { "bytes30", "keyword2" },
			{ "bytes31", "keyword2" },	  { "bytes32", "keyword2" },
			{ "int", "keyword2" },		  { "int8", "keyword2" },
			{ "int16", "keyword2" },	  { "int32", "keyword2" },
			{ "int64", "keyword2" },	  { "int128", "keyword2" },
			{ "int256", "keyword2" },	  { "uint", "keyword2" },
			{ "uint8", "keyword2" },	  { "uint16", "keyword2" },
			{ "uint32", "keyword2" },	  { "uint64", "keyword2" },
			{ "uint128", "keyword2" },	  { "uint256", "keyword2" },
			{ "bool", "keyword2" },		  { "hash", "keyword2" },
			{ "pragma", "keyword2" },	  { "using", "keyword" },
			{ "is", "keyword" },		  { "new", "keyword" },
			{ "delete", "keyword" },	  { "emit", "keyword" },
			{ "struct", "keyword" },	  { "protected", "keyword" } } } );
}

static void addObjetiveC() {
	SyntaxDefinitionManager::instance()->add(
		{ "Objective-C",
		  { "%.m$" },
		  { { { "//.-\n" }, "comment" },
			{ { "/%*", "%*/" }, "comment" },
			{ { "#", "[^\\]\n" }, "comment" },
			{ { "\"", "\"", "\\" }, "string" },
			{ { "'", "'", "\\" }, "string" },
			{ { "-?0x%x+" }, "number" },
			{ { "-?%d+[%d%.eE]*f?" }, "number" },
			{ { "-?%.?%d+f?" }, "number" },
			{ { "[%+%-=/%*%^%%<>!~|&]" }, "operator" },
			{ { "[%a_][%w_]*%f[(]" }, "function" },
			{ { "@[%a_][%w_]*" }, "keyword2" },
			{ { "[%a_][%w_]*" }, "symbol" } },
		  { { "if", "keyword" },	  { "then", "keyword" },	{ "else", "keyword" },
			{ "elseif", "keyword" },  { "do", "keyword" },		{ "while", "keyword" },
			{ "for", "keyword" },	  { "break", "keyword" },	{ "continue", "keyword" },
			{ "return", "keyword" },  { "goto", "keyword" },	{ "struct", "keyword" },
			{ "union", "keyword" },	  { "typedef", "keyword" }, { "enum", "keyword" },
			{ "extern", "keyword" },  { "static", "keyword" },	{ "volatile", "keyword" },
			{ "const", "keyword" },	  { "inline", "keyword" },	{ "switch", "keyword" },
			{ "case", "keyword" },	  { "default", "keyword" }, { "auto", "keyword" },
			{ "const", "keyword" },	  { "void", "keyword" },	{ "int", "keyword2" },
			{ "short", "keyword2" },  { "long", "keyword2" },	{ "float", "keyword2" },
			{ "double", "keyword2" }, { "char", "keyword2" },	{ "unsigned", "keyword2" },
			{ "bool", "keyword2" },	  { "true", "literal" },	{ "false", "literal" },
			{ "NULL", "literal" },	  { "nil", "literal" } },
		  "//" } );
}

static void addDart() {
	SyntaxDefinitionManager::instance()->add(
		{ "Dart",
		  { "%.dart$" },
		  {
			  { { "//.-\n" }, "comment" },
			  { { "///.-\n" }, "comment" },
			  { { "/%*", "%*/" }, "comment" },
			  { { "\"", "\"", "\\" }, "string" },
			  { { "'", "'", "\\" }, "string" },
			  { { "-?0x%x+" }, "number" },
			  { { "-?%d+[%d%.eE]*f?" }, "number" },
			  { { "-?%.?%d+f?" }, "number" },
			  { { "[%+%-=/%*%^%%<>!~|&]" }, "operator" },
			  { { "%?%?" }, "operator" },
			  { { "%?%." }, "operator" },
			  { { "[%$%@]?\"", "\"", "\\" }, "string" },
			  { { "'\\x%x?%x?%x?%x'" }, "string" },
			  { { "[%a_][%w_]*%f[(]" }, "function" },
			  { { "[%a_][%w_]*" }, "symbol" },
		  },
		  { { "await", "keyword" },	   { "bool", "keyword2" },	  { "break", "keyword" },
			{ "case", "keyword" },	   { "class", "keyword" },	  { "const", "keyword" },
			{ "continue", "keyword" }, { "default", "keyword" },  { "do", "keyword" },
			{ "double", "keyword2" },  { "dynamic", "keyword2" }, { "else", "keyword" },
			{ "enum", "keyword" },	   { "false", "literal" },	  { "final", "keyword" },
			{ "finally", "keyword" },  { "for", "keyword" },	  { "Function", "keyword2" },
			{ "if", "keyword" },	   { "in", "keyword" },		  { "int", "keyword2" },
			{ "List", "keyword2" },	   { "Map", "keyword2" },	  { "new", "keyword" },
			{ "null", "literal" },	   { "part of", "keyword" },  { "print", "keyword" },
			{ "return", "keyword" },   { "static", "keyword" },	  { "String", "keyword2" },
			{ "switch", "keyword" },   { "then", "keyword" },	  { "this", "keyword2" },
			{ "true", "literal" },	   { "void", "keyword" },	  { "while", "keyword" },
			{ "var", "keyword" } },
		  "//" } );
}

static void addKotlin() {
	SyntaxDefinitionManager::instance()->add(
		{ "Kotlin",
		  { "%.kt$" },
		  {
			  { { "//.-\n" }, "comment" },
			  { { "/%*", "%*/" }, "comment" },
			  { { "\"", "\"", "\\" }, "string" },
			  { { "'", "'", "\\" }, "string" },
			  { { "'\\x%x?%x?%x?%x'" }, "string" },
			  { { "'\\u%x%x%x%x'" }, "string" },
			  { { "'\\?.'" }, "string" },
			  { { "-?0x%x+" }, "number" },
			  { { "-?%d+[%d%.eE]*f?" }, "number" },
			  { { "-?%.?%d+f?" }, "number" },
			  { { "[%+%-=/%*%^%%<>!~|&]" }, "operator" },
			  { { "%@[%a_][%w_]*" }, "function" },
			  { { "[%a_][%w_]*%f[(]" }, "function" },
			  { { "[%a_][%w_]*" }, "symbol" },
		  },
		  { { "if", "keyword" },		  { "then", "keyword" },
			{ "else", "keyword" },		  { "elseif", "keyword" },
			{ "do", "keyword" },		  { "while", "keyword" },
			{ "for", "keyword" },		  { "new", "keyword" },
			{ "break", "keyword" },		  { "continue", "keyword" },
			{ "return", "keyword" },	  { "goto", "keyword" },
			{ "class", "keyword" },		  { "implements", "keyword" },
			{ "extends", "keyword" },	  { "private", "keyword" },
			{ "protected", "keyword" },	  { "public", "keyword" },
			{ "abstract", "keyword" },	  { "interface", "keyword" },
			{ "assert", "keyword" },	  { "import", "keyword" },
			{ "native", "keyword" },	  { "package", "keyword" },
			{ "super", "keyword" },		  { "synchronized", "keyword" },
			{ "instanceof", "keyword" },  { "enum", "keyword" },
			{ "catch", "keyword" },		  { "throw", "keyword" },
			{ "throws", "keyword" },	  { "try", "keyword" },
			{ "transient", "keyword" },	  { "finally", "keyword" },
			{ "static", "keyword" },	  { "volatile", "keyword" },
			{ "final", "keyword" },		  { "switch", "keyword" },
			{ "case", "keyword" },		  { "default", "keyword" },
			{ "void", "keyword" },		  { "Int", "keyword2" },
			{ "Short", "keyword2" },	  { "Byte", "keyword2" },
			{ "Long", "keyword2" },		  { "Float", "keyword2" },
			{ "Double", "keyword2" },	  { "String", "keyword2" },
			{ "Boolean", "keyword2" },	  { "true", "literal" },
			{ "false", "literal" },		  { "null", "literal" },
			{ "var", "keyword" },		  { "lateinit", "literal" },
			{ "override", "keyword" },	  { "fun", "keyword" },
			{ "sealed", "keyword" },	  { "companion", "keyword" },
			{ "object", "keyword" },	  { "val", "keyword" },
			{ "UInt", "keyword2" },		  { "UShort", "keyword2" },
			{ "UByte", "keyword2" },	  { "ULong", "keyword2" },
			{ "IntArray", "keyword2" },	  { "ShortArray", "keyword2" },
			{ "ByteArray", "keyword2" },  { "LongArray", "keyword2" },
			{ "UIntArray", "keyword2" },  { "UShortArray", "keyword2" },
			{ "UByteArray", "keyword2" }, { "ULongArray", "keyword2" },
			{ "Array", "keyword2" } },
		  "//" } );
}

static void addZig() {
	SyntaxDefinitionManager::instance()->add(
		{ "Zig",
		  { "%.zig$" },
		  {
			  { { "//.-\n" }, "comment" },
			  { { "\\\\.-\n" }, "string" },
			  { { "\"", "\"", "\\" }, "string" },
			  { { "'", "'", "\\" }, "string" },
			  { { "[iu][%d_]+" }, "keyword2" },
			  { { "0b[01_]+" }, "number" },
			  { { "0o[0-7_]+" }, "number" },
			  { { "0x[%x_]+" }, "number" },
			  { { "0x[%x_]+%.[%x_]*[pP][-+]?%d+" }, "number" },
			  { { "0x[%x_]+%.[%x_]*" }, "number" },
			  { { "0x%.[%x_]+[pP][-+]?%d+" }, "number" },
			  { { "0x%.[%x_]+" }, "number" },
			  { { "0x[%x_]+[pP][-+]?%d+" }, "number" },
			  { { "0x[%x_]+" }, "number" },
			  { { "%d[%d_]*%.[%d_]*[eE][-+]?%d+" }, "number" },
			  { { "%d[%d_]*%.[%d_]*" }, "number" },
			  { { "%d[%d_]*" }, "number" },
			  { { "[%+%-=/%*%^%%<>!~|&%.%?]" }, "operator" },
			  { { "([%a_][%w_]*)(%s*%()" }, { "function", "function", "normal" } },
			  { { "[A-Z][%w_]*" }, "keyword2" },
			  { { "[%a_][%w_]*" }, "symbol" },
			  { { "(@)([%a_][%w_]*)" }, { "operator", "operator", "function" } },
		  },
		  {
			  { "fn", "keyword" },
			  { "asm", "keyword" },
			  { "volatile", "keyword" },
			  { "continue", "keyword" },
			  { "break", "keyword" },
			  { "switch", "keyword" },
			  { "for", "keyword" },
			  { "while", "keyword" },
			  { "var", "keyword" },
			  { "anytype", "keyword" },
			  { "anyframe", "keyword" },
			  { "const", "keyword" },
			  { "test", "keyword" },
			  { "packed", "keyword" },
			  { "extern", "keyword" },
			  { "export", "keyword" },
			  { "pub", "keyword" },
			  { "defer", "keyword" },
			  { "errdefer", "keyword" },
			  { "align", "keyword" },
			  { "usingnamespace", "keyword" },
			  { "noasync", "keyword" },
			  { "async", "keyword" },
			  { "await", "keyword" },
			  { "cancel", "keyword" },
			  { "suspend", "keyword" },
			  { "resume", "keyword" },
			  { "threadlocal", "keyword" },
			  { "linksection", "keyword" },
			  { "callconv", "keyword" },
			  { "try", "keyword" },
			  { "catch", "keyword" },
			  { "orelse", "keyword" },
			  { "unreachable", "keyword" },
			  { "error", "keyword" },
			  { "if", "keyword" },
			  { "else", "keyword" },
			  { "return", "keyword" },
			  { "comptime", "keyword" },
			  { "stdcallcc", "keyword" },
			  { "ccc", "keyword" },
			  { "nakedcc", "keyword" },
			  { "and", "keyword" },
			  { "or", "keyword" },
			  { "struct", "keyword" },
			  { "enum", "keyword" },
			  { "union", "keyword" },
			  { "opaque", "keyword" },
			  { "inline", "keyword" },
			  { "allowzero", "keyword" },
			  { "noalias", "keyword" },
			  { "nosuspend", "keyword" },
			  { "f16", "keyword2" },
			  { "f32", "keyword2" },
			  { "f64", "keyword2" },
			  { "f128", "keyword2" },
			  { "void", "keyword2" },
			  { "c_void", "keyword2" },
			  { "isize", "keyword2" },
			  { "usize", "keyword2" },
			  { "c_short", "keyword2" },
			  { "c_ushort", "keyword2" },
			  { "c_int", "keyword2" },
			  { "c_uint", "keyword2" },
			  { "c_long", "keyword2" },
			  { "c_ulong", "keyword2" },
			  { "c_longlong", "keyword2" },
			  { "c_ulonglong", "keyword2" },
			  { "c_longdouble", "keyword2" },
			  { "bool", "keyword2" },
			  { "noreturn", "keyword2" },
			  { "type", "keyword2" },
			  { "anyerror", "keyword2" },
			  { "comptime_int", "keyword2" },
			  { "comptime_float", "keyword2" },
			  { "true", "literal" },
			  { "false", "literal" },
			  { "null", "literal" },
			  { "undefined", "literal" },
		  },
		  "//" } );
}

static void addNim() {
	std::vector<SyntaxPattern> nim_patterns;
	std::unordered_map<std::string, std::string> nim_symbols;

	const std::vector<std::string> nim_number_patterns = {
		"0[bB][01][01_]*",	  "0o[0-7][0-7_]*",
		"0[xX]%x[%x_]*",	  "%d[%d_]*%.%d[%d_]*[eE][-+]?%d[%d_]*",
		"%d[%d_]*%.%d[%d_]*", "%d[%d_]*",
	};

	std::vector<std::string> nim_type_suffix_patterns;
	const std::vector<std::string> nim_num_number_list = { "", "8", "16", "32", "64" };
	for ( auto& num : nim_num_number_list )
		nim_type_suffix_patterns.push_back( "'?[fuiFUI]" + num );

	for ( const auto& pattern : nim_number_patterns ) {
		for ( const auto& suffix : nim_type_suffix_patterns )
			nim_patterns.push_back( { { pattern + suffix }, "literal" } );
		nim_patterns.push_back( { { pattern }, "literal" } );
	}

	const std::vector<std::string> nim_keywords{
		"addr",		 "and",		"as",	 "asm",		 "bind",	  "block",	"break",   "case",
		"cast",		 "concept", "const", "continue", "converter", "defer",	"discard", "distinct",
		"div",		 "do",		"elif",	 "else",	 "end",		  "enum",	"except",  "export",
		"finally",	 "for",		"from",	 "func",	 "if",		  "import", "in",	   "include",
		"interface", "is",		"isnot", "iterator", "let",		  "macro",	"method",  "mixin",
		"mod",		 "not",		"notin", "object",	 "of",		  "or",		"out",	   "proc",
		"ptr",		 "raise",	"ref",	 "return",	 "shl",		  "shr",	"static",  "template",
		"try",		 "tuple",	"type",	 "using",	 "var",		  "when",	"while",   "xor",
		"yield",
	};

	for ( const auto& keyword : nim_keywords )
		nim_symbols[keyword] = "keyword";

	const std::vector<std::string> nim_standard_types{
		"bool",	   "byte",		  "int",	 "int8",	"int16",   "int32",		 "int64",
		"uint",	   "uint8",		  "uint16",	 "uint32",	"uint64",  "float",		 "float32",
		"float64", "char",		  "string",	 "cstring", "pointer", "typedesc",	 "void",
		"auto",	   "any",		  "untyped", "typed",	"clong",   "culong",	 "cchar",
		"cschar",  "cshort",	  "cint",	 "csize",	"csize_t", "clonglong",	 "cfloat",
		"cdouble", "clongdouble", "cuchar",	 "cushort", "cuint",   "culonglong", "cstringArray",
	};

	for ( const auto& keyword : nim_standard_types )
		nim_symbols[keyword] = "keyword2";

	const std::vector<std::string> nim_standard_generic_types{
		"range", "array", "open[aA]rray", "varargs", "seq", "set", "sink", "lent", "owned",
	};

	for ( const auto& type : nim_standard_generic_types ) {
		nim_patterns.push_back( { { type + "%f[%[]" }, "keyword2" } );
		nim_patterns.push_back( { { type + "+%f[%w]" }, "keyword2" } );
	}

	const std::vector<SyntaxPattern> nim_user_patterns{
		{ { "##?%[", "]##?" }, "comment" },
		{ { "##?.-\n" }, "comment" },
		{ { "\"", "\"", "\\" }, "string" },
		{ { "\"\"\"", "\"\"\"[^\"]" }, "string" },
		{ { "'", "'", "\\" }, "literal" },
		{ { "[a-zA-Z][a-zA-Z0-9_]*%f[(]" }, "function" },
		{ { "[A-Z][a-zA-Z0-9_]*" }, "keyword2" },
		{ { "[a-zA-Z][a-zA-Z0-9_]*" }, "symbol" },
		{ { "%.%f[^.]" }, "normal" },
		{ { ":%f[ ]" }, "normal" },
		{ { "[=+%-*/<>@$~&%%|!?%^&.:\\]+" }, "operator" },
	};

	nim_patterns.insert( nim_patterns.end(), nim_user_patterns.begin(), nim_user_patterns.end() );

	SyntaxDefinitionManager::instance()->add( {
		"Nim",
		{ "%.nim$", "%.nims$", "%.nimble$" },
		nim_patterns,
		nim_symbols,
		"#",
	} );
}

static void addCMake() {
	std::unordered_map<std::string, std::string> cmake_symbols;
	const std::vector<std::string> cmake_keywords{
		"ANDROID",		  "APPLE",	   "BORLAND",		 "CACHE",	   "CYGWIN",	  "ENV",
		"GHSMULTI",		  "IOS",	   "MINGW",			 "MSVC",	   "MSVC10",	  "MSVC11",
		"MSVC12",		  "MSVC14",	   "MSVC60",		 "MSVC70",	   "MSVC71",	  "MSVC80",
		"MSVC90",		  "MSYS",	   "UNIX",			 "WIN32",	   "WINCE",		  "XCODE",
		"ABSTRACT",		  "ADVANCED",  "AUTOMOC",		 "AUTORCC",	   "AUTOUIC",	  "BUNDLE",
		"COST",			  "DEPENDS",   "DEPRECATION",	 "DISABLED",   "ENVIRONMENT", "EchoString",
		"FOLDER",		  "FRAMEWORK", "GENERATED",		 "GNUtoMS",	   "HELPSTRING",  "IMPORTED",
		"KEEP_EXTENSION", "LABELS",	   "LANGUAGE",		 "LOCATION",   "MACROS",	  "MEASUREMENT",
		"MODIFIED",		  "NAME",	   "PREFIX",		 "PROCESSORS", "RESOURCE",	  "SOURCES",
		"SOVERSION",	  "STRINGS",   "SUBDIRECTORIES", "SUFFIX",	   "SYMBOLIC",	  "TESTS",
		"TIMEOUT",		  "TYPE",	   "VALUE",			 "VARIABLES",  "VERSION",	  "XCTEST" };

	for ( const auto& keyword : cmake_keywords )
		cmake_symbols[keyword] = "keyword2";

	const std::vector<std::string> cmake_operators{ "AND", "OR", "NOT", "EQUAL", "MATCHES" };

	for ( const auto& keyword : cmake_operators )
		cmake_symbols[keyword] = "operator";

	const std::vector<std::string> cmake_literals{
		"TRUE",		   "FALSE",		"INTERFACE", "C",		"CXX",	   "EXPR",	  "LIBTYPE",
		"ON",		   "OFF",		"WARNING",	 "REPLACE", "REGEX",   "APPEND",  "DEFINED",
		"TOUPPER",	   "TOLOWER",	"STREQUAL",	 "GLOB",	"LIBRARY", "RUNTIME", "ARCHIVE",
		"DESTINATION", "IMMEDIATE", "TARGET",	 "COMMAND", "STATUS" };

	for ( const auto& keyword : cmake_literals )
		cmake_symbols[keyword] = "literal";

	SyntaxDefinitionManager::instance()->add( { "CMake",
												{ "%.cmake$", "CMakeLists.txt$" },
												{
													{ { "#", "[^\\]\n" }, "comment" },
													{ { "\"", "\"", "\\" }, "string" },
													{ { "'", "'", "\\" }, "string" },
													{ { "[%a_][%w_]*%s?%f[(]" }, "function" },
													{ { "CMAKE_[%w%d_]+" }, "keyword" },
													{ { "CTEST_[%w%d_]+" }, "keyword" },
													{ { "%u[%u%d_]*_[%u%d_]+" }, "keyword" },
													{ { "%${[%a_][%w_]*%}" }, "keyword2" },
													{ { "[%a_][%w_]*" }, "symbol" },
												},
												cmake_symbols,
												"//" } );
}

static void addJSX() {
	SyntaxDefinitionManager::instance()->add(
		{ "JSX",
		  { "%.jsx$" },
		  {
			  { { "return%s+%(", "%s*);" }, "keyword", "HTML" },
			  { { "//.-\n" }, "comment" },
			  { { "/%*", "%*/" }, "comment" },
			  { { "\"", "\"", "\\" }, "string" },
			  { { "'", "'", "\\" }, "string" },
			  { { "`", "`", "\\" }, "string" },
			  { { "0x[%da-fA-F]+" }, "number" },
			  { { "-?%d+[%d%.eE]*" }, "number" },
			  { { "-?%.?%d+" }, "number" },
			  { { "[%+%-=/%*%^%%<>!~|&]" }, "operator" },
			  { { "[%a_][%w_]*%f[(]" }, "function" },
			  { { "[%a_][%w_]*" }, "symbol" },
		  },
		  {
			  { "async", "keyword" },	   { "await", "keyword" },		{ "break", "keyword" },
			  { "case", "keyword" },	   { "catch", "keyword" },		{ "class", "keyword" },
			  { "const", "keyword" },	   { "continue", "keyword" },	{ "debugger", "keyword" },
			  { "default", "keyword" },	   { "delete", "keyword" },		{ "do", "keyword" },
			  { "else", "keyword" },	   { "export", "keyword" },		{ "extends", "keyword" },
			  { "finally", "keyword" },	   { "for", "keyword" },		{ "function", "keyword" },
			  { "get", "keyword" },		   { "if", "keyword" },			{ "import", "keyword" },
			  { "in", "keyword" },		   { "instanceof", "keyword" }, { "let", "keyword" },
			  { "new", "keyword" },		   { "return", "keyword" },		{ "set", "keyword" },
			  { "static", "keyword" },	   { "super", "keyword" },		{ "switch", "keyword" },
			  { "throw", "keyword" },	   { "try", "keyword" },		{ "typeof", "keyword" },
			  { "var", "keyword" },		   { "void", "keyword" },		{ "while", "keyword" },
			  { "with", "keyword" },	   { "yield", "keyword" },		{ "true", "literal" },
			  { "false", "literal" },	   { "null", "literal" },		{ "undefined", "literal" },
			  { "arguments", "keyword2" }, { "Infinity", "keyword2" },	{ "NaN", "keyword2" },
			  { "this", "keyword2" },
		  },
		  "//" } );
}

static void addContainerfile() {
	SyntaxDefinitionManager::instance()->add( { "Containerfile",
												{ "^[Cc]ontainerfile$", "^[dD]ockerfile$" },
												{ { { "#.*\n" }, "comment" },
												  { { "%[", "%]" }, "string" },
												  { { "%sas%s" }, "literal" },
												  { { "--platform=" }, "literal" },
												  { { "--chown=" }, "literal" },
												  { { "[%a_][%w_]*" }, "symbol" } },
												{
													{ "FROM", "keyword" },
													{ "ARG", "keyword2" },
													{ "ENV", "keyword2" },
													{ "RUN", "keyword2" },
													{ "ADD", "keyword2" },
													{ "COPY", "keyword2" },
													{ "WORKDIR", "keyword2" },
													{ "USER", "keyword2" },
													{ "LABEL", "keyword2" },
													{ "EXPOSE", "keyword2" },
													{ "VOLUME", "keyword2" },
													{ "ONBUILD", "keyword2" },
													{ "STOPSIGNAL", "keyword2" },
													{ "HEALTHCHECK", "keyword2" },
													{ "SHELL", "keyword2" },
													{ "ENTRYPOINT", "function" },
													{ "CMD", "function" },
												},
												"#",
												{},
												"dockerfile" } );
}

static void addOdin() {
	SyntaxDefinitionManager::instance()->add(

		{ "Odin",
		  { "%.odin$" },
		  {
			  { { "//.-\n" }, "comment" },
			  { { "/%*", "%*/" }, "comment" },
			  { { "\"", "\"", "\\" }, "string" },
			  { { "'", "'", "\\" }, "string" },
			  { { "`", "`" }, "string" },
			  { { "0b[01_]+" }, "number" },
			  { { "0o[0-7_]+" }, "number" },
			  { { "0[dz][%d_]+" }, "number" },
			  { { "0x[%da-fA-F_]+" }, "number" },
			  { { "-?%d+[%d%._e]*i?" }, "number" },
			  { { "[<>~=+-*/]=" }, "operator" },
			  { { "[%+%-=/%*%^%%<>!~|&:]" }, "operator" },
			  { { "%$[%a_][%w_]*" }, "operator" },
			  { { "[%a_][%w_]*%f[(]" }, "function" },
			  { { "[#@][%a_][%w_]*" }, "keyword2" },
			  { { "[#@]%b()" }, "keyword2" },
			  { { "[%a_][%w_]*" }, "symbol" },
		  },
		  {
			  { "package", "keyword" },
			  { "import", "keyword" },
			  { "foreign", "keyword" },
			  { "when", "keyword" },
			  { "if", "keyword" },
			  { "else", "keyword" },
			  { "for", "keyword" },
			  { "defer", "keyword" },
			  { "return", "keyword" },
			  { "switch", "keyword" },
			  { "case", "keyword" },
			  { "in", "keyword" },
			  { "not_in", "keyword" },
			  { "do", "keyword" },
			  { "break", "keyword" },
			  { "continue", "keyword" },
			  { "fallthrough", "keyword" },
			  { "proc", "keyword" },
			  { "struct", "keyword" },
			  { "union", "keyword" },
			  { "enum", "keyword" },
			  { "bit_set", "keyword" },
			  { "map", "keyword" },
			  { "dynamic", "keyword" },
			  { "using", "keyword" },
			  { "inline", "keyword" },
			  { "no_inline", "keyword" },
			  { "context", "keyword" },
			  { "distinct", "keyword" },
			  { "opaque", "keyword" },
			  { "macro", "keyword" },
			  { "const", "keyword" },
			  { "cast", "keyword2" },
			  { "auto_cast", "keyword2" },
			  { "transmute", "keyword2" },
			  { "len", "keyword2" },
			  { "cap", "keyword2" },
			  { "size_of", "keyword2" },
			  { "align_of", "keyword2" },
			  { "offset_of", "keyword2" },
			  { "typeid_of", "keyword2" },
			  { "type_of", "keyword2" },
			  { "type_info_of", "keyword2" },
			  { "type_info_base", "keyword2" },
			  { "swizzle", "keyword2" },
			  { "complex", "keyword2" },
			  { "real", "keyword2" },
			  { "imag", "keyword2" },
			  { "conj", "keyword2" },
			  { "min", "keyword2" },
			  { "max", "keyword2" },
			  { "abs", "keyword2" },
			  { "clamp", "keyword2" },
			  { "assert", "keyword2" },
			  { "rawptr", "keyword2" },
			  { "typeid", "keyword2" },
			  { "any", "keyword2" },
			  { "string", "keyword2" },
			  { "cstring", "keyword2" },
			  { "int", "keyword2" },
			  { "uint", "keyword2" },
			  { "uintptr", "keyword2" },
			  { "rune", "keyword2" },
			  { "byte", "keyword2" },
			  { "u8", "keyword2" },
			  { "u16", "keyword2" },
			  { "u32", "keyword2" },
			  { "u64", "keyword2" },
			  { "u128", "keyword2" },
			  { "i8", "keyword2" },
			  { "i16", "keyword2" },
			  { "i32", "keyword2" },
			  { "i64", "keyword2" },
			  { "i128", "keyword2" },
			  { "f16", "keyword2" },
			  { "f32", "keyword2" },
			  { "f64", "keyword2" },
			  { "u16le", "keyword2" },
			  { "u32le", "keyword2" },
			  { "u64le", "keyword2" },
			  { "u128le", "keyword2" },
			  { "i16le", "keyword2" },
			  { "i32le", "keyword2" },
			  { "i64le", "keyword2" },
			  { "i128le", "keyword2" },
			  { "u16be", "keyword2" },
			  { "u32be", "keyword2" },
			  { "u64be", "keyword2" },
			  { "u128be", "keyword2" },
			  { "i16be", "keyword2" },
			  { "i32be", "keyword2" },
			  { "i64be", "keyword2" },
			  { "i128be", "keyword2" },
			  { "complex32", "keyword2" },
			  { "complex64", "keyword2" },
			  { "complex128", "keyword2" },
			  { "quaternion128", "keyword2" },
			  { "quaternion256", "keyword2" },
			  { "bool", "keyword2" },
			  { "b8", "keyword2" },
			  { "b32", "keyword2" },
			  { "b64", "keyword2" },
			  { "b128", "keyword2" },
			  { "true", "literal" },
			  { "false", "literal" },
			  { "nil", "literal" },
		  },
		  "//" } );
}

static void addIgnore() {
	SyntaxDefinitionManager::instance()->add( { ".ignore file",
												{ "%..*ignore$" },
												{
													{ { "^%s*#.*$" }, "comment" },
													{ { "^%!.*$" }, "keyword" },
												},
												{},
												"#" } );
}

static void addPowerShell() {
	SyntaxDefinitionManager::instance()->add(
		{ "PowerShell",
		  { "%.ps1$", "%.psm1$", "%.psd1$", "%.ps1xml$", "%.pssc$", "%.psrc$", "%.cdxml$" },
		  { { { "#.*\n" }, "comment" },
			{ { "[[\\.]]" }, "normal" },
			{ { "\"", "\"" }, "string" },
			{ { "'", "'" }, "string" },
			{ { "%f[%w_][%d%.]+%f[^%w_]" }, "number" },
			{ { "[%+=/%*%^%%<>!~|&,:]+" }, "operator" },
			{ { "%f[%S]%-[%w%-%_]+" }, "function" },
			{ { "[%u][%a]+[%-][%u][%a]+" }, "function" },
			{ { "${.*}" }, "symbol" },
			{ { "$[%a_@*][%w_]*" }, "keyword2" },
			{ { "$[%$][%a]+" }, "keyword2" },
			{ { "[%a_][%w_]*" }, "symbol" } },
		  { { "if", "keyword" },
			{ "else", "keyword" },
			{ "elseif", "keyword" },
			{ "switch", "keyword" },
			{ "default", "keyword" },
			{ "function", "keyword" },
			{ "filter", "keyword" },
			{ "workflow", "keyword" },
			{ "configuration", "keyword" },
			{ "class", "keyword" },
			{ "enum", "keyword" },
			{ "Parameter", "keyword" },
			{ "ValidateScript", "keyword" },
			{ "CmdletBinding", "keyword" },
			{ "try", "keyword" },
			{ "catch", "keyword" },
			{ "finally", "keyword" },
			{ "throw", "keyword" },
			{ "while", "keyword" },
			{ "for", "keyword" },
			{ "do", "keyword" },
			{ "until", "keyword" },
			{ "break", "keyword" },
			{ "continue", "keyword" },
			{ "foreach", "keyword" },
			{ "in", "keyword" },
			{ "return", "keyword" },
			{ "where", "function" },
			{ "select", "function" },
			{ "filter", "keyword" },
			{ "trap", "keyword" },
			{ "param", "keyword" },
			{ "data", "keyword" },
			{ "dynamicparam", "keyword" },
			{ "begin", "function" },
			{ "process", "function" },
			{ "end", "function" },
			{ "exit", "function" },
			{ "inlinescript", "function" },
			{ "parallel", "function" },
			{ "sequence", "function" },
			{ "true", "literal" },
			{ "false", "literal" },
			{ "TODO", "comment" },
			{ "FIXME", "comment" },
			{ "XXX", "comment" },
			{ "TBD", "comment" },
			{ "HACK", "comment" },
			{ "NOTE", "comment" } },
		  "#" } );
}

static void addWren() {
	SyntaxDefinitionManager::instance()->add(
		{ "Wren",
		  { "%.wren$" },
		  {
			  { { "//.-\n" }, "comment" },
			  { { "/%*", "%*/" }, "comment" },
			  { { "\"", "\"", "\\" }, "string" },
			  { { "'", "'", "\\" }, "string" },
			  { { "-?%.?%d+" }, "number" },
			  { { "%.%.%.?" }, "operator" },
			  { { "[<>!=]=" }, "operator" },
			  { { "[%+%-=/%*%^%%<>!~|&?:]" }, "operator" },
			  { { "[%a_][%w_]*%s*%f[(\"{]" }, "function" },
			  { { "[%a_][%w_]*" }, "symbol" },
		  },
		  {
			  { "break", "keyword" }, { "class", "keyword" },  { "construct", "keyword" },
			  { "else", "keyword" },  { "for", "keyword" },	   { "foreign", "keyword" },
			  { "if", "keyword" },	  { "import", "keyword" }, { "in", "keyword" },
			  { "is", "keyword" },	  { "return", "keyword" }, { "static", "keyword" },
			  { "super", "keyword" }, { "var", "keyword" },	   { "while", "keyword" },
			  { "this", "keyword2" }, { "true", "literal" },   { "false", "literal" },
			  { "null", "literal" },
		  },
		  "//" } );
}

static void addEnv() {
	SyntaxDefinitionManager::instance()->add(
		{ "Environment File",
		  { "%.env$", "%.env.[%w%-%_]*$" },
		  { { { "^#.-\n" }, "comment" },
			{ { "%s#.-\n" }, "comment" },
			{ { "\\[nrtfb\\\"']" }, "literal" },
			{ { "'?\\u%x%x%x%x'?" }, "literal" },
			{ { "(%${)([%w]+[%w_]*)(})" }, { "keyword", "keyword", "keyword2", "keyword" } },
			{ { "%$[%w]+[%w_]*" }, "keyword2" },
			{ { "[%a_][%w-+_%s%p]-%f[=]" }, "keyword" },
			{ { "\"", "\"", "\\" }, "string" },
			{ { "'", "'", "\\" }, "string" },
			{ { "^%[.-%]" }, "keyword2" },
			{ { "%s%[.-%]" }, "keyword2" },
			{ { "=" }, "operator" },
			{ { "https?://[%w_.~!*:@&+$/?%%#-]-%w[-.%w]*%.%w%w%w?%w?:?%d*/?[%w_.~!*:@&+$/"
				"?%%#=-]*" },
			  "link" },
			{ { "[a-z]+" }, "symbol" } },
		  { { "true", "literal" },
			{ "false", "literal" },
			{ "export", "literal" },
			{ "null", "literal" } },
		  "#" } );
}

static void addRuby() {
	SyntaxDefinitionManager::instance()->add( {
		"Ruby",
		{ "%.rb", "%.gemspec", "%.ruby" },
		{
			{ { "\"", "\"", "\\" }, "string" },
			{ { "'", "'", "\\" }, "string" },
			{ { "-?0x%x+" }, "number" },
			{ { "%#.-\n" }, "comment" },
			{ { "-?%d+[%d%.eE]*f?" }, "number" },
			{ { "-?%.?%d+f?" }, "number" },
			{ { "[%+%-=/%*%^%%<>!~|&]" }, "operator" },
			{ { "[%a_][%w_]*%f[(]" }, "function" },
			{ { "@?@[%a_][%w_]*" }, "keyword2" },
			{ { "::[%w_]*" }, "symbol" },
			{ { ":[%w_]*" }, "keyword2" },
			{ { "[%a_][%w_]*:[^:]" }, "keyword2" },
			{ { "[%a_][%w_]*" }, "symbol" },
		},
		{
			{ "nil", "literal" },
			{ "end", "literal" },
			{ "true", "literal" },
			{ "false", "literal" },
			{ "private", "keyword" },
			{ "extend", "keyword" },
			{ "include", "keyword" },
			{ "require", "keyword" },
			{ "require_dependency", "keyword" },
			{ "__ENCODING__", "keyword" },
			{ "__LINE__", "keyword" },
			{ "__FILE__", "keyword" },
			{ "BEGIN", "keyword" },
			{ "END", "keyword" },
			{ "alias", "keyword" },
			{ "and", "keyword" },
			{ "begin", "keyword" },
			{ "break", "keyword" },
			{ "case", "keyword" },
			{ "class", "keyword" },
			{ "def", "keyword" },
			{ "defined?", "keyword" },
			{ "do", "keyword" },
			{ "else", "keyword" },
			{ "elsif", "keyword" },
			{ "ensure", "keyword" },
			{ "for", "keyword" },
			{ "if", "keyword" },
			{ "in", "keyword" },
			{ "module", "keyword" },
			{ "next", "keyword" },
			{ "not", "keyword" },
			{ "or", "keyword" },
			{ "redo", "keyword" },
			{ "rescue", "keyword" },
			{ "retry", "keyword" },
			{ "return", "keyword" },
			{ "self", "keyword" },
			{ "super", "keyword" },
			{ "then", "keyword" },
			{ "undef", "keyword" },
			{ "unless", "keyword" },
			{ "until", "keyword" },
			{ "when", "keyword" },
			{ "while", "keyword" },
			{ "yield", "keyword" },
		},
		"#",
		{ "^#!.*[ /]ruby" },
	} );
}

static void addScala() {
	SyntaxDefinitionManager::instance()->add( {
		"Scala",
		{ "%.sc$", "%.scala$" },
		{
			{ { "//.-\n" }, "comment" },
			{ { "/%*", "%*/" }, "comment" },
			{ { "[ruU]?\"", "\"", "\\" }, "string" },
			{ { "[ruU]?'", "'", "\\" }, "string" },
			{ { "0x[%da-fA-F]+" }, "number" },
			{ { "-?%d+[%d%.eE]*" }, "number" },
			{ { "-?%.?%d+" }, "number" },
			{ { "[%+%-=/%*%^%%<>!~|&]" }, "operator" },
			{ { "[%a_][%w_]*\"\"\"*[%a_][%w_]*\"\"\"" }, "string" },
			{ { "[%a_][%w_]*%f[(]" }, "function" },
			{ { "[%a_][%w_]*" }, "symbol" },
		},
		{
			{ "abstract", "keyword" },	 { "case", "keyword" },		{ "catch", "keyword" },
			{ "class", "keyword" },		 { "finally", "keyword" },	{ "final", "keyword" },
			{ "do", "keyword" },		 { "extends", "keyword" },	{ "forSome", "keyword" },
			{ "implicit", "keyword" },	 { "lazy", "keyword" },		{ "match", "keyword" },
			{ "new", "keyword" },		 { "override", "keyword" }, { "package", "keyword" },
			{ "throw", "keyword" },		 { "trait", "keyword" },	{ "type", "keyword" },
			{ "var", "keyword" },		 { "val", "keyword" },		{ "println", "keyword" },
			{ "return", "keyword" },	 { "for", "keyword" },		{ "Try", "keyword" },
			{ "def", "keyword" },		 { "while", "keyword" },	{ "with", "keyword" },
			{ "if", "keyword" },		 { "else", "keyword" },		{ "import", "keyword" },
			{ "object", "keyword" },	 { "yield", "keyword" },	{ "private", "keyword2" },
			{ "protected", "keyword2" }, { "sealed", "keyword2" },	{ "super", "keyword2" },
			{ "this", "keyword2" },		 { "Byte", "keyword2" },	{ "Short", "keyword2" },
			{ "Int", "keyword2" },		 { "Long", "keyword2" },	{ "Float", "keyword2" },
			{ "Double", "keyword2" },	 { "Char", "keyword2" },	{ "String", "keyword2" },
			{ "List", "keyword2" },		 { "Array", "keyword2" },	{ "Boolean", "keyword2" },
			{ "Null", "literal" },		 { "Any", "literal" },		{ "AnyRef", "literal" },
			{ "Nothing", "literal" },	 { "Unit", "literal" },		{ "true", "literal" },
			{ "false", "literal" },
		},
		"//",
	} );
}

static void addSass() {
	SyntaxDefinitionManager::instance()->add( {
		"Sass",
		{ "%.sass$", "%.scss$" },
		{
			{ { "/[/%*].-\n" }, "comment" },
			{ { "\"", "\"", "\\" }, "string" },
			{ { "'", "'", "\\" }, "string" },
			{ { "$%w+" }, "keyword" },
			{ { "@%w+" }, "literal" },
			{ { "&" }, "keyword2" },
			{ { "[:%/%*%-]" }, "operator" },
			{ { "[%a][%w-]*%s*%f[:]" }, "keyword2" },
			{ { "-?%d+[%d%.]*p[xt]" }, "number" },
			{ { "-?%d+[%d%.]*deg" }, "number" },
			{ { "-?%d+[%d%.]*[s%%]" }, "number" },
			{ { "-?%d+[%d%.]*" }, "number" },
			{ { "^%s*#[%a%_%-][%w%_%-]*" }, "keyword2" },
			{ { ",%s*#[%a%_%-][%w%_%-]*" }, "keyword2" },
			{ { "#%x%x?%x?%x?%x?%x?%x?%x?" }, "string" },
			{ { "#[%a%_%-][%w%_%-]*" }, "keyword2" },
			{ { "%-%-%a[%w%-%_]*" }, "keyword2" },
			{ { "-?%d+[%d%.]*p[xt]" }, "number" },
			{ { "-?%d+[%d%.]*deg" }, "number" },
			{ { "-?%d+[%d%.]*" }, "number" },
			{ { "@[%a][%w%_%-]*" }, "keyword2" },
			{ { "%.[%a%_%-][%w%_%-]*" }, "keyword2" },
			{ { "(:)(hover)" }, { "normal", "operator", "literal" } },
			{ { "(:)(focus%-within)" }, { "normal", "operator", "literal" } },
			{ { "(:)(focus)" }, { "normal", "operator", "literal" } },
			{ { "(:)(selected)" }, { "normal", "operator", "literal" } },
			{ { "(:)(pressed)" }, { "normal", "operator", "literal" } },
			{ { "(:)(disabled)" }, { "normal", "operator", "literal" } },
			{ { "(:)(checked)" }, { "normal", "operator", "literal" } },
			{ { "(:)(root)" }, { "normal", "operator", "link" } },
			{ { "(%a+)(%()" }, { "normal", "function", "normal" } },
			{ { "[%a][%w-]*%s*%f[:]" }, "keyword" },
			{ { "[{}:>~!]" }, "operator" },
			{ { "[#,]%w+" }, "function" },
			{ { "[%a_][%w_]*" }, "symbol" },
		},
		{
			{ "true", "literal" },
			{ "false", "literal" },
			{ "transparent", "literal" },
			{ "none", "literal" },
			{ "center", "literal" },
			{ "arrow", "literal" },
			{ "hand", "literal" },
			{ "ibeam", "literal" },
			{ "wait", "literal" },
			{ "crosshair", "literal" },
			{ "waitarrow", "literal" },
			{ "sizenwse", "literal" },
			{ "sizenesw", "literal" },
			{ "sizewe", "literal" },
			{ "sizens", "literal" },
			{ "sizeall", "literal" },
			{ "nocursor", "literal" },
			{ "important", "literal" },
			{ "aliceblue", "literal" },
			{ "antiquewhite", "literal" },
			{ "aqua", "literal" },
			{ "aquamarine", "literal" },
			{ "azure", "literal" },
			{ "beige", "literal" },
			{ "bisque", "literal" },
			{ "black", "literal" },
			{ "blanchedalmond", "literal" },
			{ "blue", "literal" },
			{ "blueviolet", "literal" },
			{ "brown", "literal" },
			{ "burlywood", "literal" },
			{ "cadetblue", "literal" },
			{ "chartreuse", "literal" },
			{ "chocolate", "literal" },
			{ "coral", "literal" },
			{ "cornflowerblue", "literal" },
			{ "cornsilk", "literal" },
			{ "crimson", "literal" },
			{ "cyan", "literal" },
			{ "darkblue", "literal" },
			{ "darkcyan", "literal" },
			{ "darkgoldenrod", "literal" },
			{ "darkgray", "literal" },
			{ "darkgreen", "literal" },
			{ "darkgrey", "literal" },
			{ "darkkhaki", "literal" },
			{ "darkmagenta", "literal" },
			{ "darkolivegreen", "literal" },
			{ "darkorange", "literal" },
			{ "darkorchid", "literal" },
			{ "darkred", "literal" },
			{ "darksalmon", "literal" },
			{ "darkseagreen", "literal" },
			{ "darkslateblue", "literal" },
			{ "darkslategray", "literal" },
			{ "darkslategrey", "literal" },
			{ "darkturquoise", "literal" },
			{ "darkviolet", "literal" },
			{ "deeppink", "literal" },
			{ "deepskyblue", "literal" },
			{ "dimgray", "literal" },
			{ "dimgrey", "literal" },
			{ "dodgerblue", "literal" },
			{ "firebrick", "literal" },
			{ "floralwhite", "literal" },
			{ "forestgreen", "literal" },
			{ "fuchsia", "literal" },
			{ "gainsboro", "literal" },
			{ "ghostwhite", "literal" },
			{ "gold", "literal" },
			{ "goldenrod", "literal" },
			{ "gray", "literal" },
			{ "green", "literal" },
			{ "greenyellow", "literal" },
			{ "grey", "literal" },
			{ "honeydew", "literal" },
			{ "hotpink", "literal" },
			{ "indianred", "literal" },
			{ "indigo", "literal" },
			{ "ivory", "literal" },
			{ "khaki", "literal" },
			{ "lavender", "literal" },
			{ "lavenderblush", "literal" },
			{ "lawngreen", "literal" },
			{ "lemonchiffon", "literal" },
			{ "lightblue", "literal" },
			{ "lightcoral", "literal" },
			{ "lightcyan", "literal" },
			{ "lightgoldenrodyellow", "literal" },
			{ "lightgray", "literal" },
			{ "lightgreen", "literal" },
			{ "lightgrey", "literal" },
			{ "lightpink", "literal" },
			{ "lightsalmon", "literal" },
			{ "lightseagreen", "literal" },
			{ "lightskyblue", "literal" },
			{ "lightslategray", "literal" },
			{ "lightslategrey", "literal" },
			{ "lightsteelblue", "literal" },
			{ "lightyellow", "literal" },
			{ "lime", "literal" },
			{ "limegreen", "literal" },
			{ "linen", "literal" },
			{ "magenta", "literal" },
			{ "maroon", "literal" },
			{ "mediumaquamarine", "literal" },
			{ "mediumblue", "literal" },
			{ "mediumorchid", "literal" },
			{ "mediumpurple", "literal" },
			{ "mediumseagreen", "literal" },
			{ "mediumslateblue", "literal" },
			{ "mediumspringgreen", "literal" },
			{ "mediumturquoise", "literal" },
			{ "mediumvioletred", "literal" },
			{ "midnightblue", "literal" },
			{ "mintcream", "literal" },
			{ "mistyrose", "literal" },
			{ "moccasin", "literal" },
			{ "navajowhite", "literal" },
			{ "navy", "literal" },
			{ "oldlace", "literal" },
			{ "olive", "literal" },
			{ "olivedrab", "literal" },
			{ "orange", "literal" },
			{ "orangered", "literal" },
			{ "orchid", "literal" },
			{ "palegoldenrod", "literal" },
			{ "palegreen", "literal" },
			{ "paleturquoise", "literal" },
			{ "palevioletred", "literal" },
			{ "papayawhip", "literal" },
			{ "peachpuff", "literal" },
			{ "peru", "literal" },
			{ "pink", "literal" },
			{ "plum", "literal" },
			{ "powderblue", "literal" },
			{ "purple", "literal" },
			{ "red", "literal" },
			{ "rosybrown", "literal" },
			{ "royalblue", "literal" },
			{ "saddlebrown", "literal" },
			{ "salmon", "literal" },
			{ "sandybrown", "literal" },
			{ "seagreen", "literal" },
			{ "seashell", "literal" },
			{ "sienna", "literal" },
			{ "silver", "literal" },
			{ "skyblue", "literal" },
			{ "slateblue", "literal" },
			{ "slategray", "literal" },
			{ "slategrey", "literal" },
			{ "snow", "literal" },
			{ "springgreen", "literal" },
			{ "steelblue", "literal" },
			{ "tan", "literal" },
			{ "teal", "literal" },
			{ "thistle", "literal" },
			{ "tomato", "literal" },
			{ "turquoise", "literal" },
			{ "violet", "literal" },
			{ "wheat", "literal" },
			{ "white", "literal" },
			{ "whitesmoke", "literal" },
			{ "yellow", "literal" },
			{ "yellowgreen", "literal" },
		},
		"//",
	} );
}

static void addPO() {
	SyntaxDefinitionManager::instance()->add( {
		"PO",
		{ "%.po$", "%.pot$" },
		{
			{ { "#", "\n" }, "comment" },
			{ { "\"", "\"", "\\" }, "string" },
			{ { "[%[%]]" }, "operator" },
			{ { "%d+" }, "number" },
			{ { "[%a_][%w_]*" }, "symbol" },
		},
		{
			{ "msgctxt", "keyword" },
			{ "msgid", "keyword" },
			{ "msgid_plural", "keyword" },
			{ "msgstr", "keyword" },
		},
		"#",
	} );
}

static void addPerl() {
	SyntaxDefinitionManager::instance()->add( {
		"Perl",
		{ "%.pm$", "%.pl$" },
		{
			{ { "%#.-\n" }, "comment" },
			{ { "\"", "\"", "\\" }, "string" },
			{ { "'", "'", "\\" }, "string" },
			{ { "-?%d+[%d%.eE]*" }, "number" },
			{ { "-?%.?%d+" }, "number" },
			{ { "[%a_][%w_]*%f[(]" }, "function" },
			{ { "[%@%$%*]+[%a_][%w_]*" }, "keyword2" },
			{ { "%--[%a_][%w_]*" }, "symbol" },
		},
		{
			{ "-A", "keyword" },
			{ "END", "keyword" },
			{ "length", "keyword" },
			{ "setpgrp", "keyword" },
			{ "-B", "keyword" },
			{ "endgrent", "keyword" },
			{ "link", "keyword" },
			{ "setpriority", "keyword" },
			{ "-b", "keyword" },
			{ "endhostent", "keyword" },
			{ "listen", "keyword" },
			{ "setprotoent", "keyword" },
			{ "-C", "keyword" },
			{ "endnetent", "keyword" },
			{ "local", "keyword" },
			{ "setpwent", "keyword" },
			{ "-c", "keyword" },
			{ "endprotoent", "keyword" },
			{ "localtime", "keyword" },
			{ "setservent", "keyword" },
			{ "-d", "keyword" },
			{ "endpwent", "keyword" },
			{ "log", "keyword" },
			{ "setsockopt", "keyword" },
			{ "-e", "keyword" },
			{ "endservent", "keyword" },
			{ "lstat", "keyword" },
			{ "shift", "keyword" },
			{ "-f", "keyword" },
			{ "eof$", "keyword" },
			{ "map", "keyword" },
			{ "shmctl", "keyword" },
			{ "-g", "keyword" },
			{ "eval", "keyword" },
			{ "mkdir", "keyword" },
			{ "shmget", "keyword" },
			{ "-k", "keyword" },
			{ "exec", "keyword" },
			{ "msgctl", "keyword" },
			{ "shmread", "keyword" },
			{ "-l", "keyword" },
			{ "exists", "keyword" },
			{ "msgget", "keyword" },
			{ "shmwrite", "keyword" },
			{ "-M", "keyword" },
			{ "exit", "keyword" },
			{ "msgrcv", "keyword" },
			{ "shutdown", "keyword" },
			{ "-O", "keyword" },
			{ "fcntl", "keyword" },
			{ "msgsnd", "keyword" },
			{ "sin", "keyword" },
			{ "-o", "keyword" },
			{ "fileno", "keyword" },
			{ "my", "keyword" },
			{ "sleep", "keyword" },
			{ "-p", "keyword" },
			{ "flock", "keyword" },
			{ "next", "keyword" },
			{ "socket", "keyword" },
			{ "package", "keyword" },
			{ "-r", "keyword" },
			{ "fork", "keyword" },
			{ "not", "keyword" },
			{ "socketpair", "keyword" },
			{ "-R", "keyword" },
			{ "format", "keyword" },
			{ "oct", "keyword" },
			{ "sort", "keyword" },
			{ "-S", "keyword" },
			{ "formline", "keyword" },
			{ "open", "keyword" },
			{ "splice", "keyword" },
			{ "-s", "keyword" },
			{ "getc", "keyword" },
			{ "opendir", "keyword" },
			{ "split", "keyword" },
			{ "-T", "keyword" },
			{ "getgrent", "keyword" },
			{ "ord", "keyword" },
			{ "sprintf", "keyword" },
			{ "-t", "keyword" },
			{ "getgrgid", "keyword" },
			{ "our", "keyword" },
			{ "sqrt", "keyword" },
			{ "-u", "keyword" },
			{ "getgrnam", "keyword" },
			{ "pack", "keyword" },
			{ "srand", "keyword" },
			{ "-w", "keyword" },
			{ "gethostbyaddr", "keyword" },
			{ "pipe", "keyword" },
			{ "stat", "keyword" },
			{ "-W", "keyword" },
			{ "gethostbyname", "keyword" },
			{ "pop", "keyword" },
			{ "state", "keyword" },
			{ "-X", "keyword" },
			{ "gethostent", "keyword" },
			{ "pos", "keyword" },
			{ "study", "keyword" },
			{ "-x", "keyword" },
			{ "getlogin", "keyword" },
			{ "print", "keyword" },
			{ "substr", "keyword" },
			{ "-z", "keyword" },
			{ "getnetbyaddr", "keyword" },
			{ "printf", "keyword" },
			{ "symlink", "keyword" },
			{ "abs", "keyword" },
			{ "getnetbyname", "keyword" },
			{ "prototype", "keyword" },
			{ "syscall", "keyword" },
			{ "accept", "keyword" },
			{ "getnetent", "keyword" },
			{ "push", "keyword" },
			{ "sysopen", "keyword" },
			{ "alarm", "keyword" },
			{ "getpeername", "keyword" },
			{ "quotemeta", "keyword" },
			{ "sysread", "keyword" },
			{ "atan2", "keyword" },
			{ "getpgrp", "keyword" },
			{ "rand", "keyword" },
			{ "sysseek", "keyword" },
			{ "AUTOLOAD", "keyword" },
			{ "getppid", "keyword" },
			{ "read", "keyword" },
			{ "system", "keyword" },
			{ "BEGIN", "keyword" },
			{ "getpriority", "keyword" },
			{ "readdir", "keyword" },
			{ "syswrite", "keyword" },
			{ "bind", "keyword" },
			{ "getprotobyname", "keyword" },
			{ "readline", "keyword" },
			{ "tell", "keyword" },
			{ "binmode", "keyword" },
			{ "getprotobynumber", "keyword" },
			{ "SUPER", "keyword" },
			{ "readlink", "keyword" },
			{ "telldir", "keyword" },
			{ "bless", "keyword" },
			{ "sub", "keyword" },
			{ "getprotoent", "keyword" },
			{ "readpipe", "keyword" },
			{ "tie", "keyword" },
			{ "getpwent", "keyword" },
			{ "recv", "keyword" },
			{ "tied", "keyword" },
			{ "caller", "keyword" },
			{ "getpwnam", "keyword" },
			{ "redo", "keyword" },
			{ "time", "keyword" },
			{ "chdir", "keyword" },
			{ "getpwuid", "keyword" },
			{ "ref", "keyword" },
			{ "times", "keyword" },
			{ "CHECK", "keyword" },
			{ "getservbyname", "keyword" },
			{ "rename", "keyword" },
			{ "truncate", "keyword" },
			{ "chmod", "keyword" },
			{ "getservbyport", "keyword" },
			{ "require", "keyword" },
			{ "uc", "keyword" },
			{ "chomp", "keyword" },
			{ "getservent", "keyword" },
			{ "reset", "keyword" },
			{ "ucfirst", "keyword" },
			{ "chop", "keyword" },
			{ "getsockname", "keyword" },
			{ "return", "keyword" },
			{ "umask", "keyword" },
			{ "chown", "keyword" },
			{ "getsockopt", "keyword" },
			{ "reverse", "keyword" },
			{ "undef", "keyword" },
			{ "chr", "keyword" },
			{ "glob", "keyword" },
			{ "rewinddir", "keyword" },
			{ "UNITCHECK", "keyword" },
			{ "chroot", "keyword" },
			{ "gmtime", "keyword" },
			{ "rindex", "keyword" },
			{ "unlink", "keyword" },
			{ "close", "keyword" },
			{ "goto", "keyword" },
			{ "rmdir", "keyword" },
			{ "unpack", "keyword" },
			{ "closedir", "keyword" },
			{ "grep", "keyword" },
			{ "say", "keyword" },
			{ "unshift", "keyword" },
			{ "connect", "keyword" },
			{ "hex", "keyword" },
			{ "scalar", "keyword" },
			{ "untie", "keyword" },
			{ "cos", "keyword" },
			{ "index", "keyword" },
			{ "seek", "keyword" },
			{ "use", "keyword" },
			{ "crypt", "keyword" },
			{ "INIT", "keyword" },
			{ "seekdir", "keyword" },
			{ "utime", "keyword" },
			{ "dbmclose", "keyword" },
			{ "int", "keyword" },
			{ "select", "keyword" },
			{ "values", "keyword" },
			{ "dbmopen", "keyword" },
			{ "ioctl", "keyword" },
			{ "semctl", "keyword" },
			{ "vec", "keyword" },
			{ "defined", "keyword" },
			{ "join", "keyword" },
			{ "semget", "keyword" },
			{ "wait", "keyword" },
			{ "delete", "keyword" },
			{ "keys", "keyword" },
			{ "semop", "keyword" },
			{ "waitpid", "keyword" },
			{ "DESTROY", "keyword" },
			{ "kill", "keyword" },
			{ "send", "keyword" },
			{ "wantarray", "keyword" },
			{ "die", "keyword" },
			{ "last", "keyword" },
			{ "setgrent", "keyword" },
			{ "warn", "keyword" },
			{ "dump", "keyword" },
			{ "lc", "keyword" },
			{ "sethostent", "keyword" },
			{ "write", "keyword" },
			{ "each", "keyword" },
			{ "lcfirst", "keyword" },
			{ "setnetent", "keyword" },
		},
		"#",
		{ "^#!.*[ /]perl" },
	} );
}

static void addxit() {
	SyntaxDefinitionManager::instance()->add( {
		"[x]it!",
		{ "%.xit$" },
		{
			{ { "%f[^%s%(]%-%>%s%d%d%d%d%-%d%d%-%d%d%f[\n%s%!%?%)]" }, "number" },
			{ { "%f[^%s%(]%-%>%s%d%d%d%d%/%d%d%/%d%d%f[\n%s%!%?%)]" }, "number" },
			{ { "%f[^%s%(]%-%>%s%d%d%d%d%-[wWqQ]?%d%d?%f[\n%s%!%?%)]" }, "number" },
			{ { "%f[^%s%(]%-%>%s%d%d%d%d%/[wWqQ]?%d%d?%f[\n%s%!%?%)]" }, "number" },
			{ { "%f[^%s%(]%-%>%s%d%d%d%d%f[\n%s%!%?%)]" }, "number" },
			{ { "^(%[%s%]%s)([%.!]+)%s" }, { "operator", "operator", "red" } },
			{ { "^(%[x%]%s)([%.!]+)%s" }, { "function", "function", "red" } },
			{ { "^(%[@%]%s)([%.!]+)%s" }, { "keyword", "keyword", "red" } },
			{ { "^(%[~%]%s)([%.!]+)%s" }, { "comment", "comment", "red" } },
			{ { "%#[%wñàáâãäåèéêëìíîïòóôõöùúûüýÿÑÀÁÂÃÄÅÈÉÊËÌÍÎÏÒÓÔÕÖÙÚÛÜÝ%-%_]+%=\"", "\"" },
			  "string" },
			{ { "%#[%wñàáâãäåèéêëìíîïòóôõöùúûüýÿÑÀÁÂÃÄÅÈÉÊËÌÍÎÏÒÓÔÕÖÙÚÛÜÝ%-%_]+%='", "'" },
			  "string" },
			{ { "%#[%wñàáâãäåèéêëìíîïòóôõöùúûüýÿÑÀÁÂÃÄÅÈÉÊËÌÍÎÏÒÓÔÕÖÙÚÛÜÝ%-%_]+%=[%w%-%_]*" },
			  "string" },
			{ { "%#[%wñàáâãäåèéêëìíîïòóôõöùúûüýÿÑÀÁÂÃÄÅÈÉÊËÌÍÎÏÒÓÔÕÖÙÚÛÜÝ%-%_]+" }, "string" },
			{ { "^%[%s%]%s" }, "operator" },
			{ { "^%[x%]%s" }, "function" },
			{ { "^%[@%]%s" }, "keyword" },
			{ { "^%[~%]%s" }, "comment" },
			{ { "^[%wñàáâãäåèéêëìíîïòóôõöùúûüýÿÑÀÁÂÃÄÅÈÉÊËÌÍÎÏÒÓÔÕÖÙÚÛÜÝ][%w"
				"ñàáâãäåèéêëìíîïòóôõöùúûüýÿÑÀÁÂÃÄÅÈÉÊËÌÍÎÏÒÓÔÕÖÙÚÛÜÝ%s%p]*%f[\n]" },
			  "underline" },
			{ { "https?://[%w_.~!*:@&+$/?%%#-]-%w[-.%w]*%.%w%w%w?%w?:?%d*/?[%w_.~!*:@&+$/"
				"?%%#=-]*" },
			  "link" },
		},
		{},
		"",
	} );
}

static void addNelua() {
	SyntaxDefinitionManager::instance()->add(
		{ "Nelua",
		  { "%.nelua$" },
		  {
			  { { "##%[=*%[", "%]=*%]" }, "function", "Lua" },
			  { { "#|", "|#" }, "function", "Lua" },
			  { { "##", "\n" }, "function", "Lua" },
			  { { "\"", "\"", "\\" }, "string" },
			  { { "'", "'", "\\" }, "string" },
			  { { "%[%[", "%]%]" }, "string" },
			  { { "%-%-%[=*%[", "%]=*%]" }, "comment" },
			  { { "%-%-.-\n" }, "comment" },
			  { { "0x%x+%.%x*[pP][-+]?%d+" }, "number" },
			  { { "0x%x+%.%x*" }, "number" },
			  { { "0x%.%x+[pP][-+]?%d+" }, "number" },
			  { { "0x%.%x+" }, "number" },
			  { { "0x%x+[pP][-+]?%d+" }, "number" },
			  { { "0x%x+" }, "number" },
			  { { "%d%.%d*[eE][-+]?%d+" }, "number" },
			  { { "%d%.%d*" }, "number" },
			  { { "%.?%d*[eE][-+]?%d+" }, "number" },
			  { { "<%S[%w+%._,%s*'\"()<>]-%S>" }, "keyword2" },
			  { { "%.?%d+" }, "number" },
			  { { "%.%.%.?" }, "operator" },
			  { { "[<>~=]=" }, "operator" },
			  { { "[%+%-=/%*%^%%#<>]" }, "operator" },
			  { { "([%a_][%w_]*)(%s*%f[(\"'{])" }, { "function", "function", "normal" } },
			  { { "[%a_][%w_]*" }, "symbol" },
			  { { "::[%a_][%w_]*::" }, "function" },
		  },
		  {
			  { "if", "keyword" },		 { "then", "keyword" },	  { "else", "keyword" },
			  { "elseif", "keyword" },	 { "end", "keyword" },	  { "do", "keyword" },
			  { "function", "keyword" }, { "repeat", "keyword" }, { "until", "keyword" },
			  { "while", "keyword" },	 { "for", "keyword" },	  { "break", "keyword" },
			  { "return", "keyword" },	 { "local", "keyword" },  { "in", "keyword" },
			  { "not", "keyword" },		 { "and", "keyword" },	  { "or", "keyword" },
			  { "goto", "keyword" },	 { "self", "keyword2" },  { "true", "literal" },
			  { "false", "literal" },	 { "nil", "literal" },	  { "global", "keyword" },
			  { "switch", "keyword" },	 { "case", "keyword" },	  { "defer", "keyword" },
			  { "continue", "keyword" }, { "nilptr", "keyword" },
		  },
		  "--",
		  { "^#!.*[ /]nelua" } } );
}

static void addTeal() {
	SyntaxDefinitionManager::instance()->add(
		{ "Teal",
		  { "%.tl$", "%.d.tl$" },
		  {
			  { { "\"", "\"", "\\" }, "string" },
			  { { "'", "'", "\\" }, "string" },
			  { { "%[%[", "%]%]" }, "string" },
			  { { "%-%-%[%[", "%]%]" }, "comment" },
			  { { "%-%-.-\n" }, "comment" },
			  { { "-?0x%x+" }, "number" },
			  { { "-?%d+[%d%.eE]*" }, "number" },
			  { { "-?%.?%d+" }, "number" },
			  { { "<%a+>" }, "keyword2" },
			  { { "%.%.%.?" }, "operator" },
			  { { "[<>~=]=" }, "operator" },
			  { { "[%+%-=/%*%^%%#<>]" }, "operator" },
			  { { "[%a_][%w_]*%s*%f[(\"{]" }, "function" },
			  { { "[%a_][%w_]*" }, "symbol" },
			  { { "::[%a_][%w_]*::" }, "function" },
		  },
		  {
			  { "if", "keyword" },		 { "then", "keyword" },	   { "else", "keyword" },
			  { "elseif", "keyword" },	 { "end", "keyword" },	   { "do", "keyword" },
			  { "function", "keyword" }, { "repeat", "keyword" },  { "until", "keyword" },
			  { "while", "keyword" },	 { "for", "keyword" },	   { "break", "keyword" },
			  { "return", "keyword" },	 { "local", "keyword" },   { "global", "keyword" },
			  { "in", "keyword" },		 { "not", "keyword" },	   { "and", "keyword" },
			  { "or", "keyword" },		 { "goto", "keyword" },	   { "enum", "keyword" },
			  { "record", "keyword" },	 { "any", "keyword2" },	   { "boolean", "keyword2" },
			  { "number", "keyword2" },	 { "string", "keyword2" }, { "thread", "keyword2" },
			  { "true", "literal" },	 { "false", "literal" },   { "nil", "literal" },
		  },
		  "--",
		  { "^#!.*[ /]tl" } } );
}

static void addVue() {
	SyntaxDefinitionManager::instance()
		->add( { "Vue-HTML",
				 {},
				 {
					 { { "%{%{%{", "%}%}%}" }, "function", "JavaScript" },
					 { { "%{%{", "%}%}" }, "function", "JavaScript" },
					 { { "<!%-%-", "%-%->" }, "comment" },
					 { { "%f[^>][^<]", "%f[<]" }, "normal" },
					 { { "\"", "\"", "\\" }, "string" },
					 { { "'", "'", "\\" }, "string" },
					 { { "0x[%da-fA-F]+" }, "number" },
					 { { "-?%d+[%d%.]*f?" }, "number" },
					 { { "-?%.?%d+f?" }, "number" },
					 { { "%f[^<]![%a_][%w%_%-]*" }, "keyword2" },
					 { { "%f[^<][%a_][%w%_%-]*" }, "function" },
					 { { "%f[^<]/[%a_][%w%_%-]*" }, "function" },
					 { { "[%a_][%w_]*" }, "keyword" },
					 { { "[/<>=]" }, "operator" },
				 },
				 {},
				 "",
				 {} } )
		.setVisible( false )
		.setAutoCloseXMLTags( true );

	SyntaxDefinitionManager::instance()->add(
		{ "Vue",
		  { "%.vue?$" },
		  {
			  { { "<%s*[sS][cC][rR][iI][pP][tT]%s*>", "<%s*/%s*[sS][cC][rR][iI][pP][tT]>" },
				"function",
				"JavaScript" },
			  { { "<%s*[sS][tT][yY][lL][eE][^>]*>", "<%s*/%s*[sS][tT][yY][lL][eE]%s*>" },
				"function",
				"CSS" },
			  { { "<%s*[tT][eE][mM][pP][lL][aA][tT][eE][^>]*>",
				  "<%s*/%s*[tT][eE][mM][pP][lL][aA][tT][eE]%s*>" },
				"function",
				"Vue-HTML" },
			  { { "<!%-%-", "%-%->" }, "comment" },
			  { { "%f[^>][^<]", "%f[<]" }, "normal" },
			  { { "\"", "\"", "\\" }, "string" },
			  { { "'", "'", "\\" }, "string" },
			  { { "0x[%da-fA-F]+" }, "number" },
			  { { "-?%d+[%d%.]*f?" }, "number" },
			  { { "-?%.?%d+f?" }, "number" },
			  { { "%f[^<]![%a_][%w%_%-]*" }, "keyword2" },
			  { { "%f[^<][%a_][%w%_%-]*" }, "function" },
			  { { "%f[^<]/[%a_][%w%_%-]*" }, "function" },
			  { { "[%a_][%w_]*" }, "keyword" },
			  { { "[/<>=]" }, "operator" },
		  },
		  {},
		  "",
		  {} } );
}

static void addElixir() {
	SyntaxDefinitionManager::instance()->add(
		{ "Elixir",
		  { "%.ex$", "%.exs$" },
		  { { { "#.*\n" }, "comment" },
			{ { ":\"", "\"", "\\" }, "number" },
			{ { "\"\"\"", "\"\"\"", "\\" }, "string" },
			{ { "\"", "\"", "\\" }, "string" },
			{ { "'", "'", "\\" }, "string" },
			{ { "~%a\"\"\"", "\"\"\"" }, "string" },
			{ { "~%a[/\"|'%(%[%{<]", "[/\"|'%)%]%}>]", "\\" }, "string" },
			{ { "-?0x%x+" }, "number" },
			{ { "-?%d+[%d%.eE]*f?" }, "number" },
			{ { "-?%.?%d+f?" }, "number" },
			{ { ":\"?[%a_][%w_]*\"?" }, "number" },
			{ { "[%a][%w_!?]*%f[(]" }, "function" },
			{ { "%u%w+" }, "normal" },
			{ { "@[%a_][%w_]*" }, "keyword2" },
			{ { "_%a[%w_]*" }, "keyword2" },
			{ { "[%+%-=/%*<>!|&]" }, "operator" },
			{ { "[%a_][%w_]*" }, "symbol" } },
		  { { "def", "keyword" },
			{ "defp", "keyword" },
			{ "defguard", "keyword" },
			{ "defguardp", "keyword" },
			{ "defmodule", "keyword" },
			{ "defprotocol", "keyword" },
			{ "defimpl", "keyword" },
			{ "defrecord", "keyword" },
			{ "defrecordp", "keyword" },
			{ "defmacro", "keyword" },
			{ "defmacrop", "keyword" },
			{ "defdelegate", "keyword" },
			{ "defoverridable", "keyword" },
			{ "defexception", "keyword" },
			{ "defcallback", "keyword" },
			{ "defstruct", "keyword" },
			{ "for", "keyword" },
			{ "case", "keyword" },
			{ "when", "keyword" },
			{ "with", "keyword" },
			{ "cond", "keyword" },
			{ "if", "keyword" },
			{ "unless", "keyword" },
			{ "try", "keyword" },
			{ "receive", "keyword" },
			{ "after", "keyword" },
			{ "raise", "keyword" },
			{ "rescue", "keyword" },
			{ "catch", "keyword" },
			{ "else", "keyword" },
			{ "quote", "keyword" },
			{ "unquote", "keyword" },
			{ "super", "keyword" },
			{ "unquote_splicing", "keyword" },
			{ "do", "keyword" },
			{ "end", "keyword" },
			{ "fn", "keyword" },
			{ "import", "keyword2" },
			{ "alias", "keyword2" },
			{ "use", "keyword2" },
			{ "require", "keyword2" },
			{ "and", "operator" },
			{ "or", "operator" },
			{ "true", "literal" },
			{ "false", "literal" },
			{ "nil", "literal" } },
		  "#" } );
}

static void addCrystal() {
	SyntaxDefinitionManager::instance()->add(
		{ "Crystal",
		  { "%.cr$" },
		  {
			  { { "\"", "\"", "\\" }, "string" },
			  { { "'", "'" }, "string" },
			  { { "-?0x%x+" }, "number" },
			  { { "%#.-\n" }, "comment" },
			  { { " : [%w_| :]*" }, "comment" },
			  { { "-?%d+[%d%.eE]*%f[^eE]" }, "number" },
			  { { "-?%.?%d+f?" }, "number" },
			  { { "[%+%-=/%*%^%%<>!~|&]" }, "operator" },
			  { { "[%a_][%w_]*%f[(?]" }, "function" },
			  { { "%.[%a_][%w_]*%f[(%s]" }, "function" },
			  { { "@?@[%a_][%w_]*" }, "keyword2" },
			  { { ":-[%u_][%u%d_]*%f[^%w]" }, "keyword2" },
			  { { "::[%w_]*" }, "symbol" },
			  { { ":[%w_]*" }, "symbol" },
			  { { "[%a_][%w_]*:[^:]" }, "keyword2" },
			  { { "[%a_][%w_]*" }, "symbol" },
			  { { "%s+" }, "normal" },
			  { { "%w+%f[%s]" }, "normal" },
		  },
		  {
			  { "yield", "keyword" },		 { "unless", "keyword" },
			  { "undef", "keyword" },		 { "true", "literal" },
			  { "then", "keyword" },		 { "super", "keyword2" },
			  { "return", "keyword" },		 { "responds_to?", "function" },
			  { "rescue", "keyword" },		 { "private", "keyword2" },
			  { "puts", "function" },		 { "p!", "function" },
			  { "out", "function" },		 { "class", "keyword" },
			  { "break", "keyword" },		 { "self", "keyword2" },
			  { "alias", "keyword" },		 { "fun", "keyword" },
			  { "Void", "literal" },		 { "until", "keyword" },
			  { "abstract", "keyword" },	 { "__LINE__", "keyword" },
			  { "annotation", "keyword2" },	 { "__FILE__", "keyword" },
			  { "__ENCODING__", "keyword" }, { "case", "keyword" },
			  { "BEGIN", "keyword" },		 { "END", "keyword" },
			  { "end", "keyword" },			 { "else", "keyword" },
			  { "defined?", "keyword" },	 { "require", "keyword" },
			  { "do", "keyword" },			 { "while", "keyword" },
			  { "ensure", "keyword" },		 { "include", "keyword" },
			  { "nil", "literal" },			 { "begin", "keyword" },
			  { "enum", "keyword2" },		 { "when", "keyword" },
			  { "extend", "keyword" },		 { "next", "keyword" },
			  { "if", "keyword" },			 { "false", "literal" },
			  { "def", "keyword" },			 { "elsif", "keyword" },
			  { "in", "keyword" },			 { "module", "keyword" },

		  },
		  "#",
		  {} } );
}

static void addV() {
	SyntaxDefinitionManager::instance()->add(
		{ "V",
		  { "%.v$", "%.vsh$" },
		  {
			  { { "//.-\n" }, "comment" },
			  { { "/%*", "%*/" }, "comment" },
			  { { "\"", "\"", "\\" }, "string" },
			  { { "'", "'", "\\" }, "string" },
			  { { "`", "`", "\\" }, "string" },
			  { { "r'", "'" }, "string" },
			  { { "r\"", "\"" }, "string" },
			  { { "0x[%da-fA-F_]+" }, "number" },
			  { { "0b[01_]+" }, "number" },
			  { { "00[01234567_]+" }, "number" },
			  { { "-?%.?%d+" }, "number" },
			  { { "[%a_][%w_]*%f[(]" }, "function" },
			  { { "[%+%-%*%/%%%~%&%|%^%!%=]" }, "operator" },
			  { { "%:%=" }, "operator" },
			  { { "%.%.%.?" }, "operator" },
			  { { "[%a_][%w_]*" }, "symbol" },
			  { { "%$%s?[%a_][%w_]*" }, "keyword2" },
			  { { "%@%s?[%a_][%w_]*" }, "keyword2" },
			  { { "%s+" }, "normal" },
			  { { "%w+%f[%s]" }, "normal" },
		  },
		  {
			  { "voidptr", "keyword2" },  { "unsafe", "keyword" },	   { "u16", "keyword2" },
			  { "i128", "keyword2" },	  { "goto", "keyword" },	   { "as", "keyword" },
			  { "go", "keyword" },		  { "for", "keyword" },		   { "import", "keyword" },
			  { "fn", "keyword" },		  { "if", "keyword" },		   { "i64", "keyword2" },
			  { "typeof", "keyword" },	  { "f32", "keyword2" },	   { "union", "keyword" },
			  { "u32", "keyword2" },	  { "const", "keyword" },	   { "char", "keyword2" },
			  { "continue", "keyword" },  { "byte", "keyword2" },	   { "i16", "keyword2" },
			  { "assert", "keyword" },	  { "bool", "keyword2" },	   { "atomic", "keyword" },
			  { "isreftype", "keyword" }, { "u8", "keyword2" },		   { "chan", "keyword2" },
			  { "u128", "keyword2" },	  { "__offsetof", "keyword" }, { "i8", "keyword2" },
			  { "int", "keyword2" },	  { "is", "keyword" },		   { "defer", "keyword" },
			  { "interface", "keyword" }, { "false", "literal" },	   { "or", "keyword" },
			  { "pub", "keyword" },		  { "lock", "keyword" },	   { "map", "keyword2" },
			  { "module", "keyword" },	  { "rune", "keyword2" },	   { "shared", "keyword" },
			  { "mut", "keyword" },		  { "match", "keyword" },	   { "static", "keyword" },
			  { "asm", "keyword" },		  { "none", "literal" },	   { "return", "keyword" },
			  { "in", "keyword" },		  { "else", "keyword" },	   { "break", "keyword" },
			  { "rlock", "keyword" },	  { "select", "keyword" },	   { "enum", "keyword" },
			  { "f64", "keyword2" },	  { "sizeof", "keyword" },	   { "string", "keyword2" },
			  { "struct", "keyword" },	  { "thread", "keyword2" },	   { "true", "literal" },
			  { "type", "keyword" },	  { "u64", "keyword2" },
		  },
		  "//",
		  {} } );
}

static void addJulia() {
	SyntaxDefinitionManager::instance()->add(
		{ "Julia",
		  { "%.jl$" },
		  {
			  { { "#=", "=#" }, "comment" },
			  { { "#.*$" }, "comment" },
			  { { "icxx\"\"\"", "\"\"\"" }, "string", "C++" },
			  { { "cxx\"\"\"", "\"\"\"" }, "string", "C++" },
			  { { "py\"\"\"", "\"\"\"" }, "string", "Python" },
			  { { "js\"\"\"", "\"\"\"" }, "string", "JavaScript" },
			  { { "md\"\"\"", "\"\"\"" }, "string", "Markdown" },
			  { { "%d%w*[%.-+*//]" }, "number" },
			  { { "0[oO_][0-7]+" }, "number" },
			  { { "-?0x[%x_]+" }, "number" },
			  { { "-?0b[%x_]+" }, "number" },
			  { { "-?%d+_%d" }, "number" },
			  { { "-?%d+[%d%.eE]*f?" }, "number" },
			  { { "-?%.?%d+f?" }, "number" },
			  { { "[^%d%g]%:%a*" }, "function" },
			  { { "[%+%-=/%*%^%%<>!~|&%:]" }, "operator" },
			  { { "\"\"\".*\"\"\"" }, "string" },
			  { { "\".*\"" }, "string" },
			  { { "[bv]\".*\"" }, "string" },
			  { { "r\".*$" }, "string" },
			  { { "'\\.*'" }, "string" },
			  { { "'.'" }, "string" },
			  { { "[%a_][%w_]*%f[(]" }, "function" },
			  { { "%g*!" }, "function" },
			  { { "[%a_][%w_]*" }, "symbol" },
			  { { "%s+" }, "normal" },
			  { { "%w+%f[%s]" }, "normal" },
		  },
		  {
			  { "while", "keyword" },	   { "where", "keyword" },	  { "using", "keyword" },
			  { "typeof", "keyword" },	   { "true", "literal" },	  { "UInt8", "keyword2" },
			  { "elseif", "keyword" },	   { "type", "keyword" },	  { "String", "keyword2" },
			  { "Float16", "keyword2" },   { "if", "keyword" },		  { "Set", "keyword" },
			  { "Union", "keyword" },	   { "UInt16", "keyword2" },  { "Float32", "keyword2" },
			  { "try", "keyword" },		   { "UInt128", "keyword2" }, { "Ref", "keyword2" },
			  { "Function", "keyword2" },  { "Matrix", "keyword2" },  { "Vector", "keyword2" },
			  { "module", "keyword" },	   { "UInt32", "keyword2" },  { "Int8", "keyword2" },
			  { "continue", "keyword" },   { "end", "keyword" },	  { "Int128", "keyword2" },
			  { "baremodule", "keyword" }, { "Char", "keyword2" },	  { "abstract", "keyword2" },
			  { "Inf", "literal" },		   { "Int16", "keyword2" },	  { "primitive", "keyword2" },
			  { "Int64", "keyword2" },	   { "missing", "literal" },  { "mutable", "keyword2" },
			  { "NaN", "literal" },		   { "Bool", "keyword2" },	  { "Int32", "keyword2" },
			  { "macro", "keyword" },	   { "UInt64", "keyword2" },  { "Int", "keyword2" },
			  { "catch", "keyword" },	   { "do", "keyword" },		  { "export", "keyword" },
			  { "false", "literal" },	   { "Integer", "keyword2" }, { "else", "keyword" },
			  { "break", "keyword" },	   { "in", "keyword" },		  { "Float64", "keyword2" },
			  { "finally", "keyword" },	   { "const", "keyword" },	  { "local", "keyword" },
			  { "begin", "keyword" },	   { "for", "keyword" },	  { "import", "keyword" },
			  { "function", "keyword" },   { "Dict", "keyword" },	  { "global", "keyword" },
			  { "let", "keyword" },		   { "return", "keyword" },	  { "nothing", "literal" },
			  { "quote", "keyword" },	   { "Number", "keyword2" },  { "struct", "keyword2" },
		  },
		  "#",
		  {} } );
}

static void addElm() {
	SyntaxDefinitionManager::instance()->add(
		{ "Elm",
		  { "%.elm$" },
		  {
			  { { "%-%-", "\n" }, "comment" },
			  { { "{%-", "%-}" }, "comment" },
			  { { "\"", "\"", "\\" }, "string" },
			  { { "\"\"\"", "\"\"\"", "\\" }, "string" },
			  { { "'", "'", "\\" }, "string" },
			  { { "-?0x%x+" }, "number" },
			  { { "-?%d+[%d%.eE]*f?" }, "number" },
			  { { "-?%.?%d+f?" }, "number" },
			  { { "%.%." }, "operator" },
			  { { "[=:|&<>%+%-%*\\/%^%%]" }, "operator" },
			  { { "[%a_'][%w_']*" }, "symbol" },
			  { { "%s+" }, "normal" },
			  { { "%w+%f[%s]" }, "normal" },
		  },
		  {
			  { "or", "keyword" },		 { "of", "keyword" },	 { "number", "keyword2" },
			  { "module", "keyword" },	 { "not", "keyword" },	 { "let", "keyword" },
			  { "in", "keyword" },		 { "port", "keyword" },	 { "import", "keyword" },
			  { "if", "keyword" },		 { "Bool", "keyword2" }, { "type", "keyword" },
			  { "case", "keyword" },	 { "xor", "keyword" },	 { "alias", "keyword" },
			  { "String", "keyword2" },	 { "True", "literal" },	 { "Char", "keyword2" },
			  { "Float", "keyword2" },	 { "else", "keyword" },	 { "False", "literal" },
			  { "exposing", "keyword" }, { "Int", "keyword2" },	 { "and", "keyword" },
			  { "then", "keyword" },	 { "as", "keyword" },
		  },
		  "%-%-",
		  {} } );
}

static void addToml() {
	SyntaxDefinitionManager::instance()->add(
		{ "TOML",
		  { "%.toml$" },
		  {
			  { { "#.-\n" }, "comment" },
			  { { "\"\"\"", "\"\"\"", "\\" }, "string" },
			  { { "\"", "\"", "\\" }, "string" },
			  { { "'''", "'''" }, "string" },
			  { { "'", "'" }, "string" },
			  { { "[A-Za-z0-9_%.%-]+%s*%f[=]" }, "function" },
			  { { "%[[A-Za-z0-9_%.%- ]+%]" }, "keyword" },
			  { { "%[%[[A-Za-z0-9_%.%- ]+%]%]" }, "keyword" },
			  { { "[%-+]?[0-9_]+%.[0-9_]+[eE][%-+]?[0-9_]+" }, "number" },
			  { { "[%-+]?[0-9_]+%.[0-9_]+" }, "number" },
			  { { "[%-+]?[0-9_]+[eE][%-+]?[0-9_]+" }, "number" },
			  { { "[%-+]?[0-9_]+" }, "number" },
			  { { "[%-+]?0x[0-9a-fA-F_]+" }, "number" },
			  { { "[%-+]?0o[0-7_]+" }, "number" },
			  { { "[%-+]?0b[01_]+" }, "number" },
			  { { "[%-+]?nan" }, "number" },
			  { { "[%-+]?inf" }, "number" },
			  { { "[a-z]+" }, "symbol" },
			  { { "%s+" }, "normal" },
			  { { "%w+%f[%s]" }, "normal" },
		  },
		  {
			  { "true", "literal" },
			  { "false", "literal" },
		  },
		  "#",
		  {} } );
}

static void addFstab() {
	SyntaxDefinitionManager::instance()->add(
		{ "fstab",
		  { "fstab" },
		  {
			  { { "^#.*$" }, "comment" },
			  { { "[=/:.,]+" }, "operator" },
			  { { "/.*/" }, "string" },
			  { { "#" }, "operator" },
			  { { "%w-%-%w-%-%w-%-%w-%-%w- " }, "string" },
			  { { "%d+%.%d+%.%d+%.%d+" }, "string" },
			  { { " %d+ " }, "number" },
			  { { "[%w_]+" }, "symbol" },
			  { { "%s+" }, "normal" },
			  { { "%w+%f[%s]" }, "normal" },
		  },
		  {
			  { "xfs", "keyword2" },	  { "ufs", "keyword2" },
			  { "sysfs", "keyword2" },	  { "swap", "keyword2" },
			  { "sockfs", "keyword2" },	  { "qnx4", "keyword2" },
			  { "pipefs", "keyword2" },	  { "none", "literal" },
			  { "nfsd", "keyword2" },	  { "msdos", "keyword2" },
			  { "cpuset", "keyword2" },	  { "autofs", "keyword2" },
			  { "configfs", "keyword2" }, { "mqueue", "keyword2" },
			  { "cgroup2", "keyword2" },  { "securityfs", "keyword2" },
			  { "cgroup", "keyword2" },	  { "binfmt_misc", "keyword2" },
			  { "binder", "keyword2" },	  { "proc", "keyword2" },
			  { "btrfs", "keyword2" },	  { "hugetlbfs", "keyword2" },
			  { "aufs", "keyword2" },	  { "UUID", "keyword" },
			  { "LABEL", "keyword" },	  { "squashfs", "keyword2" },
			  { "devpts", "keyword2" },	  { "vfat", "keyword2" },
			  { "ecryptfs", "keyword2" }, { "ramfs", "keyword2" },
			  { "ext4", "keyword2" },	  { "minix", "keyword2" },
			  { "ext2", "keyword2" },	  { "tracefs", "keyword2" },
			  { "ext3", "keyword2" },	  { "pstore", "keyword2" },
			  { "nfs4", "keyword2" },	  { "bpf", "keyword2" },
			  { "fuse", "keyword2" },	  { "debugfs", "keyword2" },
			  { "fuseblk", "keyword2" },  { "tmpfs", "keyword2" },
			  { "ntfs", "keyword2" },	  { "bdev", "keyword2" },
			  { "fusectl", "keyword2" },  { "rpc_pipefs", "keyword2" },
			  { "hfs", "keyword2" },	  { "devtmpfs", "keyword2" },
			  { "hfsplus", "keyword2" },  { "nfs", "keyword2" },
			  { "jfs", "keyword2" },
		  },
		  "#",
		  {} } );
}

static void addHaxe() {
	SyntaxDefinitionManager::instance()->add(
		{ "Haxe Compiler Arguments",
		  { "%.hxml$" },
		  {
			  { { "#.*" }, "comment" },
			  { { "%-[%-%w_]*" }, "keyword" },
			  { { "(%.)(%u[%w_]*)" }, { "normal", "normal", "keyword2" } },
			  { { "%s+" }, "normal" },
			  { { "%w+%f[%s]" }, "normal" },
		  },
		  {},
		  "#" } );

	SyntaxDefinitionManager::instance()
		->add( { "HaxeStringInterpolation",
				 {},
				 {
					 { { "%${", "}", "\\" }, "keyword", ".hx" },
					 { { "%$", "%s", "\\" }, "keyword", ".hx" },
					 { { "[^ ]" }, "string" },
					 { { "%s+" }, "normal" },
					 { { "%w+%f[%s]" }, "normal" },
				 },
				 {} } )
		.setVisible( false );

	SyntaxDefinitionManager::instance()
		->add( { "HaxeRegularExpressions",
				 {},
				 {
					 { { "[%[%]%(%)]" }, "string" },
					 { { "[%.%*%+%?%^%$%|%-]" }, "operator" },
					 { { "%s+" }, "normal" },
					 { { "%w+%f[%s]" }, "normal" },
				 },
				 {} } )
		.setVisible( false );

	SyntaxDefinitionManager::instance()->add(
		{ "Haxe",
		  { "%.hx$" },
		  {
			  { { "%~%/", "%/[igmsu]*" }, "keyword2", "HaxeRegularExpressions" },
			  { { "%.%.%." }, "operator" },
			  { { "(%<)(%u[%w_]*)(%>*)" }, { "normal", "operator", "keyword2", "operator" } },
			  { { "(%#%s*[%a_]*)(.*\n)" }, { "normal", "keyword", "normal" } },
			  { { "(import%s+)(%u[%w]*)" }, { "normal", "keyword", "keyword2" } },
			  { { "(import%s+)([%w%.]*%.)(%u[%w]*)" },
				{ "normal", "keyword", "normal", "keyword2" } },
			  { { "(abstract%s+)(%u[%w_]*%s*%()(%s*%u[%w_]*)" },
				{ "normal", "keyword2", "normal", "keyword2" } },
			  { { "(from%s+)(%u[%w_]*%s+)(to%s+)(%u[%w_]*)" },
				{ "normal", "keyword", "keyword2", "keyword", "keyword2" } },
			  { { "//.*\n" }, "comment" },
			  { { "/%*", "%*/" }, "comment" },
			  { { "\"", "\"", "\\" }, "string" },
			  { { "'", "'", "\\" }, "string", "HaxeStringInterpolation" },
			  { { "-?%.?%d+" }, "number" },
			  { { "-?0x%x+" }, "number" },
			  { { "-?%d+%.[%deE]+" }, "number" },
			  { { "-?%d+[%deE]+" }, "number" },
			  { { "[%+%-%.=/%*%^%%<>!~|&]" }, "operator" },
			  { { "([%a_][%w_]*)(%s*%f[(])" }, { "normal", "function", "normal" } },
			  { { "[%a_][%w_]*" }, "symbol" },
			  { { "(:)(%u[%a_][%w_]*)" }, { "normal", "normal", "keyword2" } },
			  { { "@:[%a_][%w_]*%f[(]" }, "keyword" },
			  { { "%$type" }, "keyword" },
			  { { "%s+" }, "normal" },
			  { { "%w+%f[%s]" }, "normal" },
		  },
		  {
			  { "using", "keyword2" },	  { "true", "literal" },	 { "trace", "keyword" },
			  { "throw", "keyword" },	  { "typedef", "keyword2" }, { "switch", "keyword" },
			  { "try", "keyword" },		  { "static", "keyword" },	 { "set", "keyword" },
			  { "return", "keyword" },	  { "public", "keyword" },	 { "package", "keyword" },
			  { "do", "keyword" },		  { "default", "keyword" },	 { "new", "keyword" },
			  { "private", "keyword" },	  { "macro", "keyword2" },	 { "cast", "keyword" },
			  { "class", "keyword" },	  { "case", "keyword" },	 { "this", "keyword" },
			  { "continue", "keyword" },  { "else", "keyword" },	 { "extern", "keyword2" },
			  { "break", "keyword" },	  { "extends", "keyword2" }, { "interface", "keyword" },
			  { "abstract", "keyword2" }, { "for", "keyword" },		 { "override", "keyword" },
			  { "function", "keyword2" }, { "never", "keyword" },	 { "get", "keyword" },
			  { "final", "keyword" },	  { "if", "keyword" },		 { "implements", "keyword2" },
			  { "var", "keyword2" },	  { "catch", "keyword" },	 { "import", "keyword" },
			  { "false", "literal" },	  { "in", "keyword" },		 { "while", "keyword" },
			  { "inline", "keyword" },	  { "enum", "keyword" },	 { "null", "literal" },

		  },
		  "//",
		  {} } );
}

static void addR() {
	SyntaxDefinitionManager::instance()->add(

		{ "R",
		  { "%.r$", "%.rds$", "%.rda$", "%.rdata$", "%.R$" },
		  {
			  { { "#", "\n" }, "comment" },
			  { { "\"", "\"" }, "string" },
			  { { "'", "'" }, "string" },
			  { { "[%a_][%w_]*%f[(]" }, "function" },
			  { { "[%a_][%w_]*" }, "symbol" },
			  { { "[%+%-=/%*%^%%<>!|&]" }, "operator" },
			  { { "0x[%da-fA-F]+" }, "number" },
			  { { "-?%d+[%d%.eE]*" }, "number" },
			  { { "-?%.?%d+" }, "number" },
			  { { "%s+" }, "normal" },
			  { { "%w+%f[%s]" }, "normal" },
		  },
		  {
			  { "repeat", "keyword" },
			  { "next", "keyword" },
			  { "in", "keyword" },
			  { "if", "keyword" },
			  { "FALSE", "literal" },
			  { "NA_integer", "keyword" },
			  { "NA_character", "keyword" },
			  { "break", "keyword" },
			  { "while", "keyword" },
			  { "NA_complex", "keyword" },
			  { "NA_real", "keyword" },
			  { "else", "keyword" },
			  { "NULL", "literal" },
			  { "TRUE", "literal" },
			  { "Inf", "literal" },
			  { "for", "keyword" },
			  { "NA", "literal" },
			  { "function", "keyword" },
		  },
		  "#",
		  {}

		} );
}

// Syntax definitions can be directly converted from the lite (https://github.com/rxi/lite) and
// lite-plugins (https://github.com/rxi/lite-plugins) supported languages.

SyntaxDefinitionManager::SyntaxDefinitionManager() {
	if ( ms_singleton == nullptr )
		ms_singleton = this;

	mDefinitions.reserve( 64 );

	// Register some languages support.
	addPlainText();
	addAngelScript();
	addBash();
	addBatchScript();
	addC();
	addCMake();
	addContainerfile();
	addCPP();
	addCrystal();
	addCSharp();
	addCSS();
	addD();
	addDart();
	addDiff();
	addElixir();
	addElm();
	addEnv();
	addFstab();
	addGDScript();
	addGLSL();
	addGo();
	addHaskell();
	addHaxe();
	addHLSL();
	addHtaccessFile();
	addHTML();
	addIgnore();
	addIni();
	addJava();
	addJavaScript();
	addJulia();
	addJSON();
	addJSX();
	addKotlin();
	addLatex();
	addLua();
	addMakefile();
	addMarkdown();
	addMeson();
	addNelua();
	addNim();
	addObjeck();
	addObjetiveC();
	addOdin();
	addPascal();
	addPerl();
	addPICO8();
	addPHP();
	addPO();
	addPostgreSQL();
	addPowerShell();
	addPython();
	addR();
	addRuby();
	addRust();
	addSass();
	addScala();
	addSolidity();
	addSQL();
	addSwift();
	addTeal();
	addToml();
	addXML();
	addTypeScript();
	addV();
	addVisualBasic();
	addVue();
	addWren();
	addX86Assembly();
	addxit();
	addYAML();
	addZig();
}

const std::vector<SyntaxDefinition>& SyntaxDefinitionManager::getDefinitions() const {
	return mDefinitions;
}

static json toJson( const SyntaxDefinition& def ) {
	json j;
	j["name"] = def.getLanguageName();
	if ( def.getLSPName() != String::toLower( def.getLanguageName() ) )
		j["lsp_name"] = def.getLSPName();
	j["files"] = def.getFiles();
	if ( !def.getComment().empty() )
		j["comment"] = def.getComment();
	if ( !def.getPatterns().empty() ) {
		j["patterns"] = json::array();
		for ( const auto& ptrn : def.getPatterns() ) {
			json pattern;
			if ( ptrn.patterns.size() == 1 ) {
				pattern["pattern"] = ptrn.patterns[0];
			} else {
				pattern["pattern"] = ptrn.patterns;
			}
			if ( ptrn.types.size() == 1 ) {
				pattern["type"] = ptrn.types[0];
			} else {
				pattern["type"] = ptrn.types;
			}
			if ( !ptrn.syntax.empty() )
				pattern["syntax"] = ptrn.syntax;
			j["patterns"].emplace_back( std::move( pattern ) );
		}
	}
	if ( !def.getSymbols().empty() ) {
		j["symbols"] = json::array();
		for ( const auto& sym : def.getSymbols() )
			j["symbols"].emplace_back( json{ json{ sym.first, sym.second } } );
	}

	if ( !def.getHeaders().empty() )
		j["headers"] = def.getHeaders();

	if ( def.getAutoCloseXMLTags() )
		j["auto_close_xml_tags"] = true;

	if ( !def.isVisible() )
		j["visible"] = false;

	return j;
}

bool SyntaxDefinitionManager::save( const std::string& path,
									const std::vector<SyntaxDefinition>& def ) {
	if ( def.size() == 1 ) {
		return FileSystem::fileWrite( path, toJson( def[0] ).dump( 2 ) );
	} else if ( !def.empty() ) {
		json j = json::array();
		for ( const auto& d : def )
			j.emplace_back( toJson( d ) );
		return FileSystem::fileWrite( path, j.dump( 2 ) );
	} else {
		json j = json::array();
		for ( const auto& d : mDefinitions )
			j.emplace_back( toJson( d ) );
		return FileSystem::fileWrite( path, j.dump( 2 ) );
	}
	return false;
}

std::optional<size_t> SyntaxDefinitionManager::getLanguageIndex( const std::string& langName ) {
	size_t pos = 0;
	for ( const auto& def : mDefinitions ) {
		if ( def.getLanguageName() == langName ) {
			return pos;
		}
		++pos;
	}
	return {};
}

static std::string str( std::string s, const std::string& prepend = "",
						const std::string& append = "", bool allowEmptyString = true ) {
	if ( s.empty() && !allowEmptyString )
		return "";
	String::replaceAll( s, "\\", "\\\\" );
	String::replaceAll( s, "\"", "\\\"" );
	return prepend + "\"" + String::escape( s ) + "\"" + append;
}

static std::string join( std::vector<std::string> const& vec, bool createCont = true,
						 bool allowReduce = false, std::string delim = ", " ) {
	if ( vec.empty() )
		return "{}";
	if ( vec.size() == 1 && allowReduce )
		return str( vec[0] );
	std::string accum = std::accumulate(
		vec.begin() + 1, vec.end(), str( vec[0] ),
		[&delim]( const std::string& a, const std::string& b ) { return a + delim + str( b ); } );
	return createCont ? "{ " + accum + " }" : accum;
}

static std::string funcName( std::string name ) {
	if ( name.empty() )
		return "";
	String::replaceAll( name, " ", "" );
	String::replaceAll( name, "+", "p" );
	String::replaceAll( name, "#", "sharp" );
	name[0] = std::toupper( name[0] );
	return name;
}

std::pair<std::string, std::string> SyntaxDefinitionManager::toCPP( const SyntaxDefinition& def ) {
	std::string func( funcName( def.getLanguageName() ) );
	std::string header = "#ifndef EE_UI_DOC_" + func + "\n#define EE_UI_DOC_" + func +
						 "\n\nnamespace EE { namespace UI { namespace "
						 "Doc { namespace Language {\n\nextern void add" +
						 func + "();\n\n}}}}\n\n#endif\n";
	std::string buf = R"cpp(#include <eepp/ui/doc/languages/x86assembly.hpp>
#include <eepp/ui/doc/syntaxdefinitionmanager.hpp>

namespace EE { namespace UI { namespace Doc { namespace Language {
)cpp";
	buf += "\nvoid add" + func + "() {\n";
	buf += "\nSyntaxDefinitionManager::instance()->add(\n\n{";
	// lang name
	buf += str( def.getLanguageName() ) + ",\n";
	// file types
	buf += join( def.getFiles() ) + ",\n";
	// patterns
	buf += "{\n";
	for ( const auto& pattern : def.getPatterns() )
		buf += "{ " + join( pattern.patterns ) + ", " + join( pattern.types, true, true ) +
			   str( pattern.syntax, ", ", "", false ) + " },\n";
	buf += "\n},\n";
	// symbols
	buf += "{\n";
	for ( const auto& symbol : def.getSymbols() )
		buf += "{ " + str( symbol.first ) + " , " + str( symbol.second ) + " },\n";
	buf += "\n},\n";
	buf += str( def.getComment(), "", "", false ) + ",\n";
	std::string lspName =
		def.getLSPName().empty() || def.getLSPName() == String::toLower( def.getLanguageName() )
			? ""
			: def.getLSPName();
	// headers
	buf += join( def.getHeaders() ) + ( lspName.empty() ? "" : "," ) + "\n";
	// lsp
	buf += lspName.empty() ? "" : str( def.getLSPName() );
	buf += "\n}";
	buf += ")";
	if ( !def.isVisible() )
		buf += ".setVisible( false )";
	if ( def.getAutoCloseXMLTags() )
		buf += ".setAutoCloseXMLTags( true )";
	buf += ";\n}\n";
	buf += "\n}}}} // namespace EE::UI::Doc::Language\n";
	return std::make_pair( std::move( header ), std::move( buf ) );
}

SyntaxDefinition& SyntaxDefinitionManager::add( SyntaxDefinition&& syntaxStyle ) {
	mDefinitions.emplace_back( std::move( syntaxStyle ) );
	return mDefinitions.back();
}

const SyntaxDefinition& SyntaxDefinitionManager::getPlainStyle() const {
	return mDefinitions[0];
}

SyntaxDefinition& SyntaxDefinitionManager::getByExtensionRef( const std::string& filePath ) {
	return const_cast<SyntaxDefinition&>( getByExtension( filePath ) );
}

const SyntaxDefinition&
SyntaxDefinitionManager::getByLanguageName( const std::string& name ) const {
	for ( auto& style : mDefinitions ) {
		if ( style.getLanguageName() == name )
			return style;
	}
	return mDefinitions[0];
}

const SyntaxDefinition& SyntaxDefinitionManager::getByLSPName( const std::string& name ) const {
	for ( auto& style : mDefinitions ) {
		if ( style.getLSPName() == name )
			return style;
	}
	return mDefinitions[0];
}

const SyntaxDefinition&
SyntaxDefinitionManager::getByLanguageId( const String::HashType& id ) const {
	for ( auto& style : mDefinitions ) {
		if ( style.getLanguageId() == id )
			return style;
	}
	return mDefinitions[0];
}

SyntaxDefinition& SyntaxDefinitionManager::getByLanguageNameRef( const std::string& name ) {
	return const_cast<SyntaxDefinition&>( getByLanguageName( name ) );
}

std::vector<std::string> SyntaxDefinitionManager::getLanguageNames() const {
	std::vector<std::string> names;
	for ( auto& style : mDefinitions ) {
		if ( style.isVisible() )
			names.push_back( style.getLanguageName() );
	}
	std::sort( names.begin(), names.end() );
	return names;
}

std::vector<std::string> SyntaxDefinitionManager::getExtensionsPatternsSupported() const {
	std::vector<std::string> exts;
	for ( auto& style : mDefinitions )
		for ( auto& pattern : style.getFiles() )
			exts.emplace_back( pattern );
	return exts;
}

const SyntaxDefinition*
SyntaxDefinitionManager::getPtrByLanguageName( const std::string& name ) const {
	return &getByLanguageName( name );
}

const SyntaxDefinition*
SyntaxDefinitionManager::getPtrByLanguageId( const String::HashType& id ) const {
	return &getByLanguageId( id );
}

static SyntaxDefinition loadLanguage( const nlohmann::json& json ) {
	SyntaxDefinition def;
	try {
		def.setLanguageName( json.value( "name", "" ) );
		if ( json.contains( "lsp_name" ) && json["lsp_name"].is_string() )
			def.setLSPName( json["lsp_name"].get<std::string>() );
		if ( json.contains( "files" ) ) {
			if ( json["files"].is_array() ) {
				const auto& files = json["files"];
				for ( const auto& file : files ) {
					def.addFileType( file );
				}
			} else if ( json["files"].is_string() ) {
				def.addFileType( json["files"].get<std::string>() );
			}
		}
		def.setComment( json.value( "comment", "" ) );
		if ( json.contains( "patterns" ) && json["patterns"].is_array() ) {
			const auto& patterns = json["patterns"];
			for ( const auto& pattern : patterns ) {
				std::vector<std::string> type;
				if ( pattern.contains( "type" ) ) {
					if ( pattern["type"].is_array() ) {
						for ( const auto& t : pattern["type"] ) {
							if ( t.is_string() )
								type.push_back( t.get<std::string>() );
						}
					} else if ( pattern["type"].is_string() ) {
						type.push_back( pattern["type"] );
					}
				} else {
					type.push_back( "normal" );
				}
				auto syntax = !pattern.contains( "syntax" ) || !pattern["syntax"].is_string()
								  ? ""
								  : pattern.value( "syntax", "" );
				std::vector<std::string> ptrns;
				if ( pattern.contains( "pattern" ) ) {
					if ( pattern["pattern"].is_array() ) {
						const auto& ptrnIt = pattern["pattern"];
						for ( const auto& ptrn : ptrnIt )
							ptrns.emplace_back( ptrn );
					} else if ( pattern["pattern"].is_string() ) {
						ptrns.emplace_back( pattern["pattern"] );
					}
				}
				def.addPattern( SyntaxPattern( ptrns, type, syntax ) );
			}
		}
		if ( json.contains( "symbols" ) ) {
			if ( json["symbols"].is_array() ) {
				const auto& symbols = json["symbols"];
				for ( const auto& symbol : symbols ) {
					for ( auto& el : symbol.items() ) {
						def.addSymbol( el.key(), el.value() );
					}
				}
			} else if ( json["symbols"].is_object() ) {
				for ( const auto& [key, value] : json["symbols"].items() ) {
					def.addSymbol( key, value );
				}
			}
		}
		if ( json.contains( "headers" ) && json["headers"].is_array() ) {
			const auto& headers = json["headers"];
			std::vector<std::string> hds;
			if ( headers.is_array() ) {
				for ( const auto& header : headers ) {
					if ( header.is_string() )
						hds.emplace_back( header.get<std::string>() );
				}
			} else if ( headers.is_string() ) {
				hds.push_back( headers.get<std::string>() );
			}
			if ( !hds.empty() )
				def.setHeaders( hds );
		}
		if ( json.contains( "visible" ) && json["visible"].is_boolean() )
			def.setVisible( json["visible"].get<bool>() );
		if ( json.contains( "auto_close_xml_tags" ) && json["auto_close_xml_tags"].is_boolean() )
			def.setAutoCloseXMLTags( json["auto_close_xml_tags"].get<bool>() );
	} catch ( const json::exception& e ) {
		Log::error( "SyntaxDefinition loadLanguage failed:\n%s", e.what() );
	}
	return def;
}

bool SyntaxDefinitionManager::loadFromStream( IOStream& stream,
											  std::vector<std::string>* addedLangs ) {
	if ( stream.getSize() == 0 )
		return false;
	std::string buffer;
	buffer.resize( stream.getSize() );
	stream.read( buffer.data(), buffer.size() );

	nlohmann::json j = nlohmann::json::parse( buffer );

	if ( j.is_array() ) {
		for ( const auto& lang : j ) {
			auto res = loadLanguage( lang );
			if ( !res.getLanguageName().empty() ) {
				auto pos = getLanguageIndex( res.getLanguageName() );
				if ( pos.has_value() ) {
					if ( addedLangs )
						addedLangs->push_back( res.getLanguageName() );
					mDefinitions[pos.value()] = std::move( res );
				} else {
					if ( addedLangs )
						addedLangs->push_back( res.getLanguageName() );
					mDefinitions.emplace_back( std::move( res ) );
				}
			}
		}
	} else {
		auto res = loadLanguage( j );
		if ( !res.getLanguageName().empty() ) {
			auto pos = getLanguageIndex( res.getLanguageName() );
			if ( pos.has_value() ) {
				if ( addedLangs )
					addedLangs->push_back( res.getLanguageName() );
				mDefinitions[pos.value()] = std::move( res );
			} else {
				if ( addedLangs )
					addedLangs->push_back( res.getLanguageName() );
				mDefinitions.emplace_back( std::move( res ) );
			}
		}
	}

	return true;
}

bool SyntaxDefinitionManager::loadFromStream( IOStream& stream ) {
	return loadFromStream( stream, nullptr );
}

bool SyntaxDefinitionManager::loadFromFile( const std::string& fpath ) {
	if ( FileSystem::fileExists( fpath ) ) {
		IOStreamFile IOS( fpath );

		return loadFromStream( IOS );
	} else if ( PackManager::instance()->isFallbackToPacksActive() ) {
		std::string tgPath( fpath );

		Pack* tPack = PackManager::instance()->exists( tgPath );

		if ( NULL != tPack ) {
			return loadFromPack( tPack, tgPath );
		}
	}
	return false;
}

bool SyntaxDefinitionManager::loadFromMemory( const Uint8* data, const Uint32& dataSize ) {
	IOStreamMemory IOS( (const char*)data, dataSize );
	return loadFromStream( IOS );
}

bool SyntaxDefinitionManager::loadFromPack( Pack* Pack, const std::string& filePackPath ) {
	if ( NULL != Pack && Pack->isOpen() && -1 != Pack->exists( filePackPath ) ) {
		ScopedBuffer buffer;
		Pack->extractFileToMemory( filePackPath, buffer );
		return loadFromMemory( buffer.get(), buffer.length() );
	}
	return false;
}

void SyntaxDefinitionManager::loadFromFolder( const std::string& folderPath ) {
	if ( !FileSystem::isDirectory( folderPath ) )
		return;
	auto files = FileSystem::filesInfoGetInPath( folderPath );
	if ( files.empty() )
		return;
	for ( const auto& file : files ) {
		if ( file.isRegularFile() && file.isReadable() && file.getExtension() == "json" )
			loadFromFile( file.getFilepath() );
	}
}

const SyntaxDefinition& SyntaxDefinitionManager::getByExtension( const std::string& filePath,
																 bool hFileAsCPP ) const {
	std::string extension( FileSystem::fileExtension( filePath ) );
	std::string fileName( FileSystem::fileNameFromPath( filePath ) );

	// Use the filename instead
	if ( extension.empty() )
		extension = FileSystem::fileNameFromPath( filePath );

	if ( !extension.empty() ) {
		for ( auto style = mDefinitions.rbegin(); style != mDefinitions.rend(); ++style ) {
			for ( const auto& ext : style->getFiles() ) {
				if ( String::startsWith( ext, "%." ) || String::startsWith( ext, "^" ) ||
					 String::endsWith( ext, "$" ) ) {
					LuaPattern words( ext );
					int start, end;
					if ( words.find( fileName, start, end ) ) {
						if ( hFileAsCPP && style->getLSPName() == "c" && ext == "%.h$" )
							return getByLSPName( "cpp" );
						return *style;
					}
				} else if ( extension == ext ) {
					if ( hFileAsCPP && style->getLSPName() == "c" && ext == ".h" )
						return getByLSPName( "cpp" );
					return *style;
				}
			}
		}
	}
	return mDefinitions[0];
}

const SyntaxDefinition& SyntaxDefinitionManager::getByHeader( const std::string& header,
															  bool /*hFileAsCPP*/ ) const {
	if ( !header.empty() ) {
		for ( auto style = mDefinitions.rbegin(); style != mDefinitions.rend(); ++style ) {
			for ( const auto& hdr : style->getHeaders() ) {
				LuaPattern words( hdr );
				int start, end;
				if ( words.find( header, start, end ) ) {
					return *style;
				}
			}
		}
	}
	return mDefinitions[0];
}

const SyntaxDefinition& SyntaxDefinitionManager::find( const std::string& filePath,
													   const std::string& header,
													   bool hFileAsCPP ) {
	const SyntaxDefinition& def = getByHeader( header );
	if ( def.getLanguageName() == mDefinitions[0].getLanguageName() )
		return getByExtension( filePath, hFileAsCPP );
	return def;
}

}}} // namespace EE::UI::Doc
