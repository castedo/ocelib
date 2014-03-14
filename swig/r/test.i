%module test_swig_R

%include "boost_ptime.i"

%inline {
  namespace test {
    extern boost::posix_time::ptime return_time(boost::posix_time::ptime t);
    extern boost::posix_time::ptime Y2K();
    extern boost::posix_time::ptime not_a_date_time();
    extern boost::posix_time::ptime pos_infin_time();
    extern boost::posix_time::ptime neg_infin_time();
  }
}

