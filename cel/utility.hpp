#ifndef CEL_UTILITY_HPP
#define CEL_UTILITY_HPP

#include <boost/assert.hpp>

namespace cel {


#if defined(BOOST_DISABLE_ASSERTS)
# define CONFIRM(expr) (expr)
#else
# if defined(BOOST_ENABLE_ASSERT_HANDLER)
#  define CONFIRM(expr) ((expr) ? true : (::boost::assertion_failed(#expr, \
                         BOOST_CURRENT_FUNCTION, __FILE__, __LINE__), false))
# else
#  define CONFIRM(expr) (expr)
# endif
#endif


// null_deleter for shared_ptr and safe_ptr to statically allocated objects

struct null_deleter
{
  void operator()(void const *) const {}
};


} // namespace coost

#endif

