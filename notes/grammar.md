### 1. token

* keyword
* identifier
* constant
* string-literal
* punctuator

### 2. Keywords

* auto
* break
* case
* char
* const
* continue
* default
* do
* double
* else
* enum
* extern
* float
* for
* goto
* if
* inline
* int
* long
* register
* restrict
* return
* short
* signed
* sizeof
* static
* struct
* switch
* typedef
* union
* unsigned
* void
* volatile
* while
* _Bool
* _Complex
* _Imaginary

### 3. Identifiers

* identifier:
  * nondigit
  * identifier 	nondigit
  * identifier 	digit
  * nondigit: one of: _/a...z/A...Z
  * digit: one of: 0...9

### 4. Expressions

* primary-expression

  * identifier
  * constant
  * string-literal
  * (expression)

* postfix-expression

  * primary-expression
  * postfix-expression(argument-expression-list)
  * postfix-expression++
  * postfix-expression--

* argument-expression-list

  * assignment-expression
  * argument-expression-list assignment-expression

* unary-expression

  * postfix-expression
  * ++unary-expression
  * --unary-expression
  * unary-operator cast-expression
  * sizeof(type-name)

* unary-operator one of

  & * + - ~ !

* cast-expression:

  * unary-expression
  * ( type-name ) cast-expression

* multiplicative-expression

  * cast-expression
  * multiplicative-expression * cast-expression
  * multiplicative-expression / cast-expression
  * multiplicative-expression % cast-expression

* additive-expression

  * multiplicative-expression
  * additive-expression + multiplicative-expression
  * additive-expression - multiplicative-expression

* shift-expression

  * additive-expression
  * shift-expression << additive-expression
  * shift-expression >> additive-expression

* relational-expression

  * shift-expression
  * relational-expression < shift-expression
  * relational-expression > shift-expression
  * relational-expression <= shift-expression
  * relational-expression >= shift-expression

* equality-expression

  * relational-expression
  * equality-expression == relational-expression
  * equality-expression != relational-expression

* AND-expression

  * equality-expression
  * AND-expression & equality-expression

* exclusive-OR-expression

  * AND-expression
  * exclusive-OR-expression ^ AND-expression

* inclusive-OR-expression

  * exclusive-OR-expression
  * inclusive-OR-expression | exclusive-OR-expression

* logical-AND-expression

  * inclusive-OR-expression
  * logical-AND-expression && inclusive-OR-expression

* logical-OR-expression

  * logical-AND-expression
  * logical-OR-expression || logical-AND-expression

* conditional-expression

  * logical-OR-expression
  * logical-OR-expression ? expression : conditional-expression

* assignment-expression

  * conditional-expression
  * unary-expression assignment-operator assignment-expression

* assignment-operator: one of

  = *= /= %= += -= <<= >>= &= ^= |=

* expression

  * assignment-expression
  * expression, assignment-expression

* constant-expression:

  * conditional-expression

### 5. Declarations

declaration:
​	type-specifier init-declarator-list ;

init-declarator-list:
​	init-declarator
​	init-declarator-list , init-declarator

init-declarator:
​	declarator
​	declarator = initializer

declarator:
​	identifier
​	declarator( parameter-type-list )

parameter-type-list:
​	parameter-list
​	parameter-list , ...

parameter-list:
​	parameter-declaration
​	parameter-list , parameter-declaration

parameter-declaration:
​	type-specifier identifier

type-specifier

- void
- char
- int
- double

### 6. Statements

* statement
  * compound-statement
  * expression-statement
  * selection-statement
  * iteration-statement
  * jmp-statement
* compound-statement
  * {block-item-list}
* block-item-list
  * block-item
  * block-item-list
* block-item
  * declaration
  * statement
* expression-statement
  * expression
* selection-statement
  * if(expression) statement
  * if(expression) statement else statement
* iteration-statement
  * while(expression) statement
  * for(expression;expression;expression) statement
  * for(declaration;expression;expression) statement
* jump-statement
  * continue;
  * break
  * return expression;

### 7. External definitions

* function-definition
  * declaration-specifiers declarator declaration-list compound-statement
* declaration-list
  * declaration
  * declaration-list declaration