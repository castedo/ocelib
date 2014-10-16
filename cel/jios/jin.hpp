#ifndef CEL_JIOS_JIN_HPP
#define CEL_JIOS_JIN_HPP

#include <boost/concept_check.hpp>
#include <spl/safe_ptr.hpp>
#include <cel/jios/jout.hpp>

namespace cel {


class ijnode;
typedef ijnode ijsource;

//! Stream of JSON-ish values (base for ijarray and ijobject)

class ijstream
  : boost::noncopyable
{
public:
  ijstream();

  ijstream(spl::safe_ptr<ijsource> const& pimpl) : pimpl_(pimpl) {}

  ijstream(ijstream && rhs) : pimpl_(rhs.pimpl_) {}

  ijstream & operator = (ijstream && rhs) { pimpl_ = rhs.pimpl_; return *this; }

  ijnode * operator -> () { return pimpl_.get(); }

  ijnode & operator * () { return *(pimpl_.get()); }

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

  ijarray(spl::safe_ptr<ijnode> const& pimpl) : ijstream(pimpl) {}
};

//! JSON-ish ijobject 

class ijobject
  : public ijstream
{
public:
  ijobject() {}

  ijobject(spl::safe_ptr<ijnode> const& pimpl) : ijstream(pimpl) {}

  std::string key() const;
};

//! JSON-ish value

class ijnode
  : boost::noncopyable
{
public:
  virtual ~ijnode() {}

  bool fail() const { return do_get_failbit(); }
  void set_failbit() { do_set_failbit(); }

  bool parse(int32_t & dest) { do_parse(dest); return !fail(); }
  bool parse(uint32_t & dest) { do_parse(dest); return !fail(); }
  bool parse(int64_t & dest) { do_parse(dest); return !fail(); }
  bool parse(uint64_t & dest) { do_parse(dest); return !fail(); }
  bool parse(double & dest) { do_parse(dest); return !fail(); }
  bool parse(float & dest) { do_parse(dest); return !fail(); }
  bool parse(bool & dest) { do_parse(dest); return !fail(); }
  bool parse(std::string & dest) { do_parse(dest); return !fail(); }
  bool parse(ojnode & dest) { do_parse(dest); return !fail(); }
  bool ignore() { do_ignore(); return !fail(); }

  ijarray begin_array() { return do_begin_array(); }
  ijobject begin_object() { return do_begin_object(); }

  bool is_array() const { return do_is_array(); }
  bool is_object() const { return do_is_object(); }

private:
  friend class ijstream;
  friend class ijobject;

  virtual bool do_get_failbit() const = 0;
  virtual void do_set_failbit() = 0;

  virtual void do_parse(int32_t & dest) = 0;
  virtual void do_parse(uint32_t & dest) = 0;
  virtual void do_parse(int64_t & dest) = 0;
  virtual void do_parse(uint64_t & dest) = 0;
  virtual void do_parse(double & dest) = 0;
  virtual void do_parse(float & dest) = 0;
  virtual void do_parse(bool & dest) = 0;
  virtual void do_parse(std::string & dest) = 0;
  virtual void do_parse(ojnode & dest) = 0;
  virtual void do_ignore() = 0;

  virtual ijarray do_begin_array() = 0;
  virtual ijobject do_begin_object() = 0;

  virtual bool do_is_array() const = 0;
  virtual bool do_is_object() const = 0;

  virtual bool do_is_terminator() const = 0;
  virtual std::string do_key() const = 0;
};

////////////////////////////////////////
/// inline method implementations

inline bool ijstream::at_end() const
{
  return pimpl_->do_is_terminator() || pimpl_->fail();
}

inline std::string ijobject::key() const
{
  return pimpl_.get()->do_key();
}

inline void jinput(ijnode & ij, int32_t & dest) { ij.parse(dest); }
inline void jinput(ijnode & ij, uint32_t & dest) { ij.parse(dest); }
inline void jinput(ijnode & ij, int64_t & dest) { ij.parse(dest); }
inline void jinput(ijnode & ij, uint64_t & dest) { ij.parse(dest); }
inline void jinput(ijnode & ij, double & dest) { ij.parse(dest); }
inline void jinput(ijnode & ij, float & dest) { ij.parse(dest); }
inline void jinput(ijnode & ij, bool & dest) { ij.parse(dest); }
inline void jinput(ijnode & ij, std::string & dest) { ij.parse(dest); }
inline void jinput(ijnode & ij, ojnode & dest) { ij.parse(dest); }

// jinable concept

template<class T>
struct Jinable
{
  BOOST_CONCEPT_USAGE(Jinable)
  {
    jinput(*p_jin, *p_obj);
  }

  ijnode * p_jin;
  T * p_obj;
};


} // namespace

#endif

