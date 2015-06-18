#include <stdlib.h>

#include "dynUtf8Parser/simpleLisp.h"

SimpleLisp::SimpleLisp(void) {
  parser = new Parser();

  Classifier::classSet_t whiteSpaceClass = parser->classifyWhiteSpace();
  Classifier::classSet_t controlClass    = parser->classifyUtf8Chars("(,)", "control");
  parser->addCharacterClass("normal", ~(whiteSpaceClass | controlClass));

  parser->addRuleIgnoreToken("whiteSpace", "[whiteSpace]+", WhiteSpace);
  parser->addRule("normal", "[normal]+", Normal);
  parser->addRule("expression",
    "{whiteSpace}?\\({expression}({whiteSpace}?,{whiteSpace}?{expression})*\\){whiteSpace}?", Expression);
  parser->addRule("expression", "{whiteSpace}?{normal}{whiteSpace}?", Expression);
  parser->compile();
}

SimpleLisp::~SimpleLisp(void) {
  delete parser;
}


Token *SimpleLisp::parse(const char *utf8Stream, PDMTracer *pdmTracer) {
  Utf8Chars *someChars = new Utf8Chars(utf8Stream);
  Token *result = parse(someChars, pdmTracer);
  delete someChars;
  return result;
}

Token *SimpleLisp::parse(Utf8Chars *utf8Stream, PDMTracer *pdmTracer) {
  return parser->parseFromUsing("expression", utf8Stream, pdmTracer);
}
