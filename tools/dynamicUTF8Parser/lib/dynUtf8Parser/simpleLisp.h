#ifndef SIMPLE_LISP_H
#define SIMPLE_LISP_H

#include "dynUtf8Parser/parser.h"

/// \brief The SimpleLisp class provides a parser which parses simple
/// lisp-like expressions into a corresponding tree of tokens.
class SimpleLisp {

  public:

    /// \brief The Token ids recognized by the SimpleLisp parser.
    enum SimpleListTokenIds {
     WhiteSpace=1,
     Normal=2,
     Expression=3
    };

    /// \brief Create a SimpleLisp parser.
    SimpleLisp(void);

    /// \brief Destroy a SimpleLisp parser.
    ~SimpleLisp(void);

    /// \brief Parse the provided stream of UTF8 characters.
    ///
    /// If the PDMTracer is not null, then it will be used to provide a
    /// detailed tracing of the parser's actions.
    Token *parse(const char *utf8Stream, PDMTracer *pdmTracer = NULL);

    /// \brief Parse the provided stream of UTF8 characters.
    ///
    /// If the PDMTracer is not null, then it will be used to provide a
    /// detailed tracing of the parser's actions.
    Token *parse(Utf8Chars *utf8Stream, PDMTracer *pdmTracer = NULL);

  protected:

    /// \brief The parser used by this SimpleLisp parser.
    Parser *parser;

};


#endif
