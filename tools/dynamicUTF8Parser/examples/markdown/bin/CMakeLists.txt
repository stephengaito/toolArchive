# This is the cmake description of how to build the dynamicUtf8Parser 
# Markdown example sources.

defineExecutable(utf8Markdown "bin" "hattrie;dynUtf8Parser")
add_dependencies(bin utf8MarkdownBin)
