#===================================================#
#              ~ Acora IRC Services ~               #
#                                                   #
#       Acora (C) 2011 Acora Development Team       #
# Please see the file CREDITS for more information. #
#                                                   #
#   This program is free but copyrighted software   #
#      Please see the file COPYING for details.     #
#                                                   #
#===================================================#

OBJS= $(shell /bin/ls | grep '\.cpp' | grep -v svn | sed 's/\.cpp/\.o/')

all: acora

.cpp.o:
	$(CXX) $(EFLAGS) $(CFLAGS) -c $<

acora: $(OBJS)
	$(CXX) $(EFLAGS) $(LFLAGS) $(LDL) $(OBJS) -o acora /usr/lib/libboost_thread.so.1.42.0

remake: clean all

clean:
	rm -f *.o *~ acora
install:
	install acora ../bin/
	install ../bin/acora ../acora	# temp

