%{
#include <limits>
#include <boost/date_time/posix_time/posix_time_types.hpp>
%}

%include <std_vector.i>

%typemap("rtype") 
    boost::posix_time::ptime,
    boost::posix_time::ptime const&
    std::vector<boost::posix_time::ptime>,
    std::vector<boost::posix_time::ptime> const&
"POSIXct";

%typemap(scheck)
    boost::posix_time::ptime,
    boost::posix_time::ptime const&
%{
    if(length($input) > 1) {
        warning("using only the first element of $input");
    }
%}

%fragment("double2ptime", "header") {
    boost::posix_time::ptime double2ptime(double x) {
      using namespace boost::posix_time;
      if (std::isnan(x)) {
        return ptime(not_a_date_time);
      }
      if (std::isinf(x)) {
        return ptime(x > 0 ? pos_infin : neg_infin);
      }
      int64_t ticks = std::round(x * time_duration::ticks_per_second());
      ptime epoch(boost::gregorian::date(1970, 1, 1));
      return epoch + time_duration(0, 0, 0, ticks);
    }
}

%fragment("ptime2double", "header") {
    double ptime2double(boost::posix_time::ptime t) {
      using namespace boost::posix_time;
      if (t.is_not_a_date_time()) {
        return std::numeric_limits<double>::quiet_NaN();
      }
      if (t.is_pos_infinity()) {
        return std::numeric_limits<double>::infinity();
      }
      if (t.is_neg_infinity()) {
        return -std::numeric_limits<double>::infinity();
      }
      time_duration dur = t - ptime(boost::gregorian::date(1970, 1, 1));
      return double(dur.ticks()) / time_duration::ticks_per_second();
    }
}

%fragment("PtimeVectorTraits", "header",
    fragment="StdSequenceTraits",
    fragment="ptime2double",
    fragment="double2ptime")
%{
  namespace swig {
    template <>
    struct traits_from_ptr<std::vector<boost::posix_time::ptime> > {
      static SEXP from (std::vector<boost::posix_time::ptime> *val, int owner = 0) {
        SEXP result;
        PROTECT(result = Rf_allocVector(REALSXP, val->size()));
        for (unsigned pos = 0; pos < val->size(); pos++)
        {
          NUMERIC_POINTER(result)[pos] = ptime2double((*val)[pos]);
        }
        UNPROTECT(1);
        return(result);
      }
    };
    template <>
    struct traits_asptr < std::vector<boost::posix_time::ptime> > {
      static int asptr(SEXP obj, std::vector<boost::posix_time::ptime> **val) {
        std::vector<boost::posix_time::ptime> *p;
        // not sure how to check the size of the SEXP obj is correct
        unsigned int sexpsz = Rf_length(obj);
        p = new std::vector<boost::posix_time::ptime>(sexpsz);
        double *S = NUMERIC_POINTER(obj);
        for (unsigned pos = 0; pos < p->size(); pos++)
        {
          (*p)[pos] = double2ptime(static_cast<double>(S[pos]));
        }
        int res = SWIG_OK;
        if (SWIG_IsOK(res)) {
          if (val) *val = p;
        }
        return res;
      }
    };
  }
%}

%fragment(SWIG_Traits_frag(std::vector<boost::posix_time::ptime>), "header",
    fragment="StdTraits",
    fragment="PtimeVectorTraits")
{
  namespace swig {
    template <>  struct traits< std::vector<boost::posix_time::ptime> > {
      typedef pointer_category category;
      static const char* type_name() {
        return "std::vector<boost::posix_time::ptime>";
      }
    };
  }
}

%typemap_traits_ptr(SWIG_TYPECHECK_VECTOR, std::vector<boost::posix_time::ptime>)

%typemap(scoercein)
    boost::posix_time::ptime,
    boost::posix_time::ptime const&,
    std::vector<boost::posix_time::ptime>,
    std::vector<boost::posix_time::ptime> const&
%{
    $input = as.double($input);
%}

%typemap(in, fragment="double2ptime")
    boost::posix_time::ptime,
    boost::posix_time::ptime const&
{
    $1 = double2ptime(%static_cast(REAL($input)[0], double));
}

%typemap(out, fragment="ptime2double")
    boost::posix_time::ptime,
    boost::posix_time::ptime const&
{
    $result = Rf_ScalarReal(ptime2double($1));
}

%typemap(scoerceout)
    boost::posix_time::ptime,
    boost::posix_time::ptime const&,
    std::vector<boost::posix_time::ptime>,
    std::vector<boost::posix_time::ptime> const&
%{
    $result <- as.POSIXct($result, origin="1970-01-01");
%}

