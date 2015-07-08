# Items to consider later:

Bob Nystrom's [Pratt Parsers: Expression Parsing Made 
Easy](http://journal.stuffwithstuff.com/2011/03/19/pratt-parsers-expression-parsing-made-easy/) 
could be used for a front-end to the parser.

Consider adding 
[Memoization](https://en.wikipedia.org/wiki/Memoization) to help speed 
up the parser.

Change TokenArray back to VarArray<Token> from VarArray<Token*> to help 
improve the locality and hence cache usage.

Review the creation/deletion of Tokens to help reduce the churn on the 
heap and hence improve performance.
