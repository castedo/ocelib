%{
#include <limits>
#include <boost/date_time/posix_time/posix_time_types.hpp>
%}

%typemap("rtype") 
    boost::posix_time::ptime,
    boost::posix_time::ptime const&
"POSIXct";

%typemap(scheck)
    boost::posix_time::ptime,
    boost::posix_time::ptime const&
%{
    if(length($input) > 1) {
        warning("using only the first element of $input");
    }
%}

%typemap(scoercein)
    boost::posix_time::ptime,
    boost::posix_time::ptime const&
%{
    $input = as.double($input);
%}

%typemap(in)
    boost::posix_time::ptime,
    boost::posix_time::ptime const&
{
    using namespace boost::posix_time;
    double sec$1 = %static_cast(REAL($input)[0], double);
    if (std::isnan(sec$1)) {
      $1 = ptime(not_a_date_time);
    } else if (std::isinf(sec$1)) {
      $1 = ptime(sec$1 > 0 ? pos_infin : neg_infin);
    } else {
      int64_t ticks = std::round(sec$1* time_duration::ticks_per_second());
      ptime epoch(boost::gregorian::date(1970, 1, 1));
      $1 = epoch + time_duration(0, 0, 0, ticks);
    }
}

%typemap(out)
    boost::posix_time::ptime,
    boost::posix_time::ptime const&
{
    double double$1;
    using namespace boost::posix_time;
    if ($1.is_not_a_date_time()) {
      double$1 = std::numeric_limits<double>::quiet_NaN();
    } else if ($1.is_pos_infinity()) {
      double$1 = std::numeric_limits<double>::infinity();
    } else if ($1.is_neg_infinity()) {
      double$1 = -std::numeric_limits<double>::infinity();
    } else {
      time_duration t = $1 - ptime(boost::gregorian::date(1970, 1, 1));
      double$1 = double(t.ticks()) / time_duration::ticks_per_second();
    }
    $result = Rf_ScalarReal(double$1);
}

%typemap(scoerceout)
    boost::posix_time::ptime,
    boost::posix_time::ptime const&
%{
    $result <- as.POSIXct($result, origin="1970-01-01");
%}

