#ifndef CEL_JIOS_PROTOBUF_IJ_HPP
#define CEL_JIOS_PROTOBUF_IJ_HPP

#include <istream>
#include <google/protobuf/message.h>
#include <cel/jios/jin.hpp>

namespace cel {

void merge_proto_type(ijnode & ij, google::protobuf::Message & pro);

void parse_proto_type(ijnode & ij, google::protobuf::Message & pro);

} // namespace

namespace jios {

void jios_read(ijnode & ij, google::protobuf::Message & pro);

} // namespace

#endif

