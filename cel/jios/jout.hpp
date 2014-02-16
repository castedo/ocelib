#ifndef CEL_JIOS_JOUT_HPP
#define CEL_JIOS_JOUT_HPP

#include <boost/noncopyable.hpp>
#include <boost/lexical_cast.hpp>
#include <spl/safe_ptr.hpp>

namespace cel {


// ojarray

class ojnode;

class ojarray
  : boost::noncopyable
{
public:
  ojarray(ojarray && rhs);

  ojarray & operator = (ojarray && rhs);

  ojnode & operator * () { return *cur_; }

  ojnode * operator -> () { return cur_.get(); }

  void terminate();

public:
  ojarray(spl::safe_ptr<ojnode> const& sub);
  ojarray(spl::safe_ptr<ojnode> const& sub,
          spl::safe_ptr<ojnode> const& parent);

private:
  spl::safe_ptr<ojnode> cur_;
  std::shared_ptr<ojnode> parent_;
};

// ojobject

class ojknode;

class ojobject
  : boost::noncopyable
{
public:
  ojobject(ojobject && rhs);

  ojobject & operator = (ojobject && rhs);

  ojknode & operator * () { return *cur_; }

  ojknode * operator -> () { return cur_.get(); }

  void terminate();

public:
  ojobject(spl::safe_ptr<ojknode> const& sub,
           spl::safe_ptr<ojnode> const& parent);

private:
  spl::safe_ptr<ojknode> cur_;
  std::shared_ptr<ojnode> parent_;
};

// ojnode & ojknode

class ojnode
{
public:
  ~ojnode() {}

  void print_null() { return do_print_null(); }
  void print(int32_t const& value) { return do_print(value); }
  void print(uint32_t const& value) { return do_print(value); }
  void print(int64_t const& value) { return do_print(value); }
  void print(uint64_t const& value) { return do_print(value); }
  void print(double const& value) { return do_print(value); }
  void print(float const& value) { return do_print(value); }
  void print(bool const& value) { return do_print(value); }
  void print(char ch) { return do_print(ch); }
  void print(char const* p) { return do_print(p); }
  void print(std::string const& value) { return do_print(value); }

  ojarray begin_array(bool multimode = false)
  {
     return do_begin_array(multimode);
  }

  ojobject begin_object(bool multimode = false)
  {
     return do_begin_object(multimode);
  }

  void flush() { do_flush(); }

protected:
  virtual void do_print_null() = 0;
  virtual void do_print(int32_t const& value) = 0;
  virtual void do_print(uint32_t const& value) = 0;
  virtual void do_print(int64_t const& value) = 0;
  virtual void do_print(uint64_t const& value) = 0;
  virtual void do_print(double const& value) = 0;
  virtual void do_print(float const& value) = 0;
  virtual void do_print(bool const& value) = 0;
  virtual void do_print(char ch) = 0;
  virtual void do_print(char const* p) = 0;
  virtual void do_print(std::string const& value) = 0;

  virtual ojarray do_begin_array(bool multimode) = 0;
  virtual ojobject do_begin_object(bool multimode) = 0;

  friend class ojarray;
  friend class ojobject;

  virtual void do_flush() = 0;
  virtual void do_close() = 0;
  virtual void do_terminate() = 0;
};

class ojknode
  : public ojnode
{
public:
  template<typename T>
  void key(T const& k)
  {
    do_key(boost::lexical_cast<std::string>(k));
  }

  void key(std::string const& k) { do_key(k); }

  template<typename KeyT, typename ValueT>
  void kprint(KeyT const& k, ValueT const& v)
  {
    do_key(k);
    do_print(v);
  }

private:
  virtual void do_key(std::string const& k) = 0;
};


} // namespace

#endif

