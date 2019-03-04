MEMORY
        {
        vectors : ORIGIN = 0x80000000, LENGTH = 0x0000001000 
        ram     : ORIGIN = 0x80001000, LENGTH = 0x800fffff - 0x80001000
        }
 
SECTIONS
{
        .vectors :
        {
		*(.vectors)
        } > vectors

	.text : {
		*(.text)
		_endtext = .;
	}  > ram

	.rodata : {
		*(.rodata);
		*(.rodata.*)
	} > ram

	.sbss :
	{
		*(.sbss)
	} > ram

	.scommon :
	{
		*(.scommon)
	} > ram

	.data : {
		sdata  =  .;
		_sdata  =  .;
		*(.data)
                *(.sdata)
		edata  =  .;
		_edata  =  .;
	} > ram

	.bss SIZEOF(.data) + ADDR(.data) :
	{
		sbss = . ;
		_sbss = . ;
		__bss_start = ALIGN(0x8);
		___bss_start = ALIGN(0x8);
		*(.bss)
		*(COMMON)
		end = ALIGN(0x8);
		_end = ALIGN(0x8);
		__end = ALIGN(0x8);
		ebss = .;
		_ebss = .;
	} > ram

        .stack :
        {
        *(.stack)
	_stack_addr = .;        
        } > ram

}
