#ifndef SIMPLE_LISP_H
#define SIMPLE_LISP_H

#include "parser.h"

class SimpleLisp {

  public:

    enum SimpleListTokenIds {
     WhiteSpace=1,
     NonWhiteSpace=2,
     Expression=3
    };

    SimpleLisp(void);

    ~SimpleLisp(void);

    Token *parse(const char *utf8Stream, PDMTracer *pdmTracer = NULL);

    Token *parse(Utf8Chars *utf8Stream, PDMTracer *pdmTracer = NULL);

  protected:

    Parser *parser;

};


#endif
