LIB	= port.o
OBJS	= os_cpu_c.o
SOBJS	= os_cpu_a.o

all:	$(LIB)

$(LIB):	$(OBJS) $(SOBJS)
	$(LD) -EL -r -o $@ $(OBJS) $(SOBJS)

#########################################################################

.depend:	Makefile $(OBJS:.o=.c) $(SOBJS:.o=.S)
		$(CC) -EL -M $(CFLAGS) $(OBJS:.o=.c) $(SOBJS:.o=.S) > $@

sinclude .depend

#########################################################################
