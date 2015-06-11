#ifndef BLOCK_ALLOCATOR_H
#define BLOCK_ALLOCATOR_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "varArray.h"

/// \brief The BlockAllocator class holds the information required
/// to allocate multiple blocks of related (sub)structures.
class BlockAllocator {
  public:

    bool invariant(void) const {
      if (endAllocationByte < curAllocationByte) return false;
      if (blocks.getNumItems() == 0) {
        if (endAllocationByte != NULL) return false;
        if (curAllocationByte != NULL) return false;
        return true;
      }
      char *curBlock = blocks.getTop();
      if (curBlock != NULL) {
        if (curAllocationByte    <  curBlock)          return false;
        if (curBlock+blockSize+1 <  curAllocationByte) return false;
        if (curBlock+blockSize+1 != endAllocationByte) return false;
      }
      return true;
    }

    /// \brief Create a new block allocator which allocates a given
    /// blockSize.
    BlockAllocator(size_t aBlockSize) {
      blockSize = aBlockSize;
      curAllocationByte = NULL;
      endAllocationByte = NULL;
      ASSERT_INVARIANT;
    }

    /// \brief Clear (free) all of the blocks.
    void clearBlocks(void) {
      while(blocks.getNumItems()) {
        char* aBlock = blocks.popItem();
        if (aBlock) free(aBlock);
      }
      curAllocationByte = NULL;
      endAllocationByte = NULL;
      ASSERT_INVARIANT;
    }

    /// \brief Destory the block allocator and all of its blocks.
    ~BlockAllocator(void) {
      ASSERT_INVARIANT;
      clearBlocks();
      blockSize = 0;
      curAllocationByte = NULL;
      endAllocationByte = NULL;
    }

  protected:
    // \brief Add a new allocation block to this blockAllocator.
    void addNewBlock(void) {
      ASSERT_INVARIANT;
      curAllocationByte = (char*)calloc(blockSize, 1);
      endAllocationByte = curAllocationByte + blockSize + 1;
      blocks.pushItem(curAllocationByte);
      ASSERT_INVARIANT;
    }

  public:
    /// \brief Allocate a new (sub)structure of the given size.
    char *allocateNewStructure(size_t structureSize) {
      ASSERT_INVARIANT;
      if (endAllocationByte <= curAllocationByte + structureSize) {
        // we need to allocate a new block
        addNewBlock();
      }
      char *newStructure = curAllocationByte;
      curAllocationByte += structureSize;
      ASSERT_INVARIANT;
      return newStructure;
    }

  protected:

    /// \brief The current block from which allocations are being made.
    char *curAllocationByte;

    /// \brief The end of the current allocation block.
    char *endAllocationByte;

    /// \brief The size of each new allocation block
    size_t blockSize;

    /// \brief The blocks from which to allocate new sub-structures.
    VarArray<char*> blocks;
};

#endif
