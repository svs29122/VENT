# TVT - The VENT Transpiler

**V**HDL<br/>
**E**xcept<br/>
**N**ot<br/>
**T**errible<br/>

VENT is a Hardware Description Language designed to transpile to VHDL. VENT uses a simplified 'c-like' syntax, along with fewer and shorter keywords than VHDL. The major design goal of VENT is to create a friendlier, less verbose version of VHDL. 

NOTE: the VENT grammar is still being defined and is a WIP

## Simple Example
The VENT source below: <br/>

```vhdl
use ieee.std_logic_1164.all;

ent ander {
    a -> stl;
    b -> stl;
    y <- stl;
}

arch behavioral(ander) {
    sig temp stl := '0';

    temp <= a and b;
    y <= temp;
} 
```

will be transpiled to the following VHDL: <br/>

```vhdl
library ieee;
use ieee.std_logic_1164.all;

entity ander is
    port(
        a: in std_logic;
        b: in std_logic;
        y: out std_logic
    );
end ander;

architecture behavioral of ander is

    signal temp: std_logic := '0';

begin

    temp <= a and b;
    y <= temp;

end architecture behavioral;
```

## TVT
TVT is the program used to transpile VENT into VHDL. TVT is written in pure C and is composed of a handful of modules. The major stages that TVT goes through to get from VENT to VHDL are shown below. 

### Stage 1
![Image](/spec/stage1.png)

### Stage 2
![Image](/spec/stage2.png)

### Build & Run
To build TVT just run `make` in the code directory. Run `make help` for more options e.g. cleaning or running the unit tests. TVT uses GCC and the CuTest unit testing framework. <br/>

Once built, run TVT against a VENT file: <br/>
`./tvt ander.vent`<br/>

#### Other options include: <br/>
printing all of the tokens produced by the lexer: <br/>
`./tvt  ander.vent --print-tokens` <br/>

```
type: TOKEN_USE            literal: use
type: TOKEN_IDENTIFIER     literal: ieee.std_logic_1164.all
type: TOKEN_SCOLON         literal: ;
type: TOKEN_ENT            literal: ent
type: TOKEN_IDENTIFIER     literal: ander
type: TOKEN_LBRACE         literal: {
type: TOKEN_IDENTIFIER     literal: a
type: TOKEN_INPUT          literal: ->
type: TOKEN_STL            literal: stl
type: TOKEN_SCOLON         literal: ;
type: TOKEN_IDENTIFIER     literal: b
type: TOKEN_INPUT          literal: ->
type: TOKEN_STL            literal: stl
type: TOKEN_SCOLON         literal: ;
type: TOKEN_IDENTIFIER     literal: y
type: TOKEN_OUTPUT         literal: <-
type: TOKEN_STL            literal: stl
type: TOKEN_SCOLON         literal: ;
type: TOKEN_RBRACE         literal: }
type: TOKEN_ARCH           literal: arch
type: TOKEN_IDENTIFIER     literal: behavioral
type: TOKEN_LPAREN         literal: (
type: TOKEN_IDENTIFIER     literal: ander
type: TOKEN_RPAREN         literal: )
type: TOKEN_LBRACE         literal: {
type: TOKEN_SIG            literal: sig
type: TOKEN_IDENTIFIER     literal: temp
type: TOKEN_STL            literal: stl
type: TOKEN_VASSIGN        literal: :=
type: TOKEN_CHARLIT        literal: 0
type: TOKEN_SCOLON         literal: ;
type: TOKEN_IDENTIFIER     literal: temp
type: TOKEN_SASSIGN        literal: <=
type: TOKEN_IDENTIFIER     literal: a
type: TOKEN_AND            literal: and
type: TOKEN_IDENTIFIER     literal: b
type: TOKEN_SCOLON         literal: ;
type: TOKEN_IDENTIFIER     literal: y
type: TOKEN_SASSIGN        literal: <=
type: TOKEN_IDENTIFIER     literal: temp
type: TOKEN_SCOLON         literal: ;
type: TOKEN_RBRACE         literal: }
```

printing the AST produced by the parser: <br/>
`./tvt ander.vent --print-ast` <br/>

```
Program
| UseStatement: ieee.std_logic_1164.all
| DesignUnit
|- EntityDecl
|-- Identifier: 'ander'
|-- PortDecl
|--- Identifier: 'a'
|--- PortMode: '->'
|--- DataType: 'stl'
|-- PortDecl
|--- Identifier: 'b'
|--- PortMode: '->'
|--- DataType: 'stl'
|-- PortDecl
|--- Identifier: 'y'
|--- PortMode: '<-'
|--- DataType: 'stl'
| DesignUnit
|- ArchDecl
|-- Identifier: 'behavioral'
|-- Identifier: 'ander'
|-- SignalDecl
|--- Identifier: 'temp'
|--- DataType: 'stl'
|--- Expression: '0'
|-- SignalAssign
|--- Identifier: 'temp'
|--- Expression: 'a and b'
|-- SignalAssign
|--- Identifier: 'y'
|--- Expression: 'temp'
```
<br/>
<more options to come!>
