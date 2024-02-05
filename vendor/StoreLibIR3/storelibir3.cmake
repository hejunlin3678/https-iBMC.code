set(STORELIBIR3COUT ${CMAKE_CURRENT_SOURCE_DIR}/../application/src/lib_open_source/)
set(STORELIBIR3COUT2 ${CMAKE_CURRENT_SOURCE_DIR}/../application/src_bin/third_party_groupware/StoreLibIR3/)
set(INCLUDE_OUT ${CMAKE_CURRENT_SOURCE_DIR}/../application/src/include/open_source/raid/lsi)
set(BASE_DIR /home/public/workspace/application)
set(G_STORELIBIR3 SAS3_StorelibIR3_BMC_Unified_Phase16-16.02-SCGCQ01958891)
set(G_GLOBAL global.sh)
set(G_BUILD build.sh)

MESSAGE( STATUS "cmake_current_source_dir: ${CMAKE_CURRENT_SOURCE_DIR}")
MESSAGE( STATUS ":ARM64_HI1711_ENABLED is ${ARM64_HI1711_ENABLED}")
if(ARM64_HI1711_ENABLED)
    set(G_GLOBAL global64.sh)
	set(G_BUILD build64.sh)
	set(STORELIBIR3COUT2   ${CMAKE_CURRENT_SOURCE_DIR}/../application/src_bin/third_party_groupware/arm64/StoreLibIR3/)
endif()

include(ExternalProject)
ExternalProject_Add(storelibir3
        SOURCE_DIR ${BASE_DIR}/vendor/
        CONFIGURE_COMMAND cd ${BASE_DIR}/vendor/StoreLibIR3 && bash config.sh ${G_STORELIBIR3} ${G_GLOBAL}
		BUILD_COMMAND cd ${BASE_DIR}/vendor/StoreLibIR3 &&  bash ${G_BUILD} $(MAKE) 
		INSTALL_COMMAND ""
)

add_custom_command(TARGET storelibir3
  POST_BUILD
  COMMAND cp -a  ${BASE_DIR}/vendor/StoreLibIR3/output/*  ${STORELIBIR3COUT}
  COMMAND cp -a  ${BASE_DIR}/vendor/StoreLibIR3/output/*  ${STORELIBIR3COUT2}
  COMMAND LD_PRELOAD= sudo LD_PRELOAD=${LD_PRELOAD} -E cp -a  ${BASE_DIR}/vendor/StoreLibIR3/versionChangeSet/storelibir3_bmc_rel/storelib_src_rel/fw/fw_sas_mega_api/*  ${INCLUDE_OUT}
  COMMAND LD_PRELOAD= sudo LD_PRELOAD=${LD_PRELOAD} -E cp -a  ${BASE_DIR}/vendor/StoreLibIR3/versionChangeSet/storelibir3_bmc_rel/storelib_src_rel/app_util/common_storeLib_hdr/*  ${INCLUDE_OUT}
)