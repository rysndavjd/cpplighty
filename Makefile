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
	mkdir -p cpplighty-${VERSION}
	cp -R LICENSE Makefile README.md config.mk ${SRC} \
		cpplighty-${VERSION}
	tar -cf cpplighty-${VERSION}.tar cpplighty-${VERSION}
	gzip cpplighty-${VERSION}.tar
	rm -rf cpplighty-${VERSION}

install: all
	mkdir -p /usr/bin/
	cp ./cpplighty /usr/bin/cpplighty
	chmod 755 /usr/bin/cpplighty

uninstall:
	rm /usr/bin/cpplighty

.PHONY: all options clean release install uninstall
