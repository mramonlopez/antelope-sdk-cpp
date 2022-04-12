cmake_minimum_required(VERSION 3.18)
set(CMAKE_CXX_STANDARD 17)

set(LIB_NAME secp256k1)
project(${LIB_NAME} VERSION 0.1 LANGUAGES CXX C)

option(ECMULT_GEN_PREC_BITS "ecmult gen prec. bits" 4)

set(SOURCE_FILES 
	# src/bench.c
	# src/bench_ecmult.c
	# src/bench_internal.c
	src/precompute_ecmult.c
	src/precompute_ecmult_gen.c
	src/precomputed_ecmult.c
	src/precomputed_ecmult_gen.c
	src/secp256k1.c
	# src/tests.c
	# src/tests_exhaustive.c
	# src/valgrind_ctime_test.c
)

set(HEADER_FILES
	include/secp256k1.h
	include/secp256k1_ecdh.h
	include/secp256k1_extrakeys.h
	include/secp256k1_preallocated.h
	include/secp256k1_recovery.h
	include/secp256k1_schnorrsig.h
	src/assumptions.h
	src/basic-config.h
	src/bench.h
	src/ecdsa.h
	src/ecdsa_impl.h
	src/eckey.h
	src/eckey_impl.h
	src/ecmult.h
	src/ecmult_compute_table.h
	src/ecmult_compute_table_impl.h
	src/ecmult_const.h
	src/ecmult_const_impl.h
	src/ecmult_gen.h
	src/ecmult_gen_compute_table.h
	src/ecmult_gen_compute_table_impl.h
	src/ecmult_gen_impl.h
	src/ecmult_impl.h
	src/field.h
	src/field_10x26.h
	src/field_10x26_impl.h
	src/field_5x52.h
	src/field_5x52_asm_impl.h
	src/field_5x52_impl.h
	src/field_5x52_int128_impl.h
	src/field_impl.h
	src/group.h
	src/group_impl.h
	src/hash.h
	src/hash_impl.h
	src/modinv32.h
	src/modinv32_impl.h
	src/modinv64.h
	src/modinv64_impl.h
	src/modules/ecdh/bench_impl.h
	src/modules/ecdh/main_impl.h
	src/modules/ecdh/tests_impl.h
	src/modules/extrakeys/main_impl.h
	src/modules/extrakeys/tests_exhaustive_impl.h
	src/modules/extrakeys/tests_impl.h
	src/modules/recovery/bench_impl.h
	src/modules/recovery/main_impl.h
	src/modules/recovery/tests_exhaustive_impl.h
	src/modules/recovery/tests_impl.h
	src/modules/schnorrsig/bench_impl.h
	src/modules/schnorrsig/main_impl.h
	src/modules/schnorrsig/tests_exhaustive_impl.h
	src/modules/schnorrsig/tests_impl.h
	src/precomputed_ecmult.h
	src/precomputed_ecmult_gen.h
	src/scalar.h
	src/scalar_4x64.h
	src/scalar_4x64_impl.h
	src/scalar_8x32.h
	src/scalar_8x32_impl.h
	src/scalar_impl.h
	src/scalar_low.h
	src/scalar_low_impl.h
	src/scratch.h
	src/scratch_impl.h
	src/selftest.h
	src/testrand.h
	src/testrand_impl.h
	src/util.h
)

source_group(src FILES ${SOURCE_FILES})
source_group(include FILES ${HEADER_FILES})

add_library(${LIB_NAME} ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(${LIB_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_compile_definitions(${LIB_NAME} PUBLIC
    ECMULT_GEN_PREC_BITS=4
	ECMULT_WINDOW_SIZE=15
    ENABLE_MODULE_RECOVERY
    # ENABLE_MODULE_ECDH
    # ENABLE_MODULE_EXTRAKEYS
    # ENABLE_MODULE_SCHNORRSIG
)
