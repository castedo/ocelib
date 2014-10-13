#include <cel/jios/json_oj.hpp>

#include <cel/utility.hpp>
#include <boost/optional.hpp>

using namespace std;
using namespace spl;

namespace cel {


// json escaping

template<class Ch>
void json_escape(std::basic_ostream<Ch> & out, std::basic_string<Ch> const& in)
{
  // Modified version of function from boost PTree code.
  typename std::basic_string<Ch>::const_iterator b = in.begin();
  typename std::basic_string<Ch>::const_iterator e = in.end();
  while (b != e)
  {
      // We escape everything outside ASCII, because this code can't
      // handle high unicode characters.
      if (*b >= 0x20 && *b != '"' && *b != '\\' && *b <= 0xFF) { out << *b; }
      else if (*b == Ch('\b')) out << Ch('\\') << Ch('b');
      else if (*b == Ch('\f')) out << Ch('\\') << Ch('f');
      else if (*b == Ch('\n')) out << Ch('\\') << Ch('n');
      else if (*b == Ch('\r')) out << Ch('\\') << Ch('r');
      else if (*b == Ch('"')) out << Ch('\\') << Ch('"');
      else if (*b == Ch('\\')) out << Ch('\\') << Ch('\\');
      else
      {
          const char *hexdigits = "0123456789ABCDEF";
          typedef typename boost::make_unsigned<Ch>::type UCh;
          unsigned long u = (std::min)(static_cast<unsigned long>(
                                           static_cast<UCh>(*b)),
                                       0xFFFFul);
          int d1 = u / 4096; u -= d1 * 4096;
          int d2 = u / 256; u -= d2 * 256;
          int d3 = u / 16; u -= d3 * 16;
          int d4 = u;
          out << Ch('\\') << Ch('u');
          out << Ch(hexdigits[d1]) << Ch(hexdigits[d2]);
          out << Ch(hexdigits[d3]) << Ch(hexdigits[d4]);
      }
      ++b;
  }
}

// ostream_ojnode

class ostream_ojnode
  : public ojnode
  , public enable_safe_from_this<ojnode>
  , boost::noncopyable
{
public:
  virtual ~ostream_ojnode() {}

  ostream_ojnode(safe_ptr<ostream> const& os, char delim)
    : os_(os)
    , multimode_(false)
    , o_delim_(delim)
    , state_(CLEARED)
    , precomma_(false)
  {
    init(false);
  }

  ostream_ojnode(safe_ptr<std::ostream> const& os,
                      bool in_object,
                      bool multimode)
    : os_(os)
    , multimode_(multimode)
    , state_(CLEARED)
    , precomma_(false)
  {
    init(in_object);
  }

  template<typename T> void do_print_impl(T const& value);

  virtual void do_print_null();
  virtual void do_print(int32_t const& value) { do_print_impl(value); }
  virtual void do_print(uint32_t const& value) { do_print_impl(value); }
  virtual void do_print(int64_t const& value) { do_print_impl(value); }
  virtual void do_print(uint64_t const& value) { do_print_impl(value); }
  virtual void do_print(double const& value) { do_print_impl(value); }
  virtual void do_print(float const& value) { do_print_impl(value); }
  virtual void do_print(bool const& value) { do_print_impl(value); }
  virtual void do_print(char ch) { do_print_impl(std::string(1, ch)); }
  virtual void do_print(char const* p) { do_print_impl(std::string(p)); }
  virtual void do_print(std::string const& value) { do_print_impl(value); }

  virtual ojarray do_begin_array(bool multimode);
  virtual ojobject do_begin_object(bool multimode);

  virtual void do_key(std::string const& k) { prekey_ = k; }

  virtual void do_flush();
  virtual void do_close();
  virtual void do_terminate();

private:
  void init(bool object);

  virtual void post_comma_whitespace() {}
  virtual safe_ptr<ojnode> make_sub_struct(safe_ptr<ostream> const& os,
                                            bool in_object,
                                            bool multimode);
  virtual void pre_close_whitespace() {}

  void do_open();
  void out_prefix();
  void out_suffix();

protected:
  safe_ptr<std::ostream> const os_;
  bool multimode_;

private:
  boost::optional<char> const o_delim_;
  enum { CLEARED,   // cleared for printing (again)
         OPENED,    // open structure printing
         TERMINATED // no more printing should be done
  } state_;
  bool precomma_;
  boost::optional<std::string> prekey_;
};

class pretty_ojnode
  : public ostream_ojnode
{
public:
  virtual ~pretty_ojnode() {}

  pretty_ojnode(safe_ptr<std::ostream> const& os, char delim)
    : ostream_ojnode(os, delim)
    , indent_(0)
  {
  }

private:
  // json array or object
  pretty_ojnode(safe_ptr<std::ostream> const& os,
       bool in_object,
       bool multimode,
       size_t indent)
    : ostream_ojnode(os, in_object, multimode)
    , indent_(indent)
  {
    newline();
  }

  virtual void post_comma_whitespace();
  virtual safe_ptr<ojnode> make_sub_struct(safe_ptr<ostream> const& os,
                                            bool in_object,
                                            bool multimode);
  virtual void pre_close_whitespace();

  void newline();

  size_t indent_;
};

template<typename T>
void json_print(std::ostream & os, T const& value)
{
  os << value;
}

void json_print(std::ostream & os, bool const& value)
{
  os << (value ? "true" : "false");
}

void json_print(std::ostream & os, std::string const& value)
{
  os << '"';
  json_escape(os, value);
  os << '"';
}

void ostream_ojnode::do_print_null()
{
  if (CLEARED == state_) {
    out_prefix();
    *os_ << "null";
    out_suffix();
  } else {
    os_->setstate(std::ios_base::failbit);
  }
}

template<typename T>
void ostream_ojnode::do_print_impl(T const& value)
{
  if (CLEARED == state_) {
    out_prefix();
    json_print(*os_, value);
    out_suffix();
  } else {
    os_->setstate(std::ios_base::failbit);
  }
}

safe_ptr<ojnode>
    ostream_ojnode::make_sub_struct(safe_ptr<ostream> const& os,
                                         bool in_object,
                                         bool multimode)
{
  return make_safe<ostream_ojnode>(os_, in_object, multimode);
}

void ostream_ojnode::do_open()
{
  if (CLEARED == state_) {
    out_prefix();
    state_ = OPENED;
  } else {
    os_->setstate(std::ios_base::failbit);
  }
}

ojarray ostream_ojnode::do_begin_array(bool multimode)
{
  do_open();
  auto sub = this->make_sub_struct(os_, false, multimode);
  return ojarray(sub, safe_from_this());
}

ojobject ostream_ojnode::do_begin_object(bool multimode)
{
  do_open();
  auto sub = this->make_sub_struct(os_, true, multimode);
  return ojobject(sub, safe_from_this());
}

void ostream_ojnode::do_flush()
{
  os_->flush();
}

void ostream_ojnode::do_close()
{
  if (CONFIRM(OPENED == state_)) {
    out_suffix();
    state_ = CLEARED;
  } else {
    os_->setstate(std::ios_base::failbit);
  }
}

void ostream_ojnode::do_terminate()
{
  if (CLEARED == state_) {
    if (!o_delim_) {
      this->pre_close_whitespace();
      bool in_object(prekey_);
      *os_ << (in_object ? '}' : ']');
    }
    state_ = TERMINATED;
  } else {
    os_->setstate(std::ios_base::failbit);
  }
}

void ostream_ojnode::init(bool in_object)
{
  if (in_object) { prekey_ = ""; }
  if (!o_delim_) {
    *os_ << (in_object ? '{' : '[');
  }
}

void ostream_ojnode::out_prefix()
{
  if (!o_delim_) {
    if (precomma_) {
      *os_ << ',';
      this->post_comma_whitespace();
    } else {
      precomma_ = true;
    }
  }
  if (prekey_) {
    *os_ << '"';
    json_escape(*os_, *prekey_);
    *os_ << '"' << ':';
    prekey_ = "";
  }
}

void ostream_ojnode::out_suffix()
{
  if (o_delim_) {
    if (*o_delim_ != EOF) {
      *os_ << *o_delim_;
      if (*o_delim_ != '\n') {
        *os_ << '\n';
      }
    }
    *os_ << std::flush;
  }
}

// ojarray

ojarray::ojarray(ojarray && rhs)
  : cur_(std::move(rhs.cur_))
  , parent_(std::move(rhs.parent_))
{
}

ojarray::ojarray(spl::safe_ptr<ojnode> const& sub)
  : cur_(sub)
{
}

ojarray::ojarray(spl::safe_ptr<ojnode> const& sub,
                 spl::safe_ptr<ojnode> const& parent)
  : cur_(sub)
  , parent_(parent)
{
}

ojarray & ojarray::operator = (ojarray && rhs)
{
  parent_ = std::move(rhs.parent_);
  cur_ = std::move(rhs.cur_);
  BOOST_ASSERT(!rhs.parent_);
  return *this;
}

void ojarray::terminate()
{
  cur_->do_terminate();
  if (parent_) { parent_->do_close(); }
}

// ojobject
 
ojobject::ojobject(ojobject && rhs)
  : cur_(std::move(rhs.cur_))
  , parent_(std::move(rhs.parent_))
{
  BOOST_ASSERT(!rhs.parent_);
}

ojobject::ojobject(spl::safe_ptr<ojnode> const& sub,
                   spl::safe_ptr<ojnode> const& parent)
  : cur_(sub)
  , parent_(parent)
{
}

ojobject & ojobject::operator = (ojobject && rhs)
{
  parent_ = std::move(rhs.parent_);
  cur_ = std::move(rhs.cur_);
  BOOST_ASSERT(!rhs.parent_);
  return *this;
}

void ojobject::terminate()
{
  cur_->do_terminate();
  if (parent_) { parent_->do_close(); }
}

// pretty_ojnode

safe_ptr<ojnode>
    pretty_ojnode::make_sub_struct(safe_ptr<ostream> const& os,
                                           bool in_object,
                                           bool multimode)
{
  size_t sub_indent = indent_ + (multimode ? 1 : 0);
  auto p = new pretty_ojnode(os_, in_object, multimode, sub_indent);
  return safe_ptr<pretty_ojnode>(p);
}

void pretty_ojnode::pre_close_whitespace()
{
  --indent_;
  newline();
}

void pretty_ojnode::post_comma_whitespace()
{
  if (!multimode_) { *os_ << ' '; }
  else { newline(); }
}

void pretty_ojnode::newline()
{
  if (multimode_) {
    *os_ << '\n';
    for (size_t i=0; i < indent_; ++i) { *os_ << '\t'; }
  }
}

// root factory function

safe_ptr<ojnode> make_lined_json_ojnode(safe_ptr<ostream> const& os)
{
  return make_safe<ostream_ojnode>(os, '\n');
}

safe_ptr<ojnode> make_pretty_json_ojnode(safe_ptr<ostream> const& os, char delim)
{
  return make_safe<pretty_ojnode>(os, delim);
}

ojarray make_json_ojroot(safe_ptr<std::ostream> const& os, char delim)
{
  return ojarray(make_safe<pretty_ojnode>(os, delim));
}


} // namespace

