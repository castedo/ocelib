#ifndef CEL_JIOS_JIN_HPP
#define CEL_JIOS_JIN_HPP

#include <boost/concept_check.hpp>
#include <jios/jin.hpp>

namespace cel {


typedef jios::ijnode ijnode;
typedef jios::ijvalue ijvalue;
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

