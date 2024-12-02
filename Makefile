include config.mk

SRC = cpplighty.cpp

all: options cpplighty

options:
	@echo cpplighty build options:
	@echo "CXXFLAGS   = ${CXXFLAGS}"
	@echo "CXX       = ${CXX}"

cpplighty: 
	${CXX} -o $@ ${SRC} ${CXXFLAGS}

clean:
	rm -f cpplighty

release: clean
	

install: all


uninstall:


.PHONY: all options clean release install uninstall
