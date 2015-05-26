#ifndef BLOCK_ALLOCATOR_H
#define BLOCK_ALLOCATOR_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef BlockIncrement
#define BlockIncrement 10
#endif

/// \brief The BlockAllocator class holds the information required
/// to allocate multiple blocks of related (sub)structures.
class BlockAllocator {
  public:

    /// \brief Create a new block allocator which allocates a given
    /// blockSize.
    BlockAllocator(size_t aBlockSize) {
      numBlocks = BlockIncrement;
      nextBlock = 0;
      blockSize = aBlockSize;
      blocks    = (char**)calloc(numBlocks, sizeof(char*));
      curAllocationByte = NULL;
      endAllocationByte = NULL;
      for (size_t i = 0; i < numBlocks; i++) {
        blocks[i] = NULL;
      }
    }

    /// \brief Clear (free) all of the blocks.
    void clearBlocks(void) {
      for (size_t i = 0; i < numBlocks; i++) {
        if (blocks[i]) {
          free(blocks[i]);
          blocks[i] = NULL;
        }
      }
      nextBlock = 0;
    }

    /// \brief Destory the block allocator and all of its blocks.
    ~BlockAllocator(void) {
      if (blocks) {
        clearBlocks();
        free(blocks);
      }
      blocks    = NULL;
      numBlocks = 0;
      nextBlock = 0;
      blockSize = 0;
      curAllocationByte = NULL;
      endAllocationByte = NULL;
    }

  private:
    // \brief Add a new allocation block to this blockAllocator.
    void addNewBlock(void) {
      if (numBlocks <= nextBlock) {
        // we do not have enough block pointers so increase the
        // size of the blocks array.
        char **oldBlocks = blocks;
        blocks =
          (char**)calloc(numBlocks + BlockIncrement, sizeof(char*));
        if (oldBlocks) {
          memcpy(blocks, oldBlocks, numBlocks*sizeof(char*));
          free(oldBlocks);
        }
        numBlocks += BlockIncrement;
      }
      blocks[nextBlock] = (char*)calloc(blockSize, 1);
      curAllocationByte = blocks[nextBlock];
      endAllocationByte = curAllocationByte + blockSize + 1;
      nextBlock++;
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

    /// \brief The index of next (but currently unallocated) allocation
    /// block in the block array.
    size_t nextBlock;

    /// \brief The total number of possible allocation blocks in the
    /// block array.
    size_t numBlocks;

    /// \brief The size of each new allocation block
    size_t blockSize;

    /// \brief The blocks from which to allocate new sub-structures.
    char **blocks;
};

#endif
