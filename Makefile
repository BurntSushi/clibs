all: libds.so libopt.so

libds.so: libds/libds.so
	cp libds/libds.so libds.so

libds/libds.so:
	(cd libds && make libds.so)

libopt.so: libopt/libopt.so
	cp libopt/libopt.so libopt.so

libopt/libopt.so:
	(cd libopt && make libopt.so)

clean:
	(cd libds && make clean)
	(cd libopt && make clean)
	rm -rf libds.so libopt.so

