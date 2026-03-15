
# Introduction:
***Tier*** is a language based on easy scripting yet fast performance,
it is a static compiled language. It focuses on clear error reporting.

# 1.0: Expressions:
Expressions are the core of ***Tier***, everything depends
on the definition of expressions.

## Definition of a 'DEFINITION':
- Names may be assigned but are optional.
- MUST be known at compile-time.
- It MUST be able to produce an output (a type with no value is considered as a valid output) with a static type, 
  even if an output is NEVER produced.
- If a definition has input value(s), each input MUST have a fully determined static type.
- Its static type, MUST stay immutable for its lifetime.

Second, a declaration is the act of binding a definition to a name

With those defined, we can now define EXPRESSIONs:
- Any expression MUST exist at compile-time.
- Any expression MUST have a DEFINITION (whether it could be inferred by the compiler or not)

It differs from an expression because it IS the output of a definition.

## ex: 
`5 + 3` is an expression, 
- Is known to exist at compile time.
- It produces an output (type is inferred as `i32` (unless default changed see 3.2); value = 8).
- Its type will not change for its lifetime.
- It has a definition but not a declaration — it has no name bound.



# 2.0: Types:
The type system is split into types and type groups,

∙Types are concrete and fully specified and are the static type for all EXPRESSIONs (see 1.0).

∙Type groups are a reference to a range of types, they are used 	
 for broad runtime type-checking and multiple `#set` settings such as inference.
 ex: 
     `Integer` refers to, `i8`, `u8`, ... `i64`, `u64`
     `'#set infer Integer to i64'` changes the inference 
     of the `Integer` type group to a `i64` (a type).

All type groups and types:
  - `Integer` → `i8`, `u8`, `i16`, `u16`, `i32`, `u32`, `i64`, `u64`
  - `Float` → `f32`, `f64`
  - `Bool` → `bool`
  - `String` →?
  - `Char` → `char`
    
  - It is possible to make your own type groups:
    `'#set typegroup' IDENT '=' TYPE(s) ';'`
    or
    `'#enforce typegroup' IDENT '=' TYPE(s) ';'`
    ex: 
    

    Type definitions:
      - `i8`, `i16`, `i32`, `i64`: are signed integers (they can be positive and negative) with a bit size 
                           corresponding to the number found in their name, ex: `i16` is 16 bits (2 bytes).

      - `u8`, `u16`, `u32`, `u64`: are unsigned integers (they can ONLY be positive) with a bit size 
                           corresponding to the number found in their name, ex: `u16` is 16 bits (2 bytes).

      - `f32`, `f64`: are floating point numbers (they can have decimals).

      - `bool`: corresponds to i8 integers 0 and 1 masked as false (0) and true (1).

      - `void`: raw memory, type not checked by the compiler.

      - `String`: part of the STL.
      

# 3.0: Declarations:
`'let' ['mut'] IDENT [':' TYPE] '=' EXPRESSION ';'`

  - type is inferred unless `(':' TYPE)` was found.
  - immutable by default unless `mut` specified.

  ex: 
  - `let x: i32 = 5;`      // immutable, type is not inferred (`i32` was specified).
  - `let x = 5;`          // immutable, type is inferred (`i32` is inferred, see 3.2).
  - `let mut x: i32 = 5;`// mutable, type is not inferred (`i32` was specified).
  - `let mut x = 5;`    // mutable, type is inferred (`i32` is inferred, see 3.2).

## 3.1: \#set and \#enforce:
  [insert lang name] has a very powerful feature, you can tell the compiler what to do using `#set` and `#enforce`.
  You can tell the compiler how to infer types (see 3.2), and ... (WIP). 
  - Both `#set` and `#enforce` are not affected by scopes, they affect the lines after them.
  - `#set` only affects the file its in, `#enforce` can affect multiple files if it is in a {header(wip)}.
  - `#enforce` cannot be changed by `#set`.

## 3.20: Type inference:
- The type is inferred at declaration (definition MUST be present at declaration).

- It MUST be inferred from an EXPRESSION (see 1.0) that has a static and fully determinable type.

- Once a type is inferred, it will NEVER change its type.

- It checks ALL types that could work, and chooses the default based on typegroup (see 3.11).

- Different type groups have a default, ex: integers default to `i32`.	
  Default may be changed through: 
  `'#set infer' TYPE_GROUP 'to' TYPE ';'`
  or 
  `'#enforce infer' TYPE_GROUP 'to' TYPE ';'`
  * Everything after it will use that default but NOT before it, it is not affected by scopes.
  Unless defaults are set, then the defaults are as follows:
    - `Integer` → `i32`
    - `Float` → `f64`
    - `Strings` → `String`

  ## 3.21:
  - If you make your own type group, and overwrite it with overlapping types in existing typegroup then do:
    `'#set infer' TYPE_GROUP 'instead' TYPE_GROUP ';'`
    or
    `'#enforce infer' TYPE_GROUP 'instead' TYPE_GROUP ';'`
  ex:
      ```
      #set typegroup Unsigned_Integer = u8, u16, u32, u64; 
      #set infer Unsigned_Integer to u32;
      #set infer Unsigned_Integer instead Integer;
      let unsigned = 32;   // u32 is inferred.
      let negative = -32; // i32 is inferred.
      ```
      
       // `u8`, `u16`, `u32`, `u64` will be inferred with Unsigned_Integer, since it overlaps with Integer.
      // `i8`, `i16`, `i32`, `i64` will be inferred with Integer, since it doesn't overlap with Unsigned_Integer.

  - If a value has multiple types that span multiple type groups, then there is a priority list.
    the difference between it and overlapping types is that 
    this makes a type be assigned to that type group instead of the other, whilst for example
    5 could be a floating point number not only an integer, in default the list goes as so
    (note that vertically means they are the same priority):
    - `V::Arithmetic`: `Integer` > `Float`
    - `V::Strings`: `String` > (still figuring out strings)
    - `V::Bool`: `Bool`
    - `V::Other`

    These are the only value groups, they CANNOT be created.
    You may have noticed they are put into groups starting with `V::`, these are value groups, they contain type groups,
    they are critical for the priority system, they are denoted with `V::`, but note that they belong to the type inference system but NOT the type system.
    In default when making a type group, they are assigned to `V::Other`, to assign a type group in any value group at any priority rank then follow this syntax:
      `'#insert' TYPE_GROUP 'in' VALUE_GROUP 'as rank' (Integer Value) ';'`
    Note the top/first priority is 1.
    ex:
      ```
      #insert Float in V::Numbers as rank 1;
      let x = 4; // inferred as f64.
      ```

# 4.0: Functions and Templates:
  ## WIP
  Function syntax is similar to Rust:
  ```
  fn add(x: i32, y: i32) -> i32 {
    return x + y;
  }
  ```
  ommiting `->` is equal to `-> void`.
  Tier templates are made to be clear and simple, they go in hand with the type inference system using type groups and value groups, for example:
  ```
  fn add<T>(x: T, y: T) -> T {
    return x + y;
  }
  ```
  or
  ```
  fn add<T: Integer>(x: T, y: T) -> T {
    return x + y;
  }
  ```
  or
  ```
  fn add<T: V::Arithmetic>(x: T, y: T) -> T {
    return x + y;
  }
  ```

  ## WIP
  #set promotion \
  #set explicit \
  #set explicit \

  ## WIP
  generics system
  ```
  constraint<T> quack {
    can T.quack();
    exist T.health;
  } 
  ```

# 5.0: Memory:
  ## SECTION IN WIP.
  ## 5.1: References and Pointers.


  ## SECTION IN WIP.
  ## 5.2: ***Tier***ed Memory:
  Memory is also ***Tier***ed.
  * `#enforce/set memory_check off`: raw memory control, compiler enforces no memory checks, best when performance is key.
  * `#enforce/set memory_check on`: 
    - The default setting.
    - Tracks ownership, prevents use after being freed.
    - Checks for dangling pointers.
  * `#enforce/set memory_check strict`:
    - Everything in on plus:
    - Requires null checks before dereferencing.
    - Single mutable reference at any given time.
    - Full-lifetime tracking.
    - Compile-time guaranteed safety.