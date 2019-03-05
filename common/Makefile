
# CFLAGS += -DET_DEBUG -DDEBUG

LIB	= common.o

OBJS	= openmips.o

all:	$(LIB)

$(LIB):	$(OBJS) $(SOBJS)
	$(LD) -EL -r -o $@ $(OBJS) $(SOBJS)

#########################################################################

.depend:	Makefile $(OBJS:.o=.c)
		$(CC) -EL -M $(CFLAGS) $(OBJS:.o=.c) > $@

sinclude .depend

#########################################################################
