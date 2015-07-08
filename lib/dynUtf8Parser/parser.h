#ifndef PARSER_H
#define PARSER_H

/** \mainpage Dynamic UTF8 Parser index page

This parser is based upon ideas taken from Russ Cox's [Implementing
Regular Expressions](https://swtch.com/~rsc/regexp/) to provide the
parser and Bob Nystrom's [Pratt Parsers: Expression Parsing Made
Easy](http://journal.stuffwithstuff.com/2011/03/19/pratt-parsers-expression-pa$
for the parser.

Being data driven, both the parser and the parser are based upon Daniel
C. Jones's [HAT-Trie](https://github.com/dcjones/hat-trie)
implementation.  The parser uses a HAT-Trie to provide the UTF8
character classifier, and the parser uses a HAT-Trie to provide the
symbol table.

The parser is actually UTF8 naive, other than understanding the
*structure* of UTF8 characters, all character classes, such as
whitespace, special characters, etc, *must* be loaded into the parser's
character classifier. (Semi-)standard classes are provided in a form
that can be loaded if desired.

We make use of Joakim Karlsson's [Bandit C++ testing
framework](https://github.com/joakimkarlsson/bandit).

*/

#include "dynUtf8Parser/nfaBuilder.h"
#include "dynUtf8Parser/dfa/pushDownMachine.h"

using namespace DeterministicFiniteAutomaton;

/// \brief The Parser class brings together the Classifier/NFA/DFA classes
/// into the standard arrangement required for a simple Parser.
class Parser {
  public:

    /// \brief A TokenId is a user assigned NFA::TokenId (Hat-Trie::value_t).
    typedef Token::TokenId TokenId;

    /// \brief Create a Parser.
    Parser(void) {
      classifier   = new Classifier();
      nfa          = new NFA(classifier);
      nfaBuilder   = new NFABuilder(nfa);
      lastClassSet = 1;
      dfa          = NULL;
    }

    /// \brief Delete the parser.
    ~Parser(void) {
      if (dfa) delete dfa;
      dfa = NULL;
      delete nfaBuilder;
      delete nfa;
      delete classifier;
    }

    /// \brief Setup the Classifier to classify white space using the
    /// lastClasseSet (a progression of consequtive powers of 2).
    ///
    /// No classification is made if the Parser has already been compiled.
    Classifier::classSet_t classifyWhiteSpace(void) {
      Classifier::classSet_t classSet = lastClassSet;
      lastClassSet <<=1;
      classifyWhiteSpace(classSet);
      return classSet;
     };

    /// \brief Setup the Classifier to classify white space using the
    /// classSet provided.
    ///
    /// No classification is made if the Parser has already been compiled.
    void classifyWhiteSpace(Classifier::classSet_t classSet) {
      if(!dfa) classifier->classifyWhiteSpace(classSet);
     };

    /// \brief Setup the Classifier to classify a collection of UTF8
    /// characters using the lastClasseSet (a progression of
    /// consequtive powers of 2).
    ///
    /// No classification is made if the Parser has already been compiled.
    Classifier::classSet_t classifyUtf8Chars(const char *chars2Classify,
                                             const char *className) {
      Classifier::classSet_t classSet = lastClassSet;
      lastClassSet <<=1;
      classifyUtf8Chars(chars2Classify, className, classSet);
      return classSet;
     };

    /// \brief Setup the Classifier to classify a collection of UTF8
    /// characters using the classeSet provided.
    ///
    /// No classification is made if the Parser has already been compiled.
    void classifyUtf8Chars(const char *chars2Classify,
                           const char *className,
                           Classifier::classSet_t classSet) {
      if(!dfa) {
        classifier->registerClassSet(className, classSet);
        classifier->classifyUtf8CharsAs(chars2Classify, className);
      }
     };

    /// \brief (pre)Register a character class for use in one or more
    /// rules.
    ///
    /// No addition is made if the Parser has already been compiled.
    void addCharacterClass(const char* aClassName,
                           Classifier::classSet_t aCharacterClass) {
      if (!dfa) classifier->registerClassSet(aClassName, aCharacterClass);
    }

    /// \brief Add a Regular-Expression/TokenId to the Parser.
    ///
    /// No addition is made if the Parser has already been compiled.
    void addRule(const char *startStateName,
                 const char *regExp,
                 TokenId aTokenId,
                 bool ignoreToken = false) {
      if (!dfa) {
        nfaBuilder->compileRegularExpressionForTokenId(startStateName,
                                                       regExp,
                                                       aTokenId,
                                                       ignoreToken);
      }
    }

    /// \brief Add a Regular-Expression/TokenId to the Parser whose
    /// resulting tokens will *not* be added to the parse tree.
    ///
    /// No addition is made if the Parser has already been compiled.
    void addRuleIgnoreToken(const char *startStateName,
                            const char *regExp,
                            TokenId aTokenId) {
      addRule(startStateName, regExp, aTokenId, true);
    }

    /// \brief Compile the Regular-Expression/TokenId information.
    ///
    /// After a Parser has been compiled no further classifications can
    /// be made, or Regular-Expression/TokenIds can be added.
    void compile(void) {
      if (!dfa) {
        dfa = new DFA(nfa);
      }
    }

    /// \brief Parse the provided UTF8 character stream starting at the
    /// named NFA start state. Returns the resulting parse tree as a
    /// token with child tokens.
    ///
    /// If the Parser has not yet been compiled, the NULL token is
    /// returned.
    Token *parseFromUsing(const char *startStateName,
                          Utf8Chars *someChars,
                          PDMTracer *pdmTracer = NULL) {
      if (dfa) {
        PushDownMachine *pdm = new PushDownMachine(dfa);
        Token *result =
          pdm->runFromUsing(nfa->findStartStateId(startStateName),
                            someChars, pdmTracer);
        delete pdm;
        return result;
      }
      return NULL;
    }

  protected:

    /// \brief The Classifier used to classify UTF8 characters.
    Classifier *classifier;

    /// \brief The NFA used to scan Utf8Chars streams.
    NFA *nfa;

    /// \brief The NFABuilder used to build the NFA from regular expressions.
    NFABuilder *nfaBuilder;

    /// \brief the bit representing the last class set assigned.
    Classifier::classSet_t lastClassSet;

    /// \brief The DFA used to scan Utf8Chars streams.
    ///
    /// The DFA is compiled from the NFA by the compile method.
    DFA *dfa;
};

#endif

