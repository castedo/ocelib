#include <cel/jios/jin.hpp>

using namespace std;
using namespace spl;

namespace cel {


// null_ijnode

class null_ijnode
  : public ijnode
  , public spl::enable_safe_from_this<null_ijnode>
{
  bool fail_;

public:
  null_ijnode() : fail_(false) {}

  bool do_get_failbit() const { return fail_; }
  void do_set_failbit() { fail_ = true; }

  void do_parse(int32_t & dest) { do_set_failbit(); }
  void do_parse(uint32_t & dest) { do_set_failbit(); }
  void do_parse(int64_t & dest) { do_set_failbit(); }
  void do_parse(uint64_t & dest) { do_set_failbit(); }
  void do_parse(double & dest) { do_set_failbit(); }
  void do_parse(float & dest) { do_set_failbit(); }
  void do_parse(bool & dest) { do_set_failbit(); }
  void do_parse(std::string & dest) { do_set_failbit(); }
  void do_parse(ojnode & dest) { do_set_failbit(); }
  void do_ignore() { do_set_failbit(); }

  ijarray do_begin_array() {
    do_set_failbit();
    return ijarray(safe_from_this());
  }

  ijobject do_begin_object() {
    do_set_failbit();
    return ijobject(safe_from_this());
  }

  bool do_is_array() const { return false; }
  bool do_is_object() const { return false; }

  bool do_is_terminator() const { return true; }
  string do_key() const { return string(); }
};

// ijarray

ijarray::ijarray() : pimpl_(new null_ijnode()) {}

// ijobject 

ijobject::ijobject() : pimpl_(new null_ijnode()) {}


} // namespace

