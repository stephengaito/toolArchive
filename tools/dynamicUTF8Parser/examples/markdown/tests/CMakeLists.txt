# This is the cmake description of how to build the dynamicUtf8Parser 
# based Markdown parser.

defineTestsFor( utf8Markdown "lib" "dynUtf8Parser;hattrie" )
add_dependencies(tests utf8MarkdownTests)
