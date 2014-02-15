#ifndef CEL_JIOS_JSON_OJ_HPP
#define CEL_JIOS_JSON_OJ_HPP

#include <ostream>
#include <spl/safe_ptr.hpp>
#include <cel/jios/outj.hpp>

namespace cel {


// ojnode and ojarray simple implementations outputing JSON

spl::safe_ptr<ojnode>
    make_lined_json_ojnode(spl::safe_ptr<std::ostream> const& os);

spl::safe_ptr<ojnode>
    make_pretty_json_ojnode(spl::safe_ptr<std::ostream> const& os,
                            char delim = EOF);

ojarray make_json_ojroot(spl::safe_ptr<std::ostream> const& os,
                         char delim = EOF);


} // namespace

#endif

