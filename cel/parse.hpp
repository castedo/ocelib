#ifndef CEL_PARSE_HPP
#define CEL_PARSE_HPP

#include <string>

namespace cel {


// atoi etc... replacements (safer and not slower)

bool parse(std::string::const_iterator begin,
           std::string::const_iterator end,
           int32_t & out);
bool parse(std::string::const_iterator begin,
           std::string::const_iterator end,
           uint32_t & out);
bool parse(std::string::const_iterator begin,
           std::string::const_iterator end,
           int64_t & out);
bool parse(std::string::const_iterator begin,
           std::string::const_iterator end,
           uint64_t & out);

inline bool parse(std::string const& in, int32_t & out)
{
  return parse(in.begin(), in.end(), out);
}

inline bool parse(std::string const& in, uint32_t & out)
{
  return parse(in.begin(), in.end(), out);
}

inline bool parse(std::string const& in, int64_t & out)
{
  return parse(in.begin(), in.end(), out);
}

inline bool parse(std::string const& in, uint64_t & out)
{
  return parse(in.begin(), in.end(), out);
}

bool parse_hex(std::string::const_iterator it,
               std::string::const_iterator end,
               uint32_t & out);

bool parse_hex(std::string::const_iterator it,
               std::string::const_iterator end,
               uint64_t & out);

bool parse(std::string const& in, bool & out);

bool parse(std::string::const_iterator & it,
           std::string::const_iterator end,
           double & out);

inline bool parse(std::string const& s, double & out)
{
  std::string::const_iterator it = s.begin();
  return parse(it, s.end(), out) && it == s.end();
}


} // namespace cel

#endif

