set(STORELIBCOUT ${CMAKE_CURRENT_SOURCE_DIR}/../application/src/lib_open_source/)
set(STORELIBCOUT2 ${CMAKE_CURRENT_SOURCE_DIR}/../application/src_bin/third_party_groupware/StoreLib/)
set(INCLUDE_OUT ${CMAKE_CURRENT_SOURCE_DIR}/../application/src/include/open_source/raid/lsi)
set(BASE_DIR /home/public/workspace/application)
set(G_STORELIB MR_StoreLib_Unified_Ventura_7.6-07.0604.0100.1000-SCGCQ01944634)
set(G_GLOBAL global.sh)
set(G_BUILD build.sh)

MESSAGE( STATUS "cmake_current_source_dir: ${CMAKE_CURRENT_SOURCE_DIR}")
MESSAGE( STATUS ":ARM64_HI1711_ENABLED is ${ARM64_HI1711_ENABLED}")
if(ARM64_HI1711_ENABLED)
    set(G_GLOBAL global64.sh)
	set(G_BUILD build64.sh)
	set(STORELIBCOUT2   ${CMAKE_CURRENT_SOURCE_DIR}/../application/src_bin/third_party_groupware/arm64/StoreLib/)
endif()

include(ExternalProject)
ExternalProject_Add(storeli
        SOURCE_DIR ${BASE_DIR}/vendor/
        CONFIGURE_COMMAND cd ${BASE_DIR}/vendor/StoreLib && bash config.sh ${G_STORELIB} ${G_GLOBAL}
		BUILD_COMMAND cd ${BASE_DIR}/vendor/StoreLib &&  bash ${G_BUILD} $(MAKE) 
		INSTALL_COMMAND ""
)

add_custom_command(TARGET storeli
  POST_BUILD
  COMMAND cp -a  ${BASE_DIR}/vendor/StoreLib/output/*  ${STORELIBCOUT}
  COMMAND cp -a  ${BASE_DIR}/vendor/StoreLib/output/*  ${STORELIBCOUT2}
  COMMAND cp -a  ${BASE_DIR}/vendor/StoreLib/versionChangeSet/common_storelib_unf_rel/app_util/common_storelib_unf/src/oob/include/scsi-t10.h  ${INCLUDE_OUT}
  COMMAND LD_PRELOAD= sudo LD_PRELOAD=${LD_PRELOAD} -E cp -a  ${BASE_DIR}/vendor/StoreLib/versionChangeSet/common_storelib_unf_rel/fw/fw_sas_mega_api/eventmsg.h  ${INCLUDE_OUT}
)