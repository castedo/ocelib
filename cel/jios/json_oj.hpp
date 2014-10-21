#ifndef CEL_JIOS_JSON_OJ_HPP
#define CEL_JIOS_JSON_OJ_HPP

#include <ostream>
#include <spl/safe_ptr.hpp>
#include <cel/jios/jout.hpp>

namespace jios {


// ojnode and ojarray simple implementations outputing JSON

ojstream json_out(std::ostream & os, char delim = EOF);
ojstream lined_json_out(std::ostream & os);

ojarray make_json_ojroot(spl::safe_ptr<std::ostream> const& os,
                         char delim = EOF);


} // namespace

#endif

