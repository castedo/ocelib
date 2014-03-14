#include <boost/date_time/posix_time/posix_time_types.hpp>

using namespace boost::posix_time;

namespace test {

ptime return_time(ptime t) { return t; }
ptime Y2K() { return ptime(boost::gregorian::date(2000, 1, 1)); }
ptime not_a_date_time() { return boost::posix_time::not_a_date_time; }
ptime pos_infin_time() { return boost::posix_time::pos_infin; }
ptime neg_infin_time() { return boost::posix_time::neg_infin; }

}

