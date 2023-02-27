afr_module(INTERNAL)    #NAME FreeRTOS-Cellular-Interface

set(src_dir "${CMAKE_CURRENT_LIST_DIR}/source")
set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/source/include")
set(interface_dir "${CMAKE_CURRENT_LIST_DIR}/source/interface")
set(test_dir "${CMAKE_CURRENT_LIST_DIR}/test")

afr_module_sources(
        ${AFR_CURRENT_MODULE}
        PRIVATE
        "${src_dir}/cellular_3gpp_api.c"
        "${src_dir}/cellular_3gpp_urc_handler.c"
        "${src_dir}/cellular_at_core.c"
        "${src_dir}/cellular_common.c"
        "${src_dir}/cellular_common_api.c"
        "${src_dir}/cellular_pkthandler.c"
        "${src_dir}/cellular_pktio.c"
        "${interface_dir}/cellular_comm_interface.h"
        "${inc_dir}/private/cellular_common_internal.h"
        "${inc_dir}/private/cellular_internal.h"
        "${inc_dir}/private/cellular_pkthandler_internal.h"
        "${inc_dir}/private/cellular_pktio_internal.h"
        "${inc_dir}/common/cellular_at_core.h"
        "${inc_dir}/common/cellular_common.h"
        "${inc_dir}/common/cellular_common_api.h"
        "${inc_dir}/common/cellular_common_portable.h"
        "${inc_dir}/cellular_api.h"
        "${inc_dir}/cellular_config_defaults.h"
        "${inc_dir}/cellular_types.h"
)

afr_module_include_dirs(
        ${AFR_CURRENT_MODULE}
        PUBLIC "${inc_dir}" "${interface_dir}"
        PRIVATE "${test_dir}"
)

afr_module_dependencies(
        ${AFR_CURRENT_MODULE}
        PUBLIC AFR::${AFR_CURRENT_MODULE}::mcu_port FreeRTOS-Cellular-Interface-Module #AFR::FreeRTOS-Cellular-Interface::mcu_port AFR::FreeRTOS-Cellular-Interface::module
)

#afr_test_module()
#
#afr_module_sources(
#    ${AFR_CURRENT_MODULE}
#    INTERFACE
#        "${test_dir}/iot_test_freertos_tcp.c"
#)
#
#afr_module_dependencies(
#    ${AFR_CURRENT_MODULE}
#    INTERFACE AFR::FreeRTOS-Cellular-Interface
#)
