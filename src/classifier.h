#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include <hat-trie.h>
typedef value_t classSet_t;

#include "utf8chars.h"

/**

\brief The Classifier class is used to classify UTF8 characters.

Users can register new class names and associate with each class name a
fixed class set.

A classification class set is a bit set of base classification
indicators (one for each bit) which can be combined using any C/C++
bitwise operation.

*/
class Classifier {

  public:

    /// \brief Create a UTF8 character classifier.
    ///
    Classifier();

    /// \brief Find the class set associated with a given class.
    ///
    /// Returns the empty set if the given class name has not been
    /// registered.
    classSet_t findClassSet(
      const char* aClassName ///< [in] the UTF8 string name of the class.
    );

    /// \brief Register a class set to a given class.
    ///
    /// Returns the previously registered class set
    ///
    /// Returns the empty set if this class has never been registered.
    classSet_t registerClassSet(
      const char* aClassName, ///< [in] the UTF8 string name of the class.
      classSet_t aClassSet    ///< [in] the (bit) class set
    );

    /// \brief Declare the classification of a collection of UTF8 characters.
    ///
    /// **NOTE** that when classifing a given character twice with two
    /// different classes the *last* classification is used.
    void classifyUtf8CharsAs(
      const char*  someUtf8Chars, ///< [in] the collection of UTF8 characters to classify.
      const char* aClassName      ///< [in] the name of the *previously* registered class to use to classify these characters
    );

    /// \brief Get the class set classification for a given character.
    ///
    /// Returns the empty set if this character has never been classified.
    classSet_t getClassSet(
      utf8Char_t aUtf8Char ///< [in] the UTF8 character to be classified
    );

    /// \brief Get the class set classification for a given character.
    ///
    /// Returns the empty set if this character has never been classified.
    classSet_t getClassSet(
      const char* aUtf8Char ///< [in] a UTF8 character to be classified. If there are multiple characters **ONLY** the first character is classified.
    );

  private:
    hattrie_t *className2classSet;
    hattrie_t *utf8Char2classSet;

};


#endif
