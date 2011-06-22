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
# $Id: Makefile 671 2009-01-16 18:58:16Z ankit $
#===================================================#

include ./Makefile.inc

DBGFLAGS= -ggdb2
OPTFLAGS= -O2

CFLAGS= -pipe -export-dynamic -Wall -pedantic -Wno-variadic-macros -Wshadow -Woverloaded-virtual -fPIC -DPIC -I../include
LFLAGS= -pipe -export-dynamic -fPIC -DPIC -I/usr/include/boost -L/usr/lib
MCFLAGS= -pipe -pedantic -I../../include -export-dynamic -Wno-variadic-macros -Wshadow -Wall -Woverloaded-virtual -fPIC -DPIC
MLFLAGS= -pipe -shared -export-dynamic -fPIC -DPIC

COREARGS= 'CXX=${CXX}' 'EFLAGS=${OPTFLAGS}' 'LDL=${LDL}' 'CFLAGS=${CFLAGS}' 'LFLAGS=${LFLAGS}'
COREDEBUGARGS= 'CXX=${CXX}' 'EFLAGS=${DBGFLAGS}' 'LDL=${LDL}' 'CFLAGS=${CFLAGS}' 'LFLAGS=${LFLAGS}'
MODARGS= 'CXX=${CXX}' 'EFLAGS=${OPTFLAGS}' 'CFLAGS=${MCFLAGS}' 'LFLAGS=${MLFLAGS}'
MODDEBUGARGS= 'CXX=${CXX}' 'EFLAGS=${DBGFLAGS}' 'CFLAGS=${MCFLAGS}' 'LFLAGS=${MLFLAGS}'

all:
	@$(MAKE) makeall
	@echo " "
	@echo "****************************************************"
	@echo "*     ACORA IRC SERVICES SUCCESSFULLY COMPILED     *"
	@echo "*  Type '(g)make install' to install the services  *"
	@echo "****************************************************"

debug:
	@echo "********************************************"
	@echo "*       COMPILING ACORA IRC SERVICES       *"
	@echo "* Compiling with debug takes a lot of disk *"
	@echo "*  space. If you have space issues, press  *"
	@echo "*                CTRL+C now.               *"
	@echo "********************************************"
	@$(MAKE) makedebug
	@echo " "
	@echo "****************************************************"
	@echo "*     ACORA IRC SERVICES SUCCESSFULLY COMPILED     *"
	@echo "*  Type '(g)make install' to install the services  *"
	@echo "****************************************************"

modules:
	@echo "***********************"
	@echo "*  COMPILING MODULES  *"
	@echo "***********************"
	@$(MAKE) mods
	@echo " "
	@echo "******************************************************"
	@echo "*           MODULES SUCCESSFULLY COMPILED            *"
	@echo "*  Type '(g)make modinstall' to install the modules  *"
	@echo "******************************************************"

moddebug:
	@echo "********************************************"
	@echo "*            COMPILING MODULES             *"
	@echo "* Compiling with debug takes a lot of disk *"
	@echo "*  space. If you have space issues, press  *"
	@echo "*                CTRL+C now.               *"
	@echo "********************************************"
	@$(MAKE) modusdebug
	@echo " "
	@echo "******************************************************"
	@echo "*           MODULES SUCCESSFULLY COMPILED            *"
	@echo "*  Type '(g)make modinstall' to install the modules  *"
	@echo "******************************************************"

makeall: core socks ircds disks mods
makedebug: coredebug sockdebug ircddebug diskdebug modusdebug


core:
	@$(MAKE) -C src $(COREARGS) all
socks:
	@$(MAKE) -C src/socketengines $(MODARGS) all
ircds:
	@$(MAKE) -C src/ircdprotocols $(MODARGS) all
disks:
	@$(MAKE) -C src/diskmanagers $(MODARGS) all
mods:
	@$(MAKE) -C src/modules $(MODARGS) all


coredebug:
	@$(MAKE) -C src $(COREDEBUGARGS) all
sockdebug:
	@$(MAKE) -C src/socketengines $(MODDEBUGARGS) all
ircddebug:
	@$(MAKE) -C src/ircdprotocols $(MODDEBUGARGS) all
diskdebug:
	@$(MAKE) -C src/diskmanagers $(MODDEBUGARGS) all
modusdebug:
	@$(MAKE) -C src/modules $(MODDEBUGARGS) all


remake: clean all

clean:
	@echo "Cleaning ..."
	@$(MAKE) -C src clean
	@$(MAKE) -C src/modules clean
	@$(MAKE) -C src/diskmanagers clean
	@$(MAKE) -C src/ircdprotocols clean
	@$(MAKE) -C src/socketengines clean
	@echo "Done."

distclean:
	@echo "Cleaning everything ..."
	@($(MAKE) clean) > /dev/null
	@rm -f include/config.h Makefile.inc acoira bin/acora bin/*.so bin/diskmanagers/*.so bin/ircdprotocols/*.so bin/modules/*.so bin/socketengines/*.so *~ conf/*~ docs/*~ include/*~
	@echo "Done."

install:
	@echo "*************************************"
	@echo "*   INSTALLING ACORA IRC SERVICES   *"
	@echo "*************************************"
	@echo " "
	@echo "Installing core ..."
	@$(MAKE) -C src install
	@echo "Installing socketengines ..."
	@$(MAKE) -C src/socketengines install
	@echo "Installing ircdprotocols ..."
	@$(MAKE) -C src/ircdprotocols install
	@echo "Installing diskmanagers ..."
	@$(MAKE) -C src/diskmanagers install
	@echo "Installing modules ..."
	@$(MAKE) -C src/modules install
	@echo "Installed."
	@echo " "
	@echo "*************************************************"
	@echo "*   ACORA IRC SERVICES SUCCESSFULLY INSTALLED   *"
	@echo "*      Please edit your acora.conf file in      *"
	@echo "*               the conf directory.             *"
	@echo "*   Type ./acora start to start the services    *"
	@echo "*************************************************"

modinstall:
	@echo "Installing modules ..."
	@$(MAKE) -C src/modules install
	@echo "Installed."

modclean:
	@echo "Cleaning modules ..."
	@$(MAKE) -C src/modules clean
	@echo "Done."

