#include <cel/jios/protobuf_oj.hpp>

#include <boost/filesystem/fstream.hpp>
#include <cel/utility.hpp>
#include <cel/jios/json_oj.hpp>

using namespace std;
using boost::filesystem::path;
using namespace google;
using google::protobuf::FieldDescriptor;
using google::protobuf::Reflection;
using google::protobuf::Descriptor;
using namespace cel;

namespace jios {


template<typename T>
void print_field(ojnode & oj,
                 protobuf::Message const& pro,
                 FieldDescriptor const* field,
                 Reflection const* reflec,
                 T (Reflection::*get_meth)(
                      protobuf::Message const&,
                      FieldDescriptor const*) const
                )
{
  T value = (reflec->*get_meth)(pro, field);
  oj.print(value);
}

template<typename T>
void print_field(ojnode & oj,
                 protobuf::Message const& pro,
                 FieldDescriptor const* field,
                 int index,
                 Reflection const* reflec,
                 T (Reflection::*get_meth)(
                      protobuf::Message const&,
                      FieldDescriptor const*,
                      int) const
                )
{
  T value = (reflec->*get_meth)(pro, field, index);
  oj.print(value);
}

void print_singular_field(ojnode & oj,
                          protobuf::Message const& pro,
                          FieldDescriptor const* field,
                          Reflection const* reflec)
{
  switch (field->cpp_type()) {
    case FieldDescriptor::CppType::CPPTYPE_STRING:
      print_field(oj, pro, field, reflec, &Reflection::GetString);
      break;
    case FieldDescriptor::CppType::CPPTYPE_INT32:
      print_field(oj, pro, field, reflec, &Reflection::GetInt32);
      break;
    case FieldDescriptor::CppType::CPPTYPE_INT64:
      print_field(oj, pro, field, reflec, &Reflection::GetInt64);
      break;
    case FieldDescriptor::CppType::CPPTYPE_UINT32:
      print_field(oj, pro, field, reflec, &Reflection::GetUInt32);
      break;
    case FieldDescriptor::CppType::CPPTYPE_UINT64:
      print_field(oj, pro, field, reflec, &Reflection::GetUInt64);
      break;
    case FieldDescriptor::CppType::CPPTYPE_DOUBLE:
      print_field(oj, pro, field, reflec, &Reflection::GetDouble);
      break;
    case FieldDescriptor::CppType::CPPTYPE_FLOAT:
      print_field(oj, pro, field, reflec, &Reflection::GetFloat);
      break;
    case FieldDescriptor::CppType::CPPTYPE_BOOL:
      print_field(oj, pro, field, reflec, &Reflection::GetBool);
      break;
    case FieldDescriptor::CppType::CPPTYPE_MESSAGE:
      print_proto_type(oj, reflec->GetMessage(pro, field));
      break;
    case FieldDescriptor::CppType::CPPTYPE_ENUM:
    default:
      oj.print("UNIMPLEMENTED");
  }
}

void print_repeated_field(ojnode & oj,
                          protobuf::Message const& pro,
                          FieldDescriptor const* field,
                          int idx,
                          Reflection const* reflec)
{
  switch (field->cpp_type()) {
    case FieldDescriptor::CppType::CPPTYPE_STRING:
      print_field(oj, pro, field, idx, reflec, &Reflection::GetRepeatedString);
      break;
    case FieldDescriptor::CppType::CPPTYPE_INT32:
      print_field(oj, pro, field, idx, reflec, &Reflection::GetRepeatedInt32);
      break;
    case FieldDescriptor::CppType::CPPTYPE_INT64:
      print_field(oj, pro, field, idx, reflec, &Reflection::GetRepeatedInt64);
      break;
    case FieldDescriptor::CppType::CPPTYPE_UINT32:
      print_field(oj, pro, field, idx, reflec, &Reflection::GetRepeatedUInt32);
      break;
    case FieldDescriptor::CppType::CPPTYPE_UINT64:
      print_field(oj, pro, field, idx, reflec, &Reflection::GetRepeatedUInt64);
      break;
    case FieldDescriptor::CppType::CPPTYPE_DOUBLE:
      print_field(oj, pro, field, idx, reflec, &Reflection::GetRepeatedDouble);
      break;
    case FieldDescriptor::CppType::CPPTYPE_FLOAT:
      print_field(oj, pro, field, idx, reflec, &Reflection::GetRepeatedFloat);
      break;
    case FieldDescriptor::CppType::CPPTYPE_BOOL:
      print_field(oj, pro, field, idx, reflec, &Reflection::GetRepeatedBool);
      break;
    case FieldDescriptor::CppType::CPPTYPE_MESSAGE:
      print_proto_type(oj, reflec->GetRepeatedMessage(pro, field, idx));
      break;
    case FieldDescriptor::CppType::CPPTYPE_ENUM:
    default:
      oj.print("UNIMPLEMENTED");
  }
}

int print_count(protobuf::Message const& pro,
                Descriptor const* pd,
                Reflection const* reflec)
{
  int ret = 0;
  if (CONFIRM(pd && reflec)) {
    int N = pd->field_count();
    for (int i = 0; i < N; ++i) {
      FieldDescriptor const* field = pd->field(i);
      if (CONFIRM(field)) {
        if (field->is_repeated()) {
          if (reflec->FieldSize(pro, field) > 0) { ++ret; }
        } else {
          if (reflec->HasField(pro, field)) { ++ret; }
        }
      }
    }
  }
  return ret;
}

void print_proto_type(ojnode & oj, protobuf::Message const& pro)
{
  Descriptor const* pd = pro.GetDescriptor();
  Reflection const* reflec = pro.GetReflection();
  if (!CONFIRM(pd && reflec)) return;
  ojobject ojo = oj.begin_object(print_count(pro, pd, reflec) > 1);
  int N = pd->field_count();
  for (int i = 0; i < N; ++i) {
    FieldDescriptor const* field = pd->field(i);
    if (!CONFIRM(field)) return;
    if (field->is_repeated()) {
      int M = reflec->FieldSize(pro, field);
      if (M > 0) {
        ojarray oja = ojo[field->name()].begin_array(M > 1);
        for (int j = 0; j < M; ++j) {
          print_repeated_field(*oja, pro, field, j, reflec);
        }
        oja.terminate();
      }
    } else {
      if (reflec->HasField(pro, field)) {
        print_singular_field(ojo[field->name()], pro, field, reflec);
      }
    }
  }
  ojo.terminate();
};

void print_proto_type(ostream & os, protobuf::Message const& pro)
{
  print_proto_type(json_out(os).put(), pro);
  os << endl;
}

void print_proto_type(path const& file_path, protobuf::Message const& pro)
{
  boost::filesystem::ofstream fs(file_path);
  print_proto_type(fs, pro);
}


} // namespace

