#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include "hattrie/hat-trie.h"
#include "dynUtf8Parser/utf8chars.h"

/// \brief The Classifier class is used to classify UTF8 characters.
///
/// Users can register new class names and associate with each class
/// name a fixed class set.
///
/// A classification class set is a bit set of base classification
/// indicators (one for each bit) which can be combined using any C/C++
/// bitwise operation.
///
/// The Classifier class uses the [Hat-Trie
/// library](https://github.com/dcjones/hat-trie).
class Classifier {

  public:
    /// \brief A classSet_t represents a bit set of Classifier classes.
    ///
    /// Each classSet_t corresponds to a limited collection of Classifier
    /// base classes assigned by the user. Any particular Classifier class
    /// can be any collection of these base classes depending upon whether
    /// or not a given bit in the classSet_t is (un)set. Currently a
    /// classSet_t bit set is limited to the size of the platform's
    /// pointers (32 and 64 bits on 32 and 64 bit architectures
    /// respectively).
    typedef value_t classSet_t;

    /// \brief Create a UTF8 character classifier.
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

    /// \brief Setup this Classifier to classify UTF8 white space using the
    /// classSet_t provided.
    void classifyWhiteSpace(classSet_t aClassSet);

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

  protected:
    /// \brief The Hat-Trie implementing the class name to class set
    /// mapping used to register a given classification bit set.
    hattrie_t *className2classSet;

    /// \brief The Hat-Trie implementing the utf8Char_t to class set
    /// mapping used to classify a given UTF8 character.
    hattrie_t *utf8Char2classSet;

    /// \brief The classSet_t to be used if a UTF8 character has not
    /// been explicitly classified.
    ///
    /// The unClassifiedSet is the complement of the union of all
    /// classSet_t(s) used by the classifyUtf8CharsAs method.
    classSet_t unClassifiedSet;
};


#endif
