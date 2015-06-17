#ifndef MARKDOWN_H
#define MARKDOWN_H

#include <dynUtf8Parser/parser.h>

class Markdown {

  public:

    Markdown(void);

    ~Markdown(void);

    Token *parse(const char *utf8Stream, PDMTracer pdmTracer = NULL);

    Token *parse(Utf8Chars *utf8Stream, PDMTracer pdmTracer = NULL);

  protected:

    Parser *parser;

};

#endif
