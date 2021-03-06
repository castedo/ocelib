#include <boost/test/unit_test.hpp>
#include <cel/potential_ptr.hpp>

using namespace std;
using namespace cel;

boost::unit_test::test_suite* init_unit_test_suite(int, char* [])
{
  return 0;
}

BOOST_AUTO_TEST_CASE( potential_ptr_test )
{
  potential_ptr<int> p0;
  potential_ptr<int> p1(unique_ptr<int>(new int(12)));
  potential_ptr<int> p2(unique_ptr<int>(new int(12)));
  potential_ptr<int> p3;
  p3 = p2;
  BOOST_CHECK( p0 != p1 );
  BOOST_CHECK( p1 != p2 );
  BOOST_CHECK( p3 != p1 );
  BOOST_CHECK( p3 == p2 );
  BOOST_CHECK( p3 );
  BOOST_CHECK_EQUAL( *p3, 12 );

  potential_ptr<int> a;
  potential_ptr<int> b;
  potential_ptr<int> c;
  b = c;
  a = b;
  BOOST_CHECK( a == b );
  BOOST_CHECK( a == c );
  b.potential().realize(unique_ptr<int>(new int(15)));
  BOOST_CHECK( a == b );
  BOOST_CHECK( a == c );
  BOOST_CHECK_EQUAL( *a, 15 );
  BOOST_CHECK_EQUAL( *b, 15 );
  BOOST_CHECK_EQUAL( *c, 15 );
  unique_ptr<int> nova(new int(30));
  BOOST_CHECK( not c.potential().realize_try(move(nova)) );
  BOOST_CHECK( nova );
  BOOST_CHECK_EQUAL( *c, 15 );

  c.reset();
  BOOST_CHECK( !c );
}

BOOST_AUTO_TEST_CASE( potential_ptr_equate_tests )
{
  potential_ptr<int> p0;
  potential_ptr<int> p1;
  potential_ptr<int> p2;

  equate(p1.potential(), p0.potential());
  equate(p2.potential(), p0.potential());
  p0.potential().realize(unique_ptr<int>(new int(12)));
  BOOST_CHECK( p1 == p0 );
  BOOST_CHECK( p2 == p1 );
  BOOST_CHECK_EQUAL( *p0, 12 );
  BOOST_CHECK_EQUAL( *p1, 12 );
  BOOST_CHECK_EQUAL( *p2, 12 );
}

BOOST_AUTO_TEST_CASE( potential_ptr_equate_realized )
{
  potential_ptr<int> p0;
  p0.potential().realize(unique_ptr<int>(new int(12)));
  potential_ptr<int> p1;
  potential_ptr<int> p2;
  equate(p1.potential(), p0.potential());
  equate(p0.potential(), p2.potential());
  BOOST_CHECK( p1 == p0 );
  BOOST_CHECK( p2 == p1 );
  BOOST_CHECK_EQUAL( *p0, 12 );
  BOOST_CHECK_EQUAL( *p1, 12 );
  BOOST_CHECK_EQUAL( *p2, 12 );
}

BOOST_AUTO_TEST_CASE( potential_ptr_equate_realized_copied )
{
  potential_ptr<int> p0;
  p0.potential().realize(unique_ptr<int>(new int(12)));
  potential_ptr<int> p3 = p0;
  potential_ptr<int> p1;
  potential_ptr<int> p2;
  equate(p1.potential(), p0.potential());
  equate(p0.potential(), p2.potential());
  BOOST_CHECK( p1 == p0 );
  BOOST_CHECK( p2 == p1 );
  BOOST_REQUIRE( p0 );
  BOOST_CHECK_EQUAL( *p0, 12 );
  BOOST_REQUIRE( p1 );
  BOOST_CHECK_EQUAL( *p1, 12 );
  BOOST_REQUIRE( p2 );
  BOOST_CHECK_EQUAL( *p2, 12 );
  BOOST_REQUIRE( p3 );
  BOOST_CHECK_EQUAL( *p3, 12 );
}

