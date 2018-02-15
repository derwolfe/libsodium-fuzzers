LIB_FUZZING_ENGINE ?= standalone_runner.o

# Values for CC, CFLAGS, CXX, CXXFLAGS are provided by OSS-Fuzz.
# Outside of OSS-Fuzz use the ones you prefer or rely on the default values.
# You may add extra compiler flags like this:
CXXFLAGS += -std=c++11

SODIUM_INCLUDE = ./libsodium/src/libsodium/include
SODIUM_A = ./libsodium/src/libsodium/.libs/libsodium.a

clean:
	rm -fv *.a *.o *_fuzzer

check: secret_key_auth_fuzzer
	./secret_key_auth_fuzzer secret_key_auth_corpus/*

# Fuzz target, links against $LIB_FUZZING_ENGINE, so that
# you may choose which fuzzing engine to use. For travis, this means that we use
# the standalone fuzzer, for google, we let it pick.
libsodium.a:
	cd libsodium && ./configure --enable-static && $(MAKE) clean && $(MAKE) -j$(nproc) all

standalone_runner.o: standalone_runner.cc

secret_key_auth_fuzzer: secret_key_auth_fuzzer.cc libsodium.a standalone_runner.o
	${CXX} ${CXXFLAGS} $< -I${SODIUM_INCLUDE} ${SODIUM_A} ${LIB_FUZZING_ENGINE} -o $@
