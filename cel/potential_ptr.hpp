#ifndef CEL_POTENTIAL_PTR_HPP
#define CEL_POTENTIAL_PTR_HPP

#include <memory>
#include <boost/assert.hpp>
#include <boost/throw_exception.hpp>

namespace cel {


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

  potential_ptr(T * p)
    : pp_(std::make_shared<std::unique_ptr<T>>(p)) {}

  potential_ptr(std::unique_ptr<T> && p)
    : pp_(std::make_shared<std::unique_ptr<T>>(std::move(p))) {}
  
  potential_ptr(potential_ptr const& r);

  potential_ptr & operator = (potential_ptr rhs) noexcept
  { swap(*this, rhs); return *this; }

  T * operator -> () const { return this->get(); }

  T & operator * () & { return *(this->get()); }

  explicit operator bool () const { return bool(this->get()); }

  bool operator ! () const { return !bool(*this); }

  bool operator != (potential_ptr const& r) const noexcept
  { return !(*this == r); }

  bool operator == (potential_ptr const& r) const noexcept;

  T * get() const;

  void reset() { pp_.reset(); unrealized_.reset(); }

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
  { swap(a.pp_, b.pp_); swap(a.unrealized_, b.unrealized_); }

private:
  mutable std::shared_ptr<std::unique_ptr<T>> pp_;
  mutable std::shared_ptr<potential_ptr<T>> unrealized_;
};

// impl

template<class T>
potential_ptr<T>::potential_ptr(potential_ptr<T> const& that)
  : pp_(that.pp_), unrealized_(that.unrealized_)
{
  if (!pp_ && !unrealized_) {
    unrealized_ = (that.unrealized_ = std::make_shared<potential_ptr<T>>());
  }
}

template<class T>
bool potential_ptr<T>::operator == (potential_ptr<T> const& that) const noexcept
{
  if (this == &that) return true;
  if (unrealized_) return that == *unrealized_;
  if (that.unrealized_) return *that.unrealized_ == *this;
  // pp_ points to unique_ptr, thus diff pp_ must have diff unique_ptr vals
  return pp_ && that.pp_ && pp_ == that.pp_;
}

template<class T>
T * potential_ptr<T>::get() const
{
  if (unrealized_) {
    BOOST_ASSERT(!pp_);
    pp_ = unrealized_->pp_;
    if (pp_) {
      unrealized_.reset();
    }
  }
  return (pp_ ? pp_->get() : nullptr);
}

template<class T>
bool potential_ptr<T>::realize_try(std::unique_ptr<T> && p)
{
  if (!p || pp_) return false;
  if (unrealized_) {
    unrealized_->realize_try(std::move(p));
    pp_ = unrealized_->pp_;
    if (!pp_) return false;
    unrealized_.reset();
  } else {
    pp_ = std::make_shared<std::unique_ptr<T>>();
    *pp_ = std::move(p);
  }
  return true;
}

template<class T>
bool potential_ptr<T>::equate_try(potential_ptr<T> & that)
{
  if (this == &that) return true;
  if (unrealized_) return that.equate_try(*unrealized_);
  if (that.unrealized_) return that.unrealized_->equate_try(*this);
  if (!pp_) {
    *this = that;
    return true;
  }
  return that.pp_ == pp_;
}


} // namespace

#endif

