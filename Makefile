all: lib include lib/libds.so lib/libopt.so include/ds.h include/opt.h

lib:
	mkdir -p lib

include:
	mkdir -p include

lib/libds.so: libds/libds.so
	cp libds/libds.so lib/libds.so

libds/libds.so:
	(cd libds && make libds.so)

lib/libopt.so: libopt/libopt.so
	cp libopt/libopt.so lib/libopt.so

libopt/libopt.so:
	(cd libopt && make libopt.so)

include/ds.h: libds/ds.h
	cp libds/ds.h include/

libds/ds.h:
	(cd libds && make ds.h)

include/opt.h: libopt/opt.h
	cp libopt/opt.h include/

clean:
	(cd libds && make clean)
	(cd libopt && make clean)
	rm -rf lib/libds.so lib/libopt.so
	rm -rf include/ds.h include/opt.h

push:
	git push origin master
	git push github master

