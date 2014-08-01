#ifndef CEL_JIOS_JIN_HPP
#define CEL_JIOS_JIN_HPP

#include <spl/safe_ptr.hpp>
#include <cel/jios/jout.hpp>

namespace cel {


// ijarray

class ijnode;

class ijarray
  : boost::noncopyable
{
public:
  ijarray();

  ijarray(spl::safe_ptr<ijnode> const& pimpl) : pimpl_(pimpl) {}

  ijarray(ijarray && rhs) : pimpl_(rhs.pimpl_) {}

  ijarray & operator = (ijarray && rhs) { pimpl_ = rhs.pimpl_; return *this; }

  ijnode * operator -> () { return pimpl_.get(); }

  ijnode & operator * () { return *(pimpl_.get()); }

  bool at_end() const;

private:
  spl::safe_ptr<ijnode> pimpl_;
};

// ijobject 

class ijobject
  : boost::noncopyable
{
public:
  ijobject();

  ijobject(spl::safe_ptr<ijnode> const& pimpl) : pimpl_(pimpl) {}

  ijobject(ijobject && rhs) : pimpl_(rhs.pimpl_) {}

  ijobject & operator = (ijobject && r) { pimpl_ = r.pimpl_; return *this; }

  std::string key() const;

  ijnode * operator -> () { return pimpl_.get(); }

  ijnode & operator * () { return *(pimpl_.get()); }

  bool at_end() const;

private:
  spl::safe_ptr<ijnode> pimpl_;
};

// ijnode

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
  friend class ijarray;
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

inline bool ijarray::at_end() const
{
  return pimpl_->do_is_terminator() || pimpl_->fail();
}

inline std::string ijobject::key() const
{
  return pimpl_.get()->do_key();
}

inline bool ijobject::at_end() const
{
  return pimpl_->do_is_terminator() || pimpl_->fail();
}


} // namespace

#endif

