#include <bandit/bandit.h>
using namespace bandit;

#include <string.h>
#include <stdio.h>
#include <exception>

#ifndef private
#define private public
#endif

#include <stdio.h>
#include <varArray.h>

go_bandit([](){

  printf("\n----------------------------------\n");
  printf(  "varArray\n");
  printf(  "        VarArray<int> = %zu bytes (%zu bits)\n", sizeof(VarArray<int>), sizeof(VarArray<int>)*8);
  printf(  "VarArray<const char*> = %zu bytes (%zu bits)\n", sizeof(VarArray<const char*>), sizeof(VarArray<const char*>)*8);
  printf(  "----------------------------------\n");

  /// \brief We test the correctness of the C-based BlockAllocator structure.
  ///
  describe("VarArray", [](){

    it("should be created with correct values when instantiated with int", [](){
      VarArray<int> aVarArray;
      AssertThat(aVarArray.numItems,  Equals(0));
      AssertThat(aVarArray.arraySize, Equals(0));
      AssertThat(aVarArray.itemArray, Equals((void*)0));
    });

    it("should be able to push and pop lots of items when instantiated with int", [](){
      VarArray<int> aVarArray;
      AssertThat(aVarArray.numItems,  Equals(0));
      AssertThat(aVarArray.arraySize, Equals(0));
      AssertThat(aVarArray.itemArray, Equals((void*)0));
      for (size_t i = 0; i < 100; i++) {
        aVarArray.pushItem(i);
        AssertThat(aVarArray.getNumItems(), Equals(i+1));
        AssertThat(aVarArray.arraySize, Is().Not().EqualTo(0));
        AssertThat(aVarArray.itemArray[i], Equals(i));
      }
      //
      // test copyItems with a large buffer size
      //
      int someInts[200];
      for (size_t i = 0; i < 200; i++) someInts[i] = 0;
      aVarArray.copyItems(someInts, 200*sizeof(int));
      for (size_t i = 0; i < 100; i++) {
        AssertThat(someInts[i], Equals(i));
      }
      for (size_t i = 100; i < 200; i++) {
        AssertThat(someInts[i], Equals(0));
      }
      //
      // test copyItems with a small non-multiple sizeof(int) buffer size
      for (size_t i = 0; i < 200; i++) someInts[i] = 0;
      aVarArray.copyItems(someInts, (51*sizeof(int)-2));
      for (size_t i = 0; i < 50; i++) {
        AssertThat(someInts[i], Equals(i));
      }
      for (size_t i = 50; i < 200; i++) {
        AssertThat(someInts[i], Equals(0));
      }
      size_t arraySize = aVarArray.arraySize;
      for(size_t i = 100; 0 < i; i--) {
        AssertThat(aVarArray.popItem(), Equals(i-1));
        AssertThat(aVarArray.getNumItems(), Equals(i-1));
        AssertThat(aVarArray.arraySize, Equals(arraySize));
      }
      AssertThat(aVarArray.getNumItems(), Equals(0));
      AssertThat(aVarArray.arraySize, Equals(arraySize));
    });

    it("should be created with correct values when instantiated with const char*", [](){
      VarArray<const char*> aVarArray;
      AssertThat(aVarArray.numItems,  Equals(0));
      AssertThat(aVarArray.arraySize, Equals(0));
      AssertThat(aVarArray.itemArray, Equals((void*)0));
    });

    it("should be able to push/pop and swap lots of items when instantiated with const char*", [](){
      VarArray<int> aVarArray;
      AssertThat(aVarArray.numItems,  Equals(0));
      AssertThat(aVarArray.arraySize, Equals(0));
      AssertThat(aVarArray.itemArray, Equals((void*)0));
      for (size_t i = 0; i < 100; i++) {
        aVarArray.pushItem(i);
        AssertThat(aVarArray.getNumItems(), Equals(i+1));
        AssertThat(aVarArray.arraySize, Is().Not().EqualTo(0));
        AssertThat(aVarArray.itemArray[i], Equals(i));
      }
      AssertThat(aVarArray.numItems,      Equals(100));
      AssertThat(aVarArray.itemArray[99], Equals(99));
      AssertThat(aVarArray.itemArray[98], Equals(98));
      aVarArray.swapTopTwoItems();
      AssertThat(aVarArray.itemArray[99], Equals(98));
      AssertThat(aVarArray.itemArray[98], Equals(99));
      aVarArray.swapTopTwoItems();
      AssertThat(aVarArray.itemArray[99], Equals(99));
      AssertThat(aVarArray.itemArray[98], Equals(98));
      size_t arraySize = aVarArray.arraySize;
      for(size_t i = 100; 0 < i; i--) {
        AssertThat(aVarArray.popItem(), Equals(i-1));
        AssertThat(aVarArray.getNumItems(), Equals(i-1));
        AssertThat(aVarArray.arraySize, Equals(arraySize));
      }
      AssertThat(aVarArray.getNumItems(), Equals(0));
      AssertThat(aVarArray.arraySize, Equals(arraySize));
    });

    it("should be able to push/pop and swap lots of items when instantiated with const char*", [](){
      VarArray<const char*> aVarArray;
      AssertThat(aVarArray.numItems,  Equals(0));
      AssertThat(aVarArray.arraySize, Equals(0));
      AssertThat(aVarArray.itemArray, Equals((void*)0));
      for (size_t i = 0; i < 100; i++) {
        aVarArray.pushItem((char*)i);
        AssertThat(aVarArray.getNumItems(), Equals(i+1));
        AssertThat(aVarArray.arraySize, Is().Not().EqualTo(0));
        AssertThat(aVarArray.itemArray[i], Equals((char*)i));
      }
      AssertThat(aVarArray.numItems,      Equals(100));
      AssertThat(aVarArray.itemArray[99], Equals((char*)99));
      AssertThat(aVarArray.itemArray[98], Equals((char*)98));
      aVarArray.swapTopTwoItems();
      AssertThat(aVarArray.itemArray[99], Equals((char*)98));
      AssertThat(aVarArray.itemArray[98], Equals((char*)99));
      aVarArray.swapTopTwoItems();
      AssertThat(aVarArray.itemArray[99], Equals((char*)99));
      AssertThat(aVarArray.itemArray[98], Equals((char*)98));
      size_t arraySize = aVarArray.arraySize;
      for(size_t i = 100; 0 < i; i--) {
        AssertThat(aVarArray.popItem(), Equals((char*)i-1));
        AssertThat(aVarArray.getNumItems(), Equals(i-1));
        AssertThat(aVarArray.arraySize, Equals(arraySize));
      }
      AssertThat(aVarArray.getNumItems(), Equals(0));
      AssertThat(aVarArray.arraySize, Equals(arraySize));
    });

  }); // describe VarArray
});
