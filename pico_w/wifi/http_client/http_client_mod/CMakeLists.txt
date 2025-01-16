# Modified version of LwIP http client
pico_add_library(pico_lwip_http_client_mod NOFLAG)
target_include_directories(pico_lwip_http_client_mod INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}
        )
target_sources(pico_lwip_http_client_mod INTERFACE
    ${PICO_LWIP_PATH}/src/apps/http/altcp_proxyconnect.c
    ${CMAKE_CURRENT_LIST_DIR}/http_client_mod.c
    )
