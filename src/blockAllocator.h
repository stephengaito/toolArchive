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

    /// \brief Create a new block allocator which allocates a given
    /// blockSize.
    BlockAllocator(size_t aBlockSize) {
      blockSize = aBlockSize;
      curAllocationByte = NULL;
      endAllocationByte = NULL;
    }

    /// \brief Clear (free) all of the blocks.
    void clearBlocks(void) {
      while(blocks.getNumItems()) {
        char* aBlock = blocks.popItem();
        if (aBlock) free(aBlock);
      }
    }

    /// \brief Destory the block allocator and all of its blocks.
    ~BlockAllocator(void) {
      clearBlocks();
      blockSize = 0;
      curAllocationByte = NULL;
      endAllocationByte = NULL;
    }

  private:
    // \brief Add a new allocation block to this blockAllocator.
    void addNewBlock(void) {
        curAllocationByte = (char*)calloc(blockSize, 1);
        endAllocationByte = curAllocationByte + blockSize + 1;
        blocks.pushItem(curAllocationByte);
    }

  public:
    /// \brief Allocate a new (sub)structure of the given size.
    char *allocateNewStructure(size_t structureSize) {
      if (endAllocationByte <= curAllocationByte + structureSize) {
        // we need to allocate a new block
        addNewBlock();
      }
      char *newStructure = curAllocationByte;
      curAllocationByte += structureSize;
      return newStructure;
    }

  private:

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
