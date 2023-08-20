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

## Intermediate Example
The VENT source below: <br/>

```vhdl
use ieee.std_logic_1164.all;

ent clk_divider{
    DIV int := 8;
    clk -> stl;
    oclk <- stl;
}

arch divider(clk_divider){
    sig modClk stl := '0';
    proc(clk){
        var count int := 0;
        if(clk'UP){
            if(count < DIV){
                count++;
            } else {
                count := 0;
                modClk <= not modClk;
            }
        }
    }   
    oclk <= modClk; 
}

use ieee.std_logic_1164.all;

ent spi {
    WIDTH int := 11;
    clk -> stl;
    start -> stl;
    din -> stlv(WIDTH downto 0);
    cs <- stl;
    mosi <- stl;
    done <- stl;
    sclk <- stl;
}

arch behavioral(spi){
    type controllerState {idle, tx_start, send, tx_end};

    comp clk_divider{
        DIV int := 8;
        clk -> stl;
        oclk <- stl;
    }   

    sig state controllerState;
    sig sclkt stl := '0';
    sig temp stlv(WIDTH downto 0);

    D1: clk_divider map (10, clk, sclkt);

    proc(sclkt){
        var bitcount int := 0;
        if(sclkt'UP){
            switch(state) {
                case idle:
                    mosi <= '0';
                    cs <= '0';
                    done <= '0';
                    if(start == '1'){
                        state <= tx_start;
                    } else {
                        state <= idle;
                    }
                case tx_start:
                    cs <= '0';
                    temp <= din;
                    state <= send;	
                case send:
                    if(bitcount <= WIDTH){
                        bitcount++;
                        mosi <= temp(bitcount);
                        state <= send;
                    } else {
                        bitcount := 0;
                        state <= tx_end;
                        mosi <= '0';
                    }
                case tx_end:
                    cs <= '1';
                    state <= idle;
                    done <= '1';
                default:
                    state <= idle;
            }
        } 
    }

    sclk <= sclkt;
}
```

will be transpiled to the following VHDL: <br/>

```vhdl
library ieee;
use ieee.std_logic_1164.all;

entity clk_divider is
    generic(
        DIV: integer := 8   
        );
    port(
        clk: in std_logic;
        oclk: out std_logic
    );  
end clk_divider;

architecture divider of clk_divider is
    signal modClk: std_logic := '0';
begin

    process (clk) is  
        variable count: integer := 0;
    begin
        if rising_edge(clk) then
            if count < DIV then
                count := count + 1;
            else
                count := 0;
                modClk <= not modClk;
            end if; 
        end if; 
    end process;

    oclk <= modClk;

end architecture divider;

library ieee;
use ieee.std_logic_1164.all;

entity spi is
    generic(
        WIDTH: integer := 11    
        );
    port(
        clk: in std_logic;
        start: in std_logic;
        din: in std_logic_vector(WIDTH downto 0);
        cs: out std_logic;
        mosi: out std_logic;
        done: out std_logic;
        sclk: out std_logic
    );
end spi;

architecture behavioral of spi is
    type controllerState is ( idle, tx_start, send, tx_end);
    component clk_divider is
        generic(
            DIV: integer := 8
        );
        port(
            clk: in std_logic;
            oclk: out std_logic
        );
    end component;
    signal state: controllerState;
    signal sclkt: std_logic := '0';
    signal temp: std_logic_vector(WIDTH downto 0);
begin
    D1: clk_divider
        generic map ( DIV => 10
        )
        port map ( clk => clk,
             oclk => sclkt
        );

    process (sclkt) is 
        variable bitcount: integer := 0;
    begin
        if rising_edge(sclkt) then
            case state is
                when idle =>
                    mosi <= '0';
                    cs <= '0';
                    done <= '0';
                    if start = '1' then
                        state <= tx_start;
                    else
                        state <= idle;
                    end if;
                when tx_start =>
                    cs <= '0';
                    temp <= din;
                    state <= send;
                when send =>
                    if bitcount <= WIDTH then
                        bitcount := bitcount + 1;
                        mosi <= temp(bitcount);
                        state <= send;
                    else
                        bitcount := 0;
                        state <= tx_end;
                        mosi <= '0';
                    end if;
                when tx_end =>
                    cs <= '1';
                    state <= idle;
                    done <= '1';
                when others =>
                    state <= idle;
            end case;
        end if;
    end process;

    sclk <= sclkt;

end architecture behavioral;
```

## TVT
TVT is the program used to transpile VENT into VHDL. TVT is written in pure C and is composed of a handful of modules. The major stages that TVT goes through to get from VENT to VHDL are shown below. 

### Stage 1
![Image](/docs/stage1.png)

### Stage 2
![Image](/docs/stage2.png)

### Build & Run
To build TVT just run `make` in the code directory. Run `make help` for more options e.g. cleaning or running the unit tests. TVT uses GCC and the CuTest unit testing framework. <br/>

Once built, run TVT against a VENT file: <br/>
`./tvt ander.vent`<br/>
<br/>
NOTE: There is a Vim configuration file in the docs directory for VENT syntax highlighting. 

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
type: TOKEN_LESS_EQUAL     literal: <=
type: TOKEN_IDENTIFIER     literal: a
type: TOKEN_AND            literal: and
type: TOKEN_IDENTIFIER     literal: b
type: TOKEN_SCOLON         literal: ;
type: TOKEN_IDENTIFIER     literal: y
type: TOKEN_LESS_EQUAL     literal: <=
type: TOKEN_IDENTIFIER     literal: temp
type: TOKEN_SCOLON         literal: ;
type: TOKEN_RBRACE         literal: }

```

printing the AST produced by the parser: <br/>
`./tvt ander.vent --print-ast` <br/>

```
Program
| UseStatement: ieee.std_logic_1164.all
| EntityDecl
|- Identifier: 'ander'
|- PortDecl
|-- Identifier: 'a'
|-- PortMode: '->'
|-- DataType: 'stl'
|- PortDecl
|-- Identifier: 'b'
|-- PortMode: '->'
|-- DataType: 'stl'
|- PortDecl
|-- Identifier: 'y'
|-- PortMode: '<-'
|-- DataType: 'stl'
| ArchDecl
|- Identifier: 'behavioral'
|- Identifier: 'ander'
|- SignalDecl
|-- Identifier: 'temp'
|-- DataType: 'stl'
|-- Expression: '0'
|- SignalAssign
|-- Identifier: 'temp'
|-- Expression: 'a and b'
|- SignalAssign
|-- Identifier: 'y'
|-- Expression: 'temp'

```
<br/>
<more options to come!>
