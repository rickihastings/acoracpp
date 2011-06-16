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
# $Id: Makefile 664 2009-01-16 11:25:35Z ankit $
#===================================================#

SRCS= $(shell /bin/ls | grep '\.cpp' | grep -v svn)
OBJECTS= $(SRCS:.cpp=.o)

all: $(OBJECTS)

remake: clean all

.cpp.o:
	$(CXX) $(EFLAGS) $(CFLAGS) -c $<
	$(CXX) $(EFLAGS) $(LFLAGS) -o $*.so $*.o

clean:
	rm -f *.o *.so *~
install:
	install *.so ../../bin/ircdprotocols/

