%{
#
%}

%typemap(in) boost::gregorian::date {
  if (PyDate_Check($input)) {
    $1 = boost::gregorian::date(PyDateTime_GET_YEAR($input),
                                PyDateTime_GET_MONTH($input),
                                PyDateTime_GET_DAY($input));
  } else {
    PyErr_SetString(PyExc_TypeError, "not a datetime.date");
    return NULL;
  }
}

%typemap(in) boost::posix_time::ptime {
  if (PyDateTime_Check($input)) {
    boost::gregorian::date tempDate$1(
        PyDateTime_GET_YEAR($input),
        PyDateTime_GET_MONTH($input),
        PyDateTime_GET_DAY($input)
    );
    boost::posix_time::time_duration tempTime$1(
        PyDateTime_DATE_GET_HOUR($input),
        PyDateTime_DATE_GET_MINUTE($input),
        PyDateTime_DATE_GET_SECOND($input)
    );
    boost::posix_time::microseconds tempMicros$1(
        PyDateTime_DATE_GET_MICROSECOND($input)
    );
    $1 = boost::posix_time::ptime(tempDate$1, tempTime$1 + tempMicros$1);
  } else {
    PyErr_SetString(PyExc_TypeError, "not a datetime.datetime");
    return NULL;
  }
}

