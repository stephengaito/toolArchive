#ifndef VAR_ARRAY_H
#define VAR_ARRAY_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#ifndef ASSERT
#define ASSERT assert
#endif

#ifndef VarArrayIncrement
#define VarArrayIncrement 10
#endif

/// \brief The VarArray template class holds the information required
/// to manage a variable array of identical objects.
template<class ItemT>
class VarArray {
  public:

    /// \brief Create a VarArray.
    VarArray(void) {
      numItems  = 0;
      arraySize = 0;
      itemArray = NULL;
    }

    /// \brief Explicitly destroy a VarArray.
    ///
    /// Note that this should be invoked implicitly when ever the
    /// containing object gets deleted.
    ~VarArray(void) {
      if (itemArray) free(itemArray);
      itemArray = NULL;
      numItems  = 0;
      arraySize = 0;
    }

    /// \brief Return the current number of items in the array.
    size_t getNumItems(void) {
      return numItems;
    }

    /// \brief Push a new item onto the "top" of the array.
    void pushItem(ItemT anItem) {
      if (arraySize <= numItems) {
        // we need to increase the size of the array
        ItemT *oldArray = itemArray;
        itemArray = (ItemT*)calloc(arraySize+VarArrayIncrement, sizeof(ItemT));
        if (oldArray) {
          memcpy(itemArray, oldArray, arraySize*sizeof(ItemT));
          free(oldArray);
        }
        arraySize += VarArrayIncrement;
      }
      itemArray[numItems] = anItem;
      numItems++;
    }

    /// \brief Get the requested item.
    ///
    /// Returns the default provided if the itemNumber is out of range.
    ItemT getItem(size_t itemNumber, ItemT defaultItem) {
      if (numItems <= itemNumber) return defaultItem;
      return itemArray[itemNumber];
    }

    /// \brief Get the top item
    ItemT getTop(void) {
      return itemArray[numItems-1];
    }

    /// \brief Remove and return the "top" item on the array.
    ItemT popItem(void) {
      ASSERT(numItems); // incorrectly matched push/pops
      numItems--;
      return itemArray[numItems];
    }

    /// \brief Copy the items in this array into the buffer provided.
    void copyItems(void*buffer, size_t bufferSize) {
      if (numItems*sizeof(ItemT) < bufferSize) {
        bufferSize = numItems*sizeof(ItemT);
      }
      if (bufferSize < numItems*sizeof(ItemT)) {
        bufferSize = sizeof(ItemT)*(bufferSize/sizeof(ItemT));
      }
      memcpy(buffer, itemArray, bufferSize);
    }

  private:

    /// \brief The current number of items in the array.
    size_t numItems;

    /// \brief The current maximal possible number of items in the array.
    size_t arraySize;

    /// \brief The items in the array.
    ItemT *itemArray;

};

#endif
