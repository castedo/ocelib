#ifndef CEL_JIOS_JIN_HPP
#define CEL_JIOS_JIN_HPP

#include <boost/optional.hpp>
#include <boost/concept_check.hpp>
#include <spl/safe_ptr.hpp>
#include <cel/jios/jout.hpp>

namespace jios {


typedef cel::ojnode ojnode;
class ijnode;
class ijsource;

void jios_read(ijnode & ij, bool & dest);
void jios_read(ijnode & ij, std::string & dest);
void jios_read(ijnode & ij, int64_t & dest);
void jios_read(ijnode & ij, double & dest);

void jios_read(ijnode & ij, int32_t & dest);
void jios_read(ijnode & ij, uint32_t & dest);
void jios_read(ijnode & ij, uint64_t & dest);
void jios_read(ijnode & ij, float & dest);
void jios_read(ijnode & src, ojnode & dest);

//! Stream of JSON-ish values (base for ijarray and ijobject)

class ijstream
  : boost::noncopyable
{
public:
  ijstream();

  ijstream(spl::safe_ptr<ijsource> const& pimpl) : pimpl_(pimpl) {}

  ijstream(ijstream && rhs) : pimpl_(rhs.pimpl_) {}

  ijstream & operator = (ijstream && rhs) { pimpl_ = rhs.pimpl_; return *this; }

  template<typename T> ijstream & operator >> (T & dest);

  ijnode * operator -> ();

  ijnode & operator * ();

  bool at_end() const;

protected:
  spl::safe_ptr<ijsource> pimpl_;
};

//! JSON-ish array

class ijarray
  : public ijstream
{
public:
  ijarray() {}

  ijarray(spl::safe_ptr<ijsource> const& pimpl) : ijstream(pimpl) {}
};

//! JSON-ish ijobject 

class ijobject
  : public ijstream
{
public:
  ijobject() {}

  ijobject(spl::safe_ptr<ijsource> const& pimpl) : ijstream(pimpl) {}

  std::string key() const;
};

//! JSON-ish value

enum class json_type {
    jnull,
    jbool,
    jinteger, //! backends must have fidelity of int64_t
    jfloat,   //! backends may have little fidelity for jfloat
    jstring,  //! UTF-8 string
    jarray,
    jobject};

class ijnode
  : boost::noncopyable
{
public:
  virtual ~ijnode() {}

  bool fail() const { return do_get_failbit(); }
  void set_failbit() { do_set_failbit(); }

  template<typename T>
  bool read(T & dest) { jios_read(*this, dest); return !fail(); }

  bool parse(int32_t & dest) { return this->read(dest); }
  bool parse(double & dest) { return this->read(dest); }
  bool parse(bool & dest) { return this->read(dest); }
  bool parse(std::string & dest) { return this->read(dest); }

  bool ignore() { do_ignore(); return !fail(); }

  ijarray begin_array() { return do_begin_array(); }
  ijobject begin_object() { return do_begin_object(); }
  bool hint_multiline() const { return do_hint_multiline(); }

  json_type type() const { return do_type(); }
  bool is_array() const { return json_type::jarray == do_type(); }
  bool is_object() const { return json_type::jobject == do_type(); }

private:
  friend void jios_read(ijnode & ij, bool & dest);
  friend void jios_read(ijnode & ij, std::string & dest);
  friend void jios_read(ijnode & ij, int64_t & dest);
  friend void jios_read(ijnode & ij, double & dest);

  virtual bool do_get_failbit() const = 0;
  virtual void do_set_failbit() = 0;

  virtual json_type do_type() const = 0;

  virtual void do_parse(int64_t & dest) = 0;
  virtual void do_parse(double & dest) = 0;
  virtual void do_parse(bool & dest) = 0;
  virtual void do_parse(std::string & dest) = 0;
  virtual void do_ignore() = 0;

  virtual ijarray do_begin_array() = 0;
  virtual ijobject do_begin_object() = 0;
  virtual bool do_hint_multiline() const = 0;
};

class ijsource : protected ijnode
{
  friend class ijstream;
  friend class ijobject;

  virtual bool do_is_terminator() const = 0;
  virtual std::string do_key() const = 0;
};

////////////////////////////////////////
/// inline method implementations

template<typename T>
inline ijstream & ijstream::operator >> (T & dest)
{
  pimpl_->read(dest);
  return *this;
}

inline ijnode * ijstream::operator -> ()
{
  return pimpl_.get();
}

inline ijnode & ijstream::operator * ()
{
  return *pimpl_;
}

inline void jios_read(ijnode & ij, bool & dest)
{
  ij.do_parse(dest);
}

inline void jios_read(ijnode & ij, std::string & dest)
{
  ij.do_parse(dest);
}

inline void jios_read(ijnode & ij, int64_t & dest)
{
  ij.do_parse(dest);
}

inline void jios_read(ijnode & ij, double & dest)
{
  ij.do_parse(dest);
}

inline bool ijstream::at_end() const
{
  return pimpl_->do_is_terminator() || pimpl_->fail();
}

inline std::string ijobject::key() const
{
  return pimpl_.get()->do_key();
}


} // namespace

namespace cel {


typedef jios::ijnode ijnode;
typedef jios::ijsource ijsource;
typedef jios::ijstream ijstream;
typedef jios::ijarray ijarray;
typedef jios::ijobject ijobject;

// jinable concept

template<class T>
struct Jinable
{
  BOOST_CONCEPT_USAGE(Jinable)
  {
    jios_read(*p_jin, *p_obj);
  }

  jios::ijnode * p_jin;
  T * p_obj;
};


} // namespace

#endif

