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

/// \brief The VarArray class holds the information required
/// to manage a variable array of identical objects.
template<class ItemT>
class VarArray {
  public:

    VarArray(void) {
      numItems  = 0;
      arraySize = 0;
      itemArray = NULL;
    }

    ~VarArray(void) {
      if (itemArray) free(itemArray);
      itemArray = NULL;
      numItems  = 0;
      arraySize = 0;
    }

    size_t getNumItems(void) {
      return numItems;
    }

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

    ItemT popItem(void) {
      ASSERT(numItems); // incorrectly matched push/pops
      numItems--;
      return itemArray[numItems];
    }

  private:

    size_t numItems;

    size_t arraySize;

    ItemT *itemArray;

};

#endif
