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
  { if (!real_) { real_.reset(new T()); } return real_; }

  mutable std::shared_ptr<T> real_;
};

/// potential_ptr
/* potential_ptr references a POTENTIAL non-null pointer value (ptr_potential)
 * that may yet to be "realized". Conceptually potential_ptr references a
 * separate ptr_potential instance with all usual pointer operations proxied to the
 * ptr_potential. A potential_ptr never represents a null address even though
 * get() may return nullptr. A return of nullptr only means the ptr_potential
 * has yet to be realized.  Assigning one potential_ptr to another means the
 * assignee and original potential_ptr both reference the same ptr_potential.
 * Once a ptr_potential has been realized (to a non-null addrees) it can not be
 * "re-realized" to a differnt address. Potential_ptr can be reset to
 * reference a new unique unrealized ptr_potential.
 */

template<class T> class potential_ptr;
template<class T> class ptr_potential;

template<class T>
void equate(ptr_potential<T> & a, ptr_potential<T> & b)
{
  if (not equate_try(a, b)) {
    BOOST_THROW_EXCEPTION(std::logic_error("invalid potential equate"));
  }
}

template<class T> bool equate_try(ptr_potential<T> & a, ptr_potential<T> & b);

template<class T>
class ptr_potential
{
public:
  bool operator == (ptr_potential const& rhs) const
  { return find_root_this() == rhs.find_root_this(); }

  bool operator != (ptr_potential const& rhs) const
  { return !(*this == rhs); }

  void realize(std::unique_ptr<T> && p)
  {
    if (not realize_try(std::move(p))) {
      BOOST_THROW_EXCEPTION(std::logic_error("invalid potential realize"));
    }
  }

  bool realize_try(std::unique_ptr<T> && p);

  friend bool equate_try<T>(ptr_potential<T> & a, ptr_potential<T> & b);

private:
  friend class potential_ptr<T>;
  friend class lazy_non_null_shared_ptr<ptr_potential>;

  ptr_potential() noexcept {}
  ptr_potential(std::unique_ptr<T> && p) : realized_(std::move(p)) {}
  ptr_potential(ptr_potential const& rhs) = default;
  ptr_potential & operator = (ptr_potential const&) = delete;

  T * deref();

  ptr_potential * find_root_this();
  ptr_potential const* find_root_this() const;

  std::shared_ptr<T> realized_;
  lazy_non_null_shared_ptr<ptr_potential> link_;
};

template<class T>
class potential_ptr
{
  ptr_potential<T> pot_;

public:
  potential_ptr() noexcept {}

  potential_ptr(std::unique_ptr<T> && p) : pot_(std::move(p)) {}

  potential_ptr & operator = (potential_ptr rhs) noexcept
  { swap(*this, rhs); return *this; }

  T * operator -> () const { return this->get(); }

  T & operator * () const { return *(this->get()); }

  explicit operator bool () const { return bool(this->get()); }

  bool operator ! () const { return !bool(*this); }

  bool operator != (potential_ptr const& r) const { return pot_ != r.pot_; }

  bool operator == (potential_ptr const& r) const { return pot_ == r.pot_; }

  // conceptually potential_ptr "points" to ptr_potential, so const_cast ok
  ptr_potential<T> & potential() const&
  { return const_cast<ptr_potential<T>&>(pot_); }

  T * get() const { return potential().deref(); }

  void reset() { pot_.realized_.reset(); pot_.link_.assign_default(); }

  friend
  void swap(potential_ptr & a, potential_ptr & b) noexcept { a.swap_impl(b); }

private:
  void swap_impl(potential_ptr & r) noexcept
  { swap(pot_.realized_, r.pot_.realized_); swap(pot_.link_, r.pot_.link_); }
};

// impl

template<class T>
T * ptr_potential<T>::deref()
{
  if (!realized_ && not link_.unique_default()) {
    realized_ = link_->find_root_this()->realized_;
  }
  return realized_.get();
}

template<class T>
bool ptr_potential<T>::realize_try(std::unique_ptr<T> && p)
{
  std::shared_ptr<T> & realized = find_root_this()->realized_;
  if (realized) return false;
  realized = std::move(p);
  return bool(realized);
}

template<class T>
bool equate_try(ptr_potential<T> & a, ptr_potential<T> & b)
{
  ptr_potential<T> * a_root = a.find_root_this();
  ptr_potential<T> * b_root = b.find_root_this();
  if (a_root == b_root) return true;
  if (a_root->realized_) {
    if (b_root->realized_) {
      BOOST_ASSERT( a_root->realized_ != b_root->realized_ );
      return false;
    }
    b_root->link_ = a_root->link_;
  } else {
    a_root->link_ = b_root->link_;
  }
  return true;
}

template<class T>
ptr_potential<T> * ptr_potential<T>::find_root_this()
{
  return (link_.unique_default() ? this : link_->find_root_this());
}

template<class T>
ptr_potential<T> const* ptr_potential<T>::find_root_this() const
{
  return (link_.unique_default() ? this : link_->find_root_this());
}


} // namespace

#endif

