# This is the cmake description of how to build the dynamicUtf8Parser 
# Markdown example sources.

defineExecutable(utf8Markdown "src" "hattrie;dynUtf8Parser")
add_dependencies(src utf8MarkdownBin)
