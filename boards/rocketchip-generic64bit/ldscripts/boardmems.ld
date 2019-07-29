MEMORY
{
	memory	(rwx):	ORIGIN = 0x80000000,	LENGTH = 0x10000000
}

PHDRS
{
  ram_init PT_LOAD;
  ram_noload PT_NULL;
}

PROVIDE(__stack_size = 0x800000);