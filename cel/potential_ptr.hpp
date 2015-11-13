#ifndef CEL_POTENTIAL_PTR_HPP
#define CEL_POTENTIAL_PTR_HPP

#include <memory>
#include <boost/assert.hpp>
#include <boost/throw_exception.hpp>

namespace cel {


/// lazy_non_null_shared_ptr
/* lazy_non_null_shared_ptr represents a non-null shared_ptr defaulting to a
 * shared_ptr virtually pointing to a default value of T.
 */

template<class T>
class lazy_non_null_shared_ptr
{
public:
  lazy_non_null_shared_ptr() noexcept {}
  lazy_non_null_shared_ptr(lazy_non_null_shared_ptr const& rhs)
    : real_(rhs.be_real()) {}

  lazy_non_null_shared_ptr & operator = (lazy_non_null_shared_ptr rhs) noexcept
  { swap(*this, rhs); return *this; }

  T * operator -> () const { return be_real().get(); }

  T & operator * () const { return *be_real(); }

  void assign_default() { real_.reset(); } 

  bool unique() const { return !real_ || real_.unique(); }

  bool is_default() const { return !real_ || *real_ == T(); }

  bool unique_default() const { return unique() && is_default(); }

  friend void swap(lazy_non_null_shared_ptr & a,
                   lazy_non_null_shared_ptr & b) noexcept
  { swap(a.real_, b.real_); }

public:
  std::shared_ptr<T> const be_real() const&
  { return (real_ ? real_ : (real_ = std::make_shared<T>())); }

  mutable std::shared_ptr<T> real_;
};

/// potential_ptr
/* potential_ptr represents a non-null address that may yet to be realized.
 * A potential_ptr never represents a null address even though get() may
 * return nullptr. A return of nullptr only means the potential address has
 * yet to be realized.
 * Assigning one potential_ptr variable to another means the assignee and
 * original pointers will realize the same address if either is realized.
 * Once a potential_ptr has been realized (to a non-null addrees) it can not be
 * "re-realized" to a differnt address.
 * However, potential_ptr can be reset but loses all association with any
 * other potential_ptr.
 */

template<class T>
class potential_ptr
{
public:
  potential_ptr() noexcept {}

  potential_ptr(std::unique_ptr<T> && p) : realized_(std::move(p)) {}

  potential_ptr & operator = (potential_ptr rhs) noexcept
  { swap(*this, rhs); return *this; }

  T * operator -> () const { return this->get(); }

  T & operator * () const { return *(this->get()); }

  explicit operator bool () const { return bool(this->get()); }

  bool operator ! () const { return !bool(*this); }

  bool operator != (potential_ptr const& r) const
  { return !(*this == r); }

  bool operator == (potential_ptr const& r) const;

  T * get() const;

  void reset() { realized_.reset(); link_.assign_default(); }

  void realize(std::unique_ptr<T> && p)
  {
    if (not realize_try(std::move(p))) {
      BOOST_THROW_EXCEPTION(std::logic_error("invalid potential_ptr realize"));
    }
  }

  bool realize_try(std::unique_ptr<T> && p);

  void equate(potential_ptr & that)
  {
    if (not equate_try(that)) {
      BOOST_THROW_EXCEPTION(std::logic_error("invalid potential_ptr equate"));
    }
  }

  bool equate_try(potential_ptr & that);

  friend void swap(potential_ptr & a, potential_ptr & b) noexcept
  { swap(a.realized_, b.realized_); swap(a.link_, b.link_); }

private:
  potential_ptr * find_root_this();
  potential_ptr const* find_root_this() const;

  mutable std::shared_ptr<T> realized_;
  lazy_non_null_shared_ptr<potential_ptr> link_;
};

// impl

template<class T>
bool potential_ptr<T>::operator == (potential_ptr<T> const& rhs) const
{
  return find_root_this() == rhs.find_root_this();
}

template<class T>
T * potential_ptr<T>::get() const
{
  if (!realized_ && not link_.unique_default()) {
    realized_ = link_->find_root_this()->realized_;
  }
  return realized_.get();
}

template<class T>
bool potential_ptr<T>::realize_try(std::unique_ptr<T> && p)
{
  std::shared_ptr<T> & realized = find_root_this()->realized_;
  if (realized) return false;
  realized = std::move(p);
  return bool(realized);
}

template<class T>
bool potential_ptr<T>::equate_try(potential_ptr<T> & that)
{
  potential_ptr<T> * this_root = find_root_this();
  potential_ptr<T> * that_root = that.find_root_this();
  if (this_root == that_root) return true;
  if (this_root->realized_) {
    if (that_root->realized_) {
      BOOST_ASSERT( that_root->realized_ != that_root->realized_ );
      return false;
    }
    that_root->link_ = this_root->link_;
  } else {
    this_root->link_ = that_root->link_;
  }
  return true;
}

template<class T>
potential_ptr<T> * potential_ptr<T>::find_root_this()
{
  return (link_.unique_default() ? this : link_->find_root_this());
}

template<class T>
potential_ptr<T> const* potential_ptr<T>::find_root_this() const
{
  return (link_.unique_default() ? this : link_->find_root_this());
}


} // namespace

#endif

