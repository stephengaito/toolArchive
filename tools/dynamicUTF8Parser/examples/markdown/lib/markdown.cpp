#include <stdlib.h>

#include "markdown.h"

Markdown::Markdown(void) {
  parser = new Parser();

}

Markdown::~Markdown(void) {
  delete parser;
}

Token *Markdown::parse(const char *utf8Stream, PDMTracer *pdmTracer) {
  Utf8Chars *someChars = new Utf8Chars(utf8Stream);
  Token *result = parse(someChars, pdmTracer);
  delete someChars;
  return result;
}

Token *Markdown::parse(Utf8Chars *utf8Stream, PDMTracer *pdmTracer) {
  return parser->parseFromUsing("start", utf8Stream, pdmTracer);
}
