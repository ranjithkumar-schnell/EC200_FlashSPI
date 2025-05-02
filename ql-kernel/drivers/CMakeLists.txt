# Copyright (C) 2020 QUECTEL Technologies Limited and/or its affiliates("QUECTEL").
# All rights reserved.
#

set(target oql_core_drv)

add_app_libraries($<TARGET_FILE:${target}>)
add_library(${target} STATIC)
set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${out_lib_dir})
target_compile_definitions(${target} PRIVATE OSI_LOG_TAG=LOG_TAG_QUEC)
target_include_directories(${target} PUBLIC inc)
target_link_libraries(${target} PRIVATE kernel driver service)

target_sources(${target} PRIVATE
	quec_pin_init.c
    quec_pin_cfg_${CONFIG_QL_PROJECT_DEF}.c
    quec_customer_cfg.c
	quec_internal_cfg.c		
)

target_sources_if(CONFIG_QUEC_PROJECT_FEATURE_EMBED_NOR_FLASH THEN ${target} PRIVATE 
	ql_embed_nor_flash.c
)

target_sources_if(CONFIG_QUEC_PROJECT_FEATURE_SPI_NOR_FLASH THEN ${target} PRIVATE 
	quec_spi_nor_flash_prop.c
)
relative_glob(srcs include/*.h src/*.c inc/*.h)
beautify_c_code(${target} ${srcs})
