include_directories(${CMAKEDEMO_SOURCE_DIR}/src/backend)
link_directories(${CMAKEDEMO_BINARY_DIR}/src/backend)

set(LIBS ${LIBS} cronovanie processing read_conf variables wait_function)

if (CDP_P)
add_library(cdp cdp.c)
set(LIBS ${LIBS} cdp)
endif(CDP_P)

if (_SIP)
add_library(sip sip.c)
set(LIBS ${LIBS} sip)
endif(_SIP)

if (NETFLOW)
add_library(nflow_sflow nflow_sflow.c)
set(LIBS ${LIBS} nflow_sflow)
endif(NETFLOW)

add_executable(katalyzer katalyzer.c)
target_link_libraries(katalyzer ${LIBS} ${EXT_LIBS})

add_library(cronovanie cronovanie.c)
add_library(processing processing.c)
add_library(read_conf read_conf.c)
add_library(variables variables.c)
add_library(wait_function wait_function.c)
# add_library(snmpp snmpp.c)
