cmd_Release/obj.target/ibft/src/message.o := g++ -o Release/obj.target/ibft/src/message.o ../src/message.cpp '-DNODE_GYP_MODULE_NAME=ibft' '-DUSING_UV_SHARED=1' '-DUSING_V8_SHARED=1' '-DV8_DEPRECATION_WARNINGS=1' '-DV8_DEPRECATION_WARNINGS' '-DV8_IMMINENT_DEPRECATION_WARNINGS' '-D_GLIBCXX_USE_CXX11_ABI=1' '-D_LARGEFILE_SOURCE' '-D_FILE_OFFSET_BITS=64' '-D__STDC_FORMAT_MACROS' '-DOPENSSL_NO_PINSHARED' '-DOPENSSL_THREADS' '-DBUILDING_NODE_EXTENSION' -I/home/bathan1/.cache/node-gyp/20.10.0/include/node -I/home/bathan1/.cache/node-gyp/20.10.0/src -I/home/bathan1/.cache/node-gyp/20.10.0/deps/openssl/config -I/home/bathan1/.cache/node-gyp/20.10.0/deps/openssl/openssl/include -I/home/bathan1/.cache/node-gyp/20.10.0/deps/uv/include -I/home/bathan1/.cache/node-gyp/20.10.0/deps/zlib -I/home/bathan1/.cache/node-gyp/20.10.0/deps/v8/include -I/home/bathan1/dev/ibft_catpics/binding/node_modules/.pnpm/node-addon-api@8.0.0/node_modules/node-addon-api  -fPIC -pthread -Wall -Wextra -Wno-unused-parameter -m64 -O3 -fno-omit-frame-pointer -fno-rtti -std=gnu++17 -MMD -MF ./Release/.deps/Release/obj.target/ibft/src/message.o.d.raw   -c
Release/obj.target/ibft/src/message.o: ../src/message.cpp \
 ../src/message.h ../src/blockchain.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/types.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/e_os2.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/macros.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/opensslconf.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/configuration.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/./configuration_asm.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/././archs/linux-x86_64/asm/include/openssl/configuration.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/opensslv.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/./opensslv_asm.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/././archs/linux-x86_64/asm/include/openssl/opensslv.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/safestack.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/./safestack_asm.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/././archs/linux-x86_64/asm/include/openssl/safestack.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/stack.h \
 ../src/transaction.h ../src/crypto_utils.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/evp.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/core.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/core_dispatch.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/symhacks.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/bio.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/./bio_asm.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/././archs/linux-x86_64/asm/include/openssl/bio.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/crypto.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/./crypto_asm.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/././archs/linux-x86_64/asm/include/openssl/crypto.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/cryptoerr.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/cryptoerr_legacy.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/bioerr.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/evperr.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/params.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/bn.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/bnerr.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/objects.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/obj_mac.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/asn1.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/./asn1_asm.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/././archs/linux-x86_64/asm/include/openssl/asn1.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/asn1err.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/objectserr.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/err.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/./err_asm.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/././archs/linux-x86_64/asm/include/openssl/err.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/lhash.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/./lhash_asm.h \
 /home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/././archs/linux-x86_64/asm/include/openssl/lhash.h
../src/message.cpp:
../src/message.h:
../src/blockchain.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/types.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/e_os2.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/macros.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/opensslconf.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/configuration.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/./configuration_asm.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/././archs/linux-x86_64/asm/include/openssl/configuration.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/opensslv.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/./opensslv_asm.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/././archs/linux-x86_64/asm/include/openssl/opensslv.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/safestack.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/./safestack_asm.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/././archs/linux-x86_64/asm/include/openssl/safestack.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/stack.h:
../src/transaction.h:
../src/crypto_utils.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/evp.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/core.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/core_dispatch.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/symhacks.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/bio.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/./bio_asm.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/././archs/linux-x86_64/asm/include/openssl/bio.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/crypto.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/./crypto_asm.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/././archs/linux-x86_64/asm/include/openssl/crypto.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/cryptoerr.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/cryptoerr_legacy.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/bioerr.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/evperr.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/params.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/bn.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/bnerr.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/objects.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/obj_mac.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/asn1.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/./asn1_asm.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/././archs/linux-x86_64/asm/include/openssl/asn1.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/asn1err.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/objectserr.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/err.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/./err_asm.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/././archs/linux-x86_64/asm/include/openssl/err.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/lhash.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/./lhash_asm.h:
/home/bathan1/.cache/node-gyp/20.10.0/include/node/openssl/././archs/linux-x86_64/asm/include/openssl/lhash.h:
