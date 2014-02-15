#ifndef CEL_UTILITY_HPP
#define CEL_UTILITY_HPP

#include <boost/assert.hpp>


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


#endif

