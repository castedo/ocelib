dyn.load(paste("test_swig_R", .Platform$dynlib.ext, sep=""))
library("methods")
source("test_swig_R.R")
cacheMetaData(1)

stopifnot( all.equal(Y2K(), as.POSIXct("2000-01-01")) )
stopifnot( all.equal(NaN, as.double(not_a_date_time())) )
stopifnot( all.equal(Inf, as.double(pos_infin_time())) )
stopifnot( all.equal(-Inf, as.double(neg_infin_time())) )

t <- as.POSIXct("1969-07-20 20:17:40.123456")
stopifnot( all.equal(t, return_time(t)) )

t <- as.POSIXct(NaN, origin="1970-01-01")
stopifnot( all.equal(t, return_time(t)) )

t <- as.POSIXct(Inf, origin="1970-01-01")
stopifnot( all.equal(t, return_time(t)) )

t <- as.POSIXct(-Inf, origin="1970-01-01")
stopifnot( all.equal(t, return_time(t)) )


t <- as.POSIXct("1969-07-20 20:17:40.123456")
t <- c(t, t, t, t)
stopifnot( 4 == num_ptimes(t) )
stopifnot( all.equal( 3600*(0:3), as.double(add_hours(t) - t)) )

