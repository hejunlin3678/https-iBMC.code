
# compile flags
set(CC_OPT_LEVEL "-O0")
set(CC_OVERALL_FLAGS "-pipe")
set(CC_WARN_FLAGS "-Wall -Werror -Wfloat-equal -Wshadow -Wtrampolines -Wformat=2 -Wdate-time -Wstrict-prototypes")
set(CC_LANGUAGE_FLAGS "-fsigned-char")
set(CC_CDG_FLAGS "-fno-common -fno-exceptions -freg-struct-return")
set(CC_MD_DEPENDENT_FLAGS "-march=armv7-a -mlittle-endian")
set(CC_OPT_FLAGS "-fno-omit-frame-pointer -fno-strict-aliasing")
set(CC_SEC_FLAGS "-fPIC -fstack-protector-strong")
set(CC_DEFINE_FLAGS "")
set(CC_INCLUDE_FLAGS "")
set(CC_DEBUG_FLAGS "")

# Self-developed options, including warning options
set(CC_ALL_OPTIONS "\
    ${CC_OPT_LEVEL} ${CC_OVERALL_FLAGS} ${CC_WARN_FLAGS}\
    ${CC_LANGUAGE_FLAGS} ${CC_CDG_FLAGS} ${CC_MD_DEPENDENT_FLAGS}\
    ${CC_OPT_FLAGS} ${CC_SEC_FLAGS} ${CC_DEFINE_FLAGS} ${CC_INCLUDE_FLAGS} ${CC_DEBUG_FLAGS}")
# Open-source option, excluding warning options
set(CC_OPENSOURCE_OPTIONS "\
    ${CC_OPT_LEVEL} ${CC_OVERALL_FLAGS}\
    ${CC_LANGUAGE_FLAGS} ${CC_CDG_FLAGS} ${CC_MD_DEPENDENT_FLAGS}\
    ${CC_OPT_FLAGS} ${CC_SEC_FLAGS} ${CC_DEFINE_FLAGS} ${CC_INCLUDE_FLAGS} ${CC_DEBUG_FLAGS}")

# public link flags
set(LINK_LIBS "")
set(PUBLIC_LNK_FLAGS "-Wl,-z,relro,-z,noexecstack,-z,now -rdynamic ${LINK_LIBS}")

# link flag for module
set(SHARED_LNK_FLAGS "-shared ${PUBLIC_LNK_FLAGS}")
set(PIE_EXE_LNK_FLAGS "-pie ${PUBLIC_LNK_FLAGS}")

separate_arguments(CC_ALL_OPTIONS)
separate_arguments(CC_OPENSOURCE_OPTIONS)