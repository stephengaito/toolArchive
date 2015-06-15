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

    /// \brief An invariant which should ALWAYS be true for any
    /// instance of a BlockAllocator class.
    ///
    /// Throws an AssertionFailure with a brief description of any
    /// inconsistencies discovered.
    bool invariant(void) const {
      if (endAllocationByte < curAllocationByte)
        throw AssertionFailure("incorrectly ordered allocation bytes");
      if (blocks.getNumItems() == 0) {
        if (endAllocationByte != NULL)
          throw AssertionFailure("no blocks but endAllocationByte not NULL");
        if (curAllocationByte != NULL)
          throw AssertionFailure("no blocks but curAllocationByte not NULL");
        return true;
      }
      char *curBlock = blocks.getTop();
      if (curBlock != NULL) {
        if (curAllocationByte    <  curBlock)
          throw AssertionFailure("curAllocationByte below block");
        if (curBlock+blockSize+1 <  curAllocationByte)
          throw AssertionFailure("curAllocationByte above block");
        if (curBlock+blockSize+1 != endAllocationByte)
          throw AssertionFailure("incorrect endAllocationByte for block");
      }
      return true;
    }

    /// \brief Create a new block allocator which allocates a given
    /// blockSize.
    BlockAllocator(size_t aBlockSize) {
      blockSize = aBlockSize;
      curAllocationByte = NULL;
      endAllocationByte = NULL;
      ASSERT(invariant());
    }

    /// \brief Clear (free) all of the blocks.
    void clearBlocks(void) {
      while(blocks.getNumItems()) {
        char* aBlock = blocks.popItem();
        if (aBlock) free(aBlock);
      }
      curAllocationByte = NULL;
      endAllocationByte = NULL;
      ASSERT(invariant());
    }

    /// \brief Destory the block allocator and all of its blocks.
    ~BlockAllocator(void) {
      ASSERT(invariant());
      clearBlocks();
      blockSize = 0;
      curAllocationByte = NULL;
      endAllocationByte = NULL;
    }

  public:
    /// \brief Allocate a new (sub)structure of the given size.
    char *allocateNewStructure(size_t structureSize) {
      ASSERT(invariant());
      if (endAllocationByte <= curAllocationByte + structureSize) {
        // we need to allocate a new block
        addNewBlock();
      }
      char *newStructure = curAllocationByte;
      curAllocationByte += structureSize;
      ASSERT(invariant());
      return newStructure;
    }

  protected:

    /// \brief Add a new allocation block to this blockAllocator.
    void addNewBlock(void) {
      ASSERT(invariant());
      curAllocationByte = (char*)calloc(blockSize, 1);
      endAllocationByte = curAllocationByte + blockSize + 1;
      blocks.pushItem(curAllocationByte);
      ASSERT(invariant());
    }

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
