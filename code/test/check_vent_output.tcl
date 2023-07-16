#this is a script to check the syntax of a VHDL file using Vivado's batch mode

variable vhdlFile [lindex $argv 0]

#puts -nonewline "checking syntax of " 
#puts $vhdlFile

add_files -quiet $vhdlFile
set errorString [check_syntax -return_string -quiet]

set slen [string length $errorString]
if {$slen != 0} {
	puts $errorString
}
