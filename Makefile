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
	rm -f cpplighty-*.tar.gz

release: clean
	mkdir -p cpplighty-${VERSION}
	cp -R LICENSE Makefile README.md config.mk ${SRC} \
		50-cpplighty.rules cpplighty-${VERSION}
	tar -cf cpplighty-${VERSION}.tar cpplighty-${VERSION}
	gzip cpplighty-${VERSION}.tar
	rm -rf cpplighty-${VERSION}

install: all
	mkdir -p $(DESTDIR)${PREFIX}/bin/
	cp ./cpplighty $(DESTDIR)${PREFIX}/bin/cpplighty
	chmod 755 $(DESTDIR)${PREFIX}/bin/cpplighty

uninstall:
	rm $(DESTDIR)${PREFIX}/bin/cpplighty

.PHONY: all options clean release install uninstall
