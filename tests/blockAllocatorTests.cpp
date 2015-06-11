#include <bandit/bandit.h>
using namespace bandit;

#include <string.h>
#include <stdio.h>
#include <exception>

#ifndef protected
#define protected public
#endif

#include <stdio.h>
#include <blockAllocator.h>

go_bandit([](){

  printf("\n----------------------------------\n");
  printf(  "blockAllocator\n");
  printf(  "BlockAllocator = %zu bytes (%zu bits)\n", sizeof(BlockAllocator), sizeof(BlockAllocator)*8);
  printf(  "----------------------------------\n");

  /// \brief We test the correctness of the C-based BlockAllocator structure.
  ///
  describe("BlockAllocator", [](){

    it("CreateBlockAllocator should create a working block allocator", [&](){
      BlockAllocator *blockAllocator = new BlockAllocator(10);
      AssertThat(blockAllocator, Is().Not().EqualTo((void*)0));
      AssertThat(blockAllocator->blockSize, Is().EqualTo(10));
//      for (size_t i = 0; i < blockAllocator->numBlocks; i++) {
//        AssertThat(blockAllocator->blocks[i], Is().EqualTo((void*)0));
//      }
      delete blockAllocator;
    });

    it("AddNewBlock should add a new block", [&](){
      char* prevPtrs[25];
      BlockAllocator *blockAllocator = new BlockAllocator(10);
      for (size_t j = 0; j < 25; j++) {
        blockAllocator->addNewBlock();
        prevPtrs[j] = blockAllocator->curAllocationByte;
        AssertThat(blockAllocator->blocks.getNumItems(), Is().EqualTo(j+1));
        for (size_t i = 0; i <= j; i++) {
         AssertThat(blockAllocator->blocks.getItem(i, NULL), Is().Not().EqualTo((void*)0));
         AssertThat(blockAllocator->blocks.getItem(i, NULL), Is().EqualTo(prevPtrs[i]));
        }
      }
      delete blockAllocator;
    });

    it("ClearBlocks and AddNewBlock should work together", [&](){
      char* prevPtrs[25];
      BlockAllocator *blockAllocator = new BlockAllocator(10);
      //
      // allocate lots of blocks
      //
      for (size_t j = 0; j < 25; j++) {
        blockAllocator->addNewBlock();
        prevPtrs[j] = blockAllocator->curAllocationByte;
        AssertThat(blockAllocator->blocks.getNumItems(), Is().EqualTo(j+1));
        for (size_t i = 0; i <= j; i++) {
         AssertThat(blockAllocator->blocks.getItem(i, NULL), Is().Not().EqualTo((void*)0));
         AssertThat(blockAllocator->blocks.getItem(i, NULL), Is().EqualTo(prevPtrs[i]));
        }
      }
      //
      // clear them all
      //
      blockAllocator->clearBlocks();
      //
      // now allocate some more
      //
      for (size_t j = 0; j < 25; j++) {
        blockAllocator->addNewBlock();
        prevPtrs[j] = blockAllocator->curAllocationByte;
        AssertThat(blockAllocator->blocks.getNumItems(), Is().EqualTo(j+1));
        for (size_t i = 0; i <= j; i++) {
         AssertThat(blockAllocator->blocks.getItem(i, NULL), Is().Not().EqualTo((void*)0));
         AssertThat(blockAllocator->blocks.getItem(i, NULL), Is().EqualTo(prevPtrs[i]));
        }
      }
      delete blockAllocator;
    });

    it("AllocateNewStructure should allocate some new structures", [&](){
      BlockAllocator *blockAllocator = new BlockAllocator(10);
      for (size_t j = 0; j < 25; j++) {
        char *aStructure = blockAllocator->allocateNewStructure(2);
        AssertThat(aStructure, Is().Not().EqualTo((char*)0));
      }
      AssertThat(blockAllocator->blocks.getNumItems(), Is().EqualTo(5));
      size_t i = 0;
      for ( ; i < 5; i++) {
        AssertThat(blockAllocator->blocks.getItem(i, NULL), Is().Not().EqualTo((char*)0));
      }
      for ( ; i < blockAllocator->blocks.getNumItems(); i++) {
        AssertThat(blockAllocator->blocks.getItem(i, NULL), Is().EqualTo((char*)0));
      }
      delete blockAllocator;
    });

  }); // describe BlockAllocator

});

