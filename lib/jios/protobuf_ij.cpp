#include <cel/jios/protobuf_ij.hpp>

#include <cel/utility.hpp>

using namespace google;
using google::protobuf::FieldDescriptor;
using google::protobuf::Reflection;
using google::protobuf::Descriptor;

namespace cel {


template<typename T>
void parse_field(ijnode & ij,
                 protobuf::Message * pro,
                 FieldDescriptor const* field,
                 Reflection const* reflec,
                 void (Reflection::*set_meth)(
                       protobuf::Message *,
                       FieldDescriptor const*,
                       T) const
                )
{
  typedef typename
          std::remove_const<typename std::remove_reference<T>::type>::type
      value_type;
  value_type value = value_type();
  if (ij.read(value)) {
    (reflec->*set_meth)(pro, field, value);
  }
}

void parse_singular_field(ijnode & ij,
                          protobuf::Message * pro,
                          FieldDescriptor const* field,
                          Reflection const* reflec)
{
  switch (field->cpp_type()) {
    case FieldDescriptor::CppType::CPPTYPE_STRING:
      parse_field(ij, pro, field, reflec, &Reflection::SetString);
      break;
    case FieldDescriptor::CppType::CPPTYPE_INT32:
      parse_field(ij, pro, field, reflec, &Reflection::SetInt32);
      break;
    case FieldDescriptor::CppType::CPPTYPE_INT64:
      parse_field(ij, pro, field, reflec, &Reflection::SetInt64);
      break;
    case FieldDescriptor::CppType::CPPTYPE_UINT32:
      parse_field(ij, pro, field, reflec, &Reflection::SetUInt32);
      break;
    case FieldDescriptor::CppType::CPPTYPE_UINT64:
      parse_field(ij, pro, field, reflec, &Reflection::SetUInt64);
      break;
    case FieldDescriptor::CppType::CPPTYPE_DOUBLE:
      parse_field(ij, pro, field, reflec, &Reflection::SetDouble);
      break;
    case FieldDescriptor::CppType::CPPTYPE_FLOAT:
      parse_field(ij, pro, field, reflec, &Reflection::SetFloat);
      break;
    case FieldDescriptor::CppType::CPPTYPE_BOOL:
      parse_field(ij, pro, field, reflec, &Reflection::SetBool);
      break;
    case FieldDescriptor::CppType::CPPTYPE_MESSAGE:
      {
        protobuf::Message * p_sub = reflec->MutableMessage(pro, field);
        if (CONFIRM(p_sub)) { parse_proto_type(ij, *p_sub); }
        else { ij.set_failbit(); }
      }
      break;
    case FieldDescriptor::CppType::CPPTYPE_ENUM:
    default:
      ij.set_failbit();
      return;
  }
};

void parse_repeated_field(ijnode & ij,
                          protobuf::Message * pro,
                          FieldDescriptor const* field,
                          Reflection const* reflec)
{
  switch (field->cpp_type()) {
    case FieldDescriptor::CppType::CPPTYPE_STRING:
      parse_field(ij, pro, field, reflec, &Reflection::AddString);
      break;
    case FieldDescriptor::CppType::CPPTYPE_INT32:
      parse_field(ij, pro, field, reflec, &Reflection::AddInt32);
      break;
    case FieldDescriptor::CppType::CPPTYPE_INT64:
      parse_field(ij, pro, field, reflec, &Reflection::AddInt64);
      break;
    case FieldDescriptor::CppType::CPPTYPE_UINT32:
      parse_field(ij, pro, field, reflec, &Reflection::AddUInt32);
      break;
    case FieldDescriptor::CppType::CPPTYPE_UINT64:
      parse_field(ij, pro, field, reflec, &Reflection::AddUInt64);
      break;
    case FieldDescriptor::CppType::CPPTYPE_DOUBLE:
      parse_field(ij, pro, field, reflec, &Reflection::AddDouble);
      break;
    case FieldDescriptor::CppType::CPPTYPE_FLOAT:
      parse_field(ij, pro, field, reflec, &Reflection::AddFloat);
      break;
    case FieldDescriptor::CppType::CPPTYPE_BOOL:
      parse_field(ij, pro, field, reflec, &Reflection::AddBool);
      break;
    case FieldDescriptor::CppType::CPPTYPE_MESSAGE:
      {
        protobuf::Message * p_sub = reflec->AddMessage(pro, field);
        if (CONFIRM(p_sub)) { parse_proto_type(ij, *p_sub); }
        else { ij.set_failbit(); }
      }
      break;
    case FieldDescriptor::CppType::CPPTYPE_ENUM:
    default:
      ij.set_failbit();
      return;
  }
};

void merge_proto_type(ijnode & ij, protobuf::Message & pro)
{
  Descriptor const* pd = pro.GetDescriptor();
  Reflection const* reflec = pro.GetReflection();
  if (!CONFIRM(pd && reflec)) {
    ij.set_failbit();
    return;
  }
  ijobject ijo = ij.begin_object();
  while (!ijo.at_end()) {
    FieldDescriptor const* field = pd->FindFieldByName(ijo.key());
    if (NULL == field) {
      ij.set_failbit();
      return;
    }
    if (field->is_repeated()) {
      reflec->ClearField(&pro, field);
      ijarray ija = ijo->begin_array();
      while (!ija.at_end()) {
        parse_repeated_field(*ija, &pro, field, reflec);
      }
    } else {
      parse_singular_field(*ijo, &pro, field, reflec);
    }
  }
}

void parse_proto_type(ijnode & ij, protobuf::Message & pro)
{
  pro.Clear();
  merge_proto_type(ij, pro);
  if (!pro.IsInitialized()) { ij.set_failbit(); }
}


} // namespace

namespace jios {

void jios_read(ijnode & ij, google::protobuf::Message & pro)
{
  cel::parse_proto_type(ij, pro);
}

}

