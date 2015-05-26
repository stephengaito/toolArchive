#include <bandit/bandit.h>
using namespace bandit;

#include <string.h>
#include <stdio.h>
#include <exception>

#ifndef private
#define private public
#endif

#include <symbols.h>

go_bandit([](){

  printf("\n----------------------------------\n");
  printf(  "symbols\n");
  printf(  "    Symbols = %zu bytes (%zu bits)\n", sizeof(Symbols), sizeof(Symbols)*8);
  printf(  "SymbolEntry = %zu bytes (%zu bits)\n", sizeof(Symbols::SymbolEntry), sizeof(Symbols::SymbolEntry)*8);
  printf(  "----------------------------------\n");

  /// \brief We test the correctness of the symbol tables class.
  describe("symbols", [](){

    it("should be able to create a new Symbols object", [&](){
      Symbols *symbols = new Symbols();
      AssertThat(symbols, Is().Not().EqualTo((void*)0));
      delete symbols;
    });

    it("should be able to delete a Symbols object", [&](){
      Symbols *symbols = new Symbols();
      AssertThat(symbols, Is().Not().EqualTo((void*)0));
      delete symbols;
      AssertThat(symbols, Is().Not().EqualTo((void*)0));
    });

    it("registerSymbol should be able to register a new symbol", [&](){
      Symbols *symbols = new Symbols();
      AssertThat(symbols, Is().Not().EqualTo((void*)0));
      char symbol[] = "simple";
      Symbols::SymbolEntry *entry =
        symbols->registerSymbol(symbol, strlen(symbol));
      AssertThat(entry, Is().Not().EqualTo((void*)0));
      delete symbols;
    });

    it("registerSymbol should return any already registered symbol", [&](){
      Symbols *symbols = new Symbols();
      AssertThat(symbols, Is().Not().EqualTo((void*)0));
      char symbol[] = "simple";
      Symbols::SymbolEntry *entry0 =
        symbols->registerSymbol(symbol, strlen(symbol));
      AssertThat(entry0, Is().Not().EqualTo((void*)0));
      Symbols::SymbolEntry *entry1 =
        symbols->registerSymbol(symbol, strlen(symbol));
      AssertThat(entry1, Is().Not().EqualTo((void*)0));
      AssertThat(entry0, Is().EqualTo(entry1));
      delete symbols;
    });

    it("getSymbol should return an already registered symbol", [&](){
      Symbols *symbols = new Symbols();
      AssertThat(symbols, Is().Not().EqualTo((void*)0));
      char symbol[] = "simple";
      Symbols::SymbolEntry *entry0 =
        symbols->registerSymbol(symbol, strlen(symbol));
      AssertThat(entry0, Is().Not().EqualTo((void*)0));
      Symbols::SymbolEntry *entry1 =
        symbols->getSymbol(symbol, strlen(symbol));
      AssertThat(entry1, Is().Not().EqualTo((void*)0));
      AssertThat(entry0, Is().EqualTo(entry1));
      delete symbols;
    });

    it("getSymbol should return NULL if the symbol is not registered", [&](){
      Symbols *symbols = new Symbols();
      AssertThat(symbols, Is().Not().EqualTo((void*)0));
      char symbol[] = "simple";
      Symbols::SymbolEntry *entry =
        symbols->getSymbol(symbol, strlen(symbol));
      AssertThat(entry, Is().EqualTo((void*)0));
      delete symbols;
    });

  }); // describe symbols

});

