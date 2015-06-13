#ifndef VAR_ARRAY_H
#define VAR_ARRAY_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "assertions.h"

#ifndef VarArrayIncrement
#define VarArrayIncrement 10
#endif

/// \brief The VarArray template class holds the information required
/// to manage a variable array of identical objects.
template<class ItemT>
class VarArray {
  public:

    bool invariant(void) const {
      return (numItems <= arraySize) && ((itemArray) || (arraySize==0));
    }

    /// \brief Create a VarArray.
    VarArray(void) {
      numItems  = 0;
      arraySize = 0;
      itemArray = NULL;
      ASSERT(invariant());
    }

    /// \brief Explicitly destroy a VarArray.
    ///
    /// Note that this should be invoked implicitly when ever the
    /// containing object gets deleted.
    ~VarArray(void) {
      ASSERT(invariant());
      if (itemArray) free(itemArray);
      itemArray = NULL;
      numItems  = 0;
      arraySize = 0;
    }

    void shallowCopyFrom(const VarArray &other) {
      ASSERT(other.invariant());
      numItems  = other.numItems;
      arraySize = other.arraySize;
      if (itemArray) free(itemArray);
      itemArray = (ItemT*)calloc(arraySize, sizeof(ItemT));
      if (itemArray && other.itemArray) {
        memcpy(itemArray, other.itemArray, arraySize*sizeof(ItemT));
      }
      ASSERT(invariant());
    }

    VarArray *shallowClone(void) {
      VarArray *result  = new VarArray();
      result->shallowCopyFrom(this);
      return result;
    }

    void deepCopyFrom(const VarArray &other) {
      ASSERT(other.invariant());
      numItems  = other.numItems;
      arraySize = other.arraySize;
      if (itemArray) free(itemArray);
      itemArray = NULL;
      if (arraySize) {
        itemArray = (ItemT*)calloc(arraySize, sizeof(ItemT));
        for (size_t i = 0; i < numItems; i++) {
          itemArray[i].deepCopyFrom(other.itemArray[i]);
        }
      }
      ASSERT(invariant());
    }

    VarArray *deepClone(void) {
      VarArray *result  = new VarArray();
      result->deepCopyFrom(this);
      return result;
    }

    /// \brief Return the current number of items in the array.
    size_t getNumItems(void) const {
      return numItems;
    }

    /// \brief Push a new item onto the "top" of the array.
    void pushItem(ItemT anItem) {
      ASSERT(invariant());
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
      ASSERT(invariant());
    }

    /// \brief Get the requested item.
    ///
    /// Returns the default provided if the itemNumber is out of range.
    ItemT getItem(size_t itemNumber, ItemT defaultItem) {
      ASSERT(invariant());
      if (numItems <= itemNumber) return defaultItem;
      return itemArray[itemNumber];
    }

    /// \brief Set the requested item to the value provided.
    void setItem(size_t itemNumber, ItemT anItem) {
      ASSERT(invariant());
      if (itemNumber < numItems) itemArray[itemNumber] = anItem;
    }

    /// \brief Get the top item
    ItemT getTop(void) const {
      ASSERT(invariant());
      ASSERT(numItems);
      return itemArray[numItems-1];
    }

    /// \brief Remove and return the "top" item on the array.
    ItemT popItem(void) {
      ASSERT(invariant());
      ASSERT(numItems); // incorrectly matched push/pops
      numItems--;
      return itemArray[numItems];
    }

    /// \brief Copy the items in this array into the buffer provided.
    void copyItems(void*buffer, size_t bufferSize) {
      ASSERT(invariant());
      if (numItems*sizeof(ItemT) < bufferSize) {
        bufferSize = numItems*sizeof(ItemT);
      }
      if (bufferSize < numItems*sizeof(ItemT)) {
        bufferSize = sizeof(ItemT)*(bufferSize/sizeof(ItemT));
      }
      memcpy(buffer, itemArray, bufferSize);
    }

    void swapTopTwoItems(void) {
      ASSERT(invariant());
      if (numItems < 2) return;
      ItemT tempItem        = itemArray[numItems-1];
      itemArray[numItems-1] = itemArray[numItems-2];
      itemArray[numItems-2] = tempItem;
    }

    /// \brief Remove all items from this array.
    void clearItems(void) {
      numItems = 0;
      ASSERT(invariant());
    }

  protected:

    /// \brief The current number of items in the array.
    size_t numItems;

    /// \brief The current maximal possible number of items in the array.
    size_t arraySize;

    /// \brief The items in the array.
    ItemT *itemArray;

};

#endif
