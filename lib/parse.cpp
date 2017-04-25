#include "../cel/parse.hpp"

#include <boost/spirit/include/qi.hpp>

using std::string;
using namespace boost::spirit;

namespace cel {


bool parse(string::const_iterator it,
           string::const_iterator end,
           int32_t & out)
{
  return qi::parse(it, end, qi::int_, out) && it == end;
}

bool parse(string::const_iterator it,
           string::const_iterator end,
           uint32_t & out)
{
  return qi::parse(it, end, qi::uint_, out) && it == end;
}

bool parse_hex(string::const_iterator it,
               string::const_iterator end,
               uint32_t & out)
{
  return qi::parse(it, end, qi::hex, out) && it == end;
}

bool parse(string::const_iterator it,
           string::const_iterator end,
           int64_t & out)
{
  return qi::parse(it, end, qi::int_, out) && it == end;
}

bool parse(string::const_iterator it,
           string::const_iterator end,
           uint64_t & out)
{
  return qi::parse(it, end, qi::uint_, out) && it == end;
}

bool parse_hex(string::const_iterator it,
               string::const_iterator end,
               uint64_t & out)
{
  return qi::parse(it, end, qi::hex, out) && it == end;
}

bool parse(string::const_iterator & it,
           string::const_iterator end,
           double & out)
{
  return qi::parse(it, end, qi::double_, out);
}

bool parse( string const& in, bool & out )
{
  int i = -1;
  string::const_iterator it = in.begin();
  bool parsed = qi::parse(it, in.end(), qi::int_, i);
  if ( parsed && in.end() == it && ( i == 0 || i == 1 ) ) {
    out = (bool)i;
    return true;
  }
  return false;
}


} // namespace cel

