cmake_minimum_required(VERSION 2.8.1)

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ib.proto)

add_library(ocel-proto-lib ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(ocel-proto-lib pthread ${PROTOBUF_LIBRARIES})

