#include <stdlib.h>

#include "dynUtf8Parser/tokens.h"

static const char *indents[] = {
  "", //0
  "  ", //1
  "    ", //2
  "      ", //3
  "        ", //4
  "          ", //5
  "            ", //6
  "              ", //7
  "                ", //8
  "                  ", //9
  "                    ", //10
  "                      ", //11
  "                        ", //12
  "                          ", //13
  "                            ", //14
  "                              ", //15
  "                                ", //16
  "                                  ", //17
  "                                    ", //18
  "                                      ", //19
  "                                      - "  //20
};

void Token::printOn(FILE *outFile, size_t indent) {
  if (!outFile) return;
  if (20 < indent) indent = 20;
  const char *text = strndup(textStart, textLength);
  fprintf(outFile, "%s%lu(%lu) [%s]\n",
          indents[indent], tokenId,
          tokens.getNumItems(), text);
  free((void*)text);
//  Token nullToken;
  for (size_t i = 0; i < tokens.getNumItems(); i++) {
    tokens.getItem(i, NULL /*nullToken*/)->printOn(outFile, indent+1);
  }
}
