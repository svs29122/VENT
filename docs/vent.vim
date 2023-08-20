" Vim syntax file
" Language:		VENT [VHDL Except Not Terrible]
" Maintainer:	XXXX	
" Previous Maintainer: XXXX	
" Credits:		Stephen S. 
" Last Changed:		2023 Aug XX by Stephen S.

" *****  IMPORTANT *****
" * add the lines below to filetype.vim
" * a good place is directly after VHDL 

" VENT
" au BufNewFile,BufRead *.vent  setf vent


" quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

let s:cpo_save = &cpo
set cpo&vim

" case is not significant
syn case	ignore

" VENT keywords
syn keyword		ventStatement	access after alias all assert
syn keyword 	ventStatement	architecture arch array attribute
syn keyword 	ventStatement	assume assume_guarantee
syn keyword 	ventStatement	begin block body buffer bus break
syn keyword 	ventStatement	case comp component configuration constant
syn keyword 	ventStatement	context cover cont
syn keyword 	ventStatement	default disconnect downto
syn keyword 	ventStatement	elsif end ent entity exit
syn keyword 	ventStatement	file for function
syn keyword 	ventStatement	fairness force
syn keyword 	ventStatement	generate generic group guarded
syn keyword 	ventStatement	impure in inertial inout is
syn keyword 	ventStatement	label library linkage literal loop
syn keyword 	ventStatement	map
syn keyword 	ventStatement	new next null
syn keyword 	ventStatement	of on open others out
syn keyword 	ventStatement	package port postponed procedure process proc pure
syn keyword 	ventStatement	parameter property protected
syn keyword 	ventStatement	range record register reject report return
syn keyword 	ventStatement	release restrict restrict_guarantee
syn keyword 	ventStatement	select severity signal sig shared
syn keyword 	ventStatement	subtype
syn keyword 	ventStatement	sequence strong switch
syn keyword 	ventStatement	then to transport type
syn keyword 	ventStatement	unaffected units until use
syn keyword 	ventStatement	variable var
" VHDL-2017 interface
syn keyword 	ventStatement	view
syn keyword 	ventStatement	vmode vprop vunit
syn keyword 	ventStatement	wait when while with
syn keyword 	ventStatement	note warning error failure

" Linting of conditionals.
syn match	ventStatement	"\<\(if\|else\)\>"
syn match	ventError	"\<else\s\+if\>"

" Types and type qualifiers
" Predefined standard VHDL types
syn match	ventType	"\<bit\>\'\="
syn match	ventType	"\<boolean\>\'\="
syn match	ventType	"\<bool\>\'\="
syn match	ventType	"\<natural\>\'\="
syn match	ventType	"\<positive\>\'\="
syn match	ventType	"\<int\>\'\="
syn match	ventType	"\<integer\>\'\="
syn match	ventType	"\<real\>\'\="
syn match	ventType	"\<time\>\'\="

syn match	ventType	"\<bit_vector\>\'\="
syn match	ventType	"\<boolean_vector\>\'\="
syn match	ventType	"\<integer_vector\>\'\="
syn match	ventType	"\<real_vector\>\'\="
syn match	ventType	"\<time_vector\>\'\="

syn match	ventType	"\<character\>\'\="
syn match	ventType	"\<string\>\'\="
"syn keyword	ventType	line text side width

" Predefined standard IEEE VHDL types
syn match	ventType	"\<stl\>\'\="
syn match	ventType	"\<std_ulogic\>\'\="
syn match	ventType	"\<std_logic\>\'\="
syn match	ventType	"\<std_ulogic_vector\>\'\="
syn match	ventType	"\<stlv\>\'\="
syn match	ventType	"\<std_logic_vector\>\'\="
syn match	ventType	"\<unresolved_signed\>\'\="
syn match	ventType	"\<unresolved_unsigned\>\'\="
syn match	ventType	"\<u_signed\>\'\="
syn match	ventType	"\<u_unsigned\>\'\="
syn match	ventType	"\<signed\>\'\="
syn match	ventType	"\<unsigned\>\'\="


" array attributes
syn match	ventAttribute	"\'high"
syn match	ventAttribute	"\'left"
syn match	ventAttribute	"\'length"
syn match	ventAttribute	"\'low"
syn match	ventAttribute	"\'range"
syn match	ventAttribute	"\'reverse_range"
syn match	ventAttribute	"\'right"
syn match	ventAttribute	"\'ascending"
" block attributes
syn match	ventAttribute	"\'simple_name"
syn match   	ventAttribute	"\'instance_name"
syn match   	ventAttribute	"\'path_name"
syn match   	ventAttribute	"\'foreign"	    " VHPI
" signal attribute
syn match	ventAttribute	"\'active"
syn match   	ventAttribute	"\'delayed"
syn match   	ventAttribute	"\'event"
syn match   	ventAttribute	"\'last_active"
syn match   	ventAttribute	"\'last_event"
syn match   	ventAttribute	"\'last_value"
syn match   	ventAttribute	"\'quiet"
syn match   	ventAttribute	"\'stable"
syn match   	ventAttribute	"\'transaction"
syn match   	ventAttribute	"\'driving"
syn match   	ventAttribute	"\'driving_value"
" type attributes
syn match	ventAttribute	"\'base"
syn match   	ventAttribute	"\'subtype"
syn match   	ventAttribute	"\'element"
syn match   	ventAttribute	"\'leftof"
syn match   	ventAttribute	"\'pos"
syn match   	ventAttribute	"\'pred"
syn match   	ventAttribute	"\'rightof"
syn match   	ventAttribute	"\'succ"
syn match   	ventAttribute	"\'val"
syn match   	ventAttribute	"\'image"
syn match   	ventAttribute	"\'value"
" VHDL-2017 interface attribute
syn match   	ventAttribute	"\'converse"
" VENT special attributes
syn match   	ventAttribute	"\'up"
syn match   	ventAttribute	"\'down"


syn keyword	ventBoolean	true false

" for this vector values case is significant
syn case	match
" Values for standard VHDL types
syn match	ventVector	"\'[0L1HXWZU\-\?]\'"
syn case	ignore

syn match	ventVector	"B\"[01_]\+\""
syn match   	ventVector	"O\"[0-7_]\+\""
syn match   	ventVector	"X\"[0-9a-f_]\+\""
syn match   	ventCharacter   "'.'"
syn region  	ventString	start=+"+  end=+"+

" floating numbers
syn match	ventNumber	"-\=\<\d\+\.\d\+\(E[+\-]\=\d\+\)\>"
syn match	ventNumber	"-\=\<\d\+\.\d\+\>"
syn match	ventNumber	"0*2#[01_]\+\.[01_]\+#\(E[+\-]\=\d\+\)\="
syn match	ventNumber	"0*16#[0-9a-f_]\+\.[0-9a-f_]\+#\(E[+\-]\=\d\+\)\="
" integer numbers
syn match	ventNumber	"-\=\<\d\+\(E[+\-]\=\d\+\)\>"
syn match	ventNumber	"-\=\<\d\+\>"
syn match	ventNumber	"0*2#[01_]\+#\(E[+\-]\=\d\+\)\="
syn match	ventNumber	"0*16#[0-9a-f_]\+#\(E[+\-]\=\d\+\)\="

" operators
syn keyword	ventOperator	and nand or nor xor xnor
syn keyword	ventOperator	rol ror sla sll sra srl
syn keyword	ventOperator	mod rem abs not

" Concatenation and math operators
syn match	ventOperator	"&\|+\|++\|+=\|-\|--\|-=\|\*\|\/"

" Equality and comparison operators
syn match	ventOperator	"=\|\/=\|>\|<\|>="

" Assignment operators
syn match	ventOperator	"<=\|:="
syn match	ventOperator	"=>"

" VHDL-2017 concurrent signal association (spaceship) operator
syn match	ventOperator	"<=>"

" VHDL-2008 conversion, matching equality/non-equality operators
syn match	ventOperator	"??\|?=\|?\/=\|?<\|?<=\|?>\|?>=\|=="

" VHDL-2008 external names
syn match	ventOperator	"<<\|>>"

" Linting for illegal operators
" '='
syn match	ventError	"\(=\)[<&+\-\*\/\\]\+"
syn match	ventError	"[&]\+\(=\)"
" '>', '<'
" Allow external names: '<< ... >>'
syn match	ventError	"\(>\)[<&+\-\/\\]\+"
syn match	ventError	"[>=&+\-\/\\]\+\(<\)"
" Covers most operators
" support negative sign after operators. E.g. q<=-b;
" Supports VHDL-2017 spaceship (concurrent simple signal association).
syn match	ventError	"\(<=\)[<=&+\*\\?:]\+"
syn match	ventError	"[>=&+\-\*\\:]\+\(=>\)"
syn match	ventError	"[<>=&+\-\*\\:]\+\(&\|\*\*\|\/=\|??\|?=\|?\/=\|?<\|?<=\|?>\|?>=\|>=\|<=\|:=\)"
syn match	ventError	"\(?<\|?>\)[<>&+\*\/\\?:]\+"
syn match	ventError	"\(<<\|>>\)[<>&+\*\/\\?:]\+"

"syn match	ventError	"[?]\+\(&\|+\|\-\|\*\*\|??\|?=\|?\/=\|?<\|?<=\|?>\|?>=\|:=\|=>\)"
" '/'
syn match	ventError	"\(\/\)[<>&+\-\*\/\\?:]\+"
syn match	ventError	"[<>=&+\-\*\/\\:]\+\(\/\)"

syn match	ventSpecial	"<>"
syn match	ventSpecial	"[().,;{}]"


" time
syn match	ventTime	"\<\d\+\s\+\(\([fpnum]s\)\|\(sec\)\|\(min\)\|\(hr\)\)\>"
syn match	ventTime	"\<\d\+\.\d\+\s\+\(\([fpnum]s\)\|\(sec\)\|\(min\)\|\(hr\)\)\>"

syn case	match
syn keyword	ventTodo	contained TODO NOTE
syn keyword	ventFixme	contained FIXME
syn case	ignore

syn region	ventComment	start="/\*" end="\*/"	contains=ventTodo,ventFixme,@Spell
syn match	ventComment	"\(^\|\s\)//.*"		contains=ventTodo,ventFixme,@Spell

" Standard IEEE P1076.6 preprocessor directives (metacomments).
syn match	ventPreProc	"/\*\s*rtl_synthesis\s\+\(on\|off\)\s*\*/"
syn match	ventPreProc	"\(^\|\s\)--\s*rtl_synthesis\s\+\(on\|off\)\s*"
syn match	ventPreProc	"/\*\s*rtl_syn\s\+\(on\|off\)\s*\*/"
syn match	ventPreProc	"\(^\|\s\)--\s*rtl_syn\s\+\(on\|off\)\s*"

" Industry-standard directives. These are not standard VHDL, but are commonly
" used in the industry.
syn match	ventPreProc	"/\*\s*synthesis\s\+translate_\(on\|off\)\s*\*/"
"syn match	ventPreProc	"/\*\s*simulation\s\+translate_\(on\|off\)\s*\*/"
syn match	ventPreProc	"/\*\s*pragma\s\+translate_\(on\|off\)\s*\*/"
syn match	ventPreProc	"/\*\s*pragma\s\+synthesis_\(on\|off\)\s*\*/"
syn match	ventPreProc	"/\*\s*synopsys\s\+translate_\(on\|off\)\s*\*/"

syn match	ventPreProc	"\(^\|\s\)--\s*synthesis\s\+translate_\(on\|off\)\s*"
"syn match	ventPreProc	"\(^\|\s\)--\s*simulation\s\+translate_\(on\|off\)\s*"
syn match	ventPreProc	"\(^\|\s\)--\s*pragma\s\+translate_\(on\|off\)\s*"
syn match	ventPreProc	"\(^\|\s\)--\s*pragma\s\+synthesis_\(on\|off\)\s*"
syn match	ventPreProc	"\(^\|\s\)--\s*synopsys\s\+translate_\(on\|off\)\s*"

"Modify the following as needed.  The trade-off is performance versus functionality.
syn sync	minlines=600

" Define the default highlighting.
" Only when an item doesn't have highlighting yet

hi def link ventSpecial	Special
hi def link ventStatement   Statement
hi def link ventCharacter   Character
hi def link ventString	String
hi def link ventVector	Number
hi def link ventBoolean	Number
hi def link ventTodo	Todo
hi def link ventFixme	Fixme
hi def link ventComment	Comment
hi def link ventNumber	Number
hi def link ventTime	Number
hi def link ventType	Type
hi def link ventOperator    Operator
hi def link ventError	Error
hi def link ventAttribute   Special
hi def link ventPreProc	PreProc


let b:current_syntax = "vent"

let &cpo = s:cpo_save
unlet s:cpo_save
" vim: ts=8
