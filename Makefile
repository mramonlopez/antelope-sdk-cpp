
#CXX = g++-8
libs = -lcurl -L. -L ./abieos/build/ #-lprofiler
includes = -I . -I secp256k1/includes -I abieos/external/rapidjson/include -I abieos/include/
#######objects for main prog:
app_objects = app.cpp
app_objects += eos_client.o
app_objects += ./abieos/build/libabieos.so
app_static_lib = ./abieos/build/libabieos.a ./cryptopp/libcryptopp.a secp256k1/.libs/libsecp256k1.a
#some flags
flag_global = -O2 -std=c++17  #eosio is on standart c++17  -pg 
flag_main = -Wall -Werror -Wno-unknown-pragmas -Wno-maybe-uninitialized  -Wno-unknown-warning-option -dynamiclib#-pthread

all: app #test

app: $(app_objects)
	$(CXX) $(flag_global) $(flag_main) -DNDEBUG $(app_objects) $(libs) $(includes) -o app $(app_static_lib) 

eos_client.o: eos_client.cpp eos_client.h
	$(CXX) $(flag_global) $(libs) $(includes) -c eos_client.cpp -o eos_client.o

cryptopp/cryptlib.o: cryptopp/cryptlib.h cryptopp/cryptlib.cpp
	cd cryptopp && make && cd -

libsecp256k1.a: secp256k1/.libs/libsecp256k1.a
	cp secp256k1/.libs/libsecp256k1.a .	

clean: 
	rm -fr app
	rm -fr *.o *.out.* *.out *.stats *.a