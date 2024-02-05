set(STORELIBITCOUT ${CMAKE_CURRENT_SOURCE_DIR}/../application/src/lib_open_source/)
set(STORELIBITCOUT2   ${CMAKE_CURRENT_SOURCE_DIR}/../application/src_bin/third_party_groupware/StoreLibIT/)
set(INCLUDE_OUT ${CMAKE_CURRENT_SOURCE_DIR}/../application/src/include/open_source/raid/lsi)
set(BASE_DIR /home/public/workspace/application)
set(G_STORELIBIT StoreLib_Unified_Ventura_IT_Phase7-07.0700.0200.0600-SCGCQ01958902)
set(G_GLOBAL global.sh)
set(G_BUILD build.sh)

MESSAGE( STATUS "cmake_current_source_dir: ${CMAKE_CURRENT_SOURCE_DIR}")
MESSAGE( STATUS ":ARM64_HI1711_ENABLED is ${ARM64_HI1711_ENABLED}")
if(ARM64_HI1711_ENABLED)
    set(G_GLOBAL global64.sh)
	set(G_BUILD build64.sh)
	set(STORELIBITCOUT2   ${CMAKE_CURRENT_SOURCE_DIR}/../application/src_bin/third_party_groupware/arm64/StoreLibIT/)
endif()

include(ExternalProject)
ExternalProject_Add(storelibit
        SOURCE_DIR ${BASE_DIR}/vendor/
        CONFIGURE_COMMAND cd ${BASE_DIR}/vendor/StoreLibIT && bash config.sh ${G_STORELIBIT} ${G_GLOBAL}
		BUILD_COMMAND cd ${BASE_DIR}/vendor/StoreLibIT &&  bash ${G_BUILD} $(MAKE) 
		INSTALL_COMMAND ""
)

add_custom_command(TARGET storelibit
  POST_BUILD
  COMMAND cp -a  ${BASE_DIR}/vendor/StoreLibIT/output/*  ${STORELIBITCOUT}
  COMMAND cp -a  ${BASE_DIR}/vendor/StoreLibIT/output/*  ${STORELIBITCOUT2}
  COMMAND cp -a  ${BASE_DIR}/vendor/StoreLibIT/versionChangeSet/storelib_unf_rel/app_util/storelib_unf/src/common/include/inc_mpi/mpi2_cnfg.h  ${INCLUDE_OUT}
  COMMAND cp -a  ${BASE_DIR}/vendor/StoreLibIT/versionChangeSet/storelib_unf_rel/app_util/storelib_unf/src/common/include/inc_mpi/mpi2.h  ${INCLUDE_OUT}
)