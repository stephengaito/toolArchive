# To Do

# Adding infix operator precedence

## Coarse grained precedence

If it is acceptable to have a small number of predefined infix operator 
precedences, then we can do this by defining extra Lexer Tokens (one 
for each predefined infix operator type), and then have the lexer parse 
all infix operators to a small collection of "base" tokens. Then 
subclass the CommonTokenFactory so that the create method switches the 
token type dynamically at runtime given a runtime specified infix 
operator table. Then replace the lexer's tokenFactory with the new 
switching tokenFactory.

## Fine grained percedence

To get finer grained infix operator precedence, we will need to 
annotate the CommonToken to include a precedence indicator and then 
proceed as above.  In the grammar's parser proper we will then need to 
make use of semantic predicates which make use of the annotated token's 
precedence indicator to ensure the infix operators have the correct 
precedence.
