#include <boost/date_time/posix_time/posix_time_types.hpp>

using namespace boost::posix_time;
using std::vector;

namespace test {

ptime return_time(ptime t) { return t; }
ptime Y2K() { return ptime(boost::gregorian::date(2000, 1, 1)); }
ptime not_a_date_time() { return boost::posix_time::not_a_date_time; }
ptime pos_infin_time() { return boost::posix_time::pos_infin; }
ptime neg_infin_time() { return boost::posix_time::neg_infin; }

int num_ptimes(vector<ptime> const& v)
{
  return v.size();
}

vector<ptime> add_hours(vector<ptime> const& v)
{
  vector<ptime> ret(v.size());
  vector<ptime>::const_iterator it = v.begin();
  for (int i = 0; it != v.end(); ++it, ++i) {
    ret[i] = *it + hours(i);
  }
  return ret;
}

}

