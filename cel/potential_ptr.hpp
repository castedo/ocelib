#ifndef CEL_POTENTIAL_PTR_HPP
#define CEL_POTENTIAL_PTR_HPP

#include <memory>
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
  
  potential_ptr(potential_ptr const& r)
    : pp_(r.pp_allocated()) {}

  potential_ptr & operator = (potential_ptr rhs) noexcept
  { swap(*this, rhs); return *this; }

  T * operator -> () const noexcept { return this->get(); }

  T & operator * () & noexcept { return *(this->get()); }

  explicit operator bool () const noexcept { return bool(this->get()); }

  bool operator ! () const noexcept { return !bool(*this); }

  bool operator != (potential_ptr const& r) const noexcept
  { return !(*this == r); }

  bool operator == (potential_ptr const& r) const noexcept
  {
    // pp_ points to unique_ptr, thus diff pp_ must have diff unique_ptr vals
    return pp_ && r.pp_ && pp_ == r.pp_;
  }

  T * get() const noexcept { return (pp_ ? pp_->get() : nullptr); }

  void reset() noexcept { pp_.reset(); }

  bool has_copies() const noexcept { return pp_ && pp_.use_count() > 1; }

  void realize(std::unique_ptr<T> && p)
  {
    if (not this->try_realize(std::move(p))) {
      BOOST_THROW_EXCEPTION(std::logic_error("potential_ptr already realized"));
    }
  }

  bool try_realize(std::unique_ptr<T> && p)
  {
    if (!p) return false;
    std::unique_ptr<T> & up = *pp_allocated();
    if (up) return false;
    up = std::move(p);
    return true;
  }

  friend void swap(potential_ptr & a, potential_ptr & b) noexcept
  { swap(a.pp_, b.pp_); }

private:
  std::shared_ptr<std::unique_ptr<T>> const& pp_allocated() const
  {
    return (pp_ ? pp_ : pp_ = std::make_shared<std::unique_ptr<T>>());
  }

  mutable std::shared_ptr<std::unique_ptr<T>> pp_;
};


} // namespace

#endif

