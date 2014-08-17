all: #libsendev.so

clean:
	rm -rf lib bin 

test: modalkb
	./modalkb

#install: modalkb
#	- pkill modalkb
#	cp ./modalkb ~/bin
#	modalkbd

libsendev.so: sendev.cpp util.h Makefile
	g++ -fPIC -shared -o libsendev.so sendev.cpp