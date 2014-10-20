#include <boost/test/unit_test.hpp>

#include <cel/jios/json_oj.hpp>

using namespace spl;
using namespace jios;
using namespace cel;

// streamed

BOOST_AUTO_TEST_CASE( empty_object_test )
{
  auto ss = make_safe<std::stringstream>();
  ojarray oj = make_json_ojroot(ss);
  BOOST_CHECK_EQUAL( ss->str(), "" );
  ojobject ojo = oj->begin_object();
  BOOST_CHECK_EQUAL( ss->str(), "{" );
  ojo.terminate();
  BOOST_CHECK_EQUAL( ss->str(), "{}" );
}

BOOST_AUTO_TEST_CASE( empty_array_test )
{
  auto ss = make_safe<std::stringstream>();
  ojarray oj = make_json_ojroot(ss);
  BOOST_CHECK_EQUAL( ss->str(), "" );
  ojarray oja = oj->begin_array();
  BOOST_CHECK_EQUAL( ss->str(), "[" );
  oja.terminate();
  BOOST_CHECK_EQUAL( ss->str(), "[]" );
}

BOOST_AUTO_TEST_CASE( lined_json_test )
{
  auto ss = make_safe<std::stringstream>();
  ojobject ojo = make_lined_json_ojnode(ss)->begin_object(true);
  ojarray oja = ojo["A"].begin_array(true);
  BOOST_CHECK_EQUAL( ss->str(), "{\"A\":[" );
  oja->print("B");
  BOOST_CHECK_EQUAL( ss->str(), "{\"A\":[\"B\"" );
  oja->print("C");
  BOOST_CHECK_EQUAL( ss->str(), "{\"A\":[\"B\",\"C\"" );
  oja.terminate();
  BOOST_CHECK_EQUAL( ss->str(), "{\"A\":[\"B\",\"C\"]" );
  ojo.terminate();
  BOOST_CHECK_EQUAL( ss->str(), "{\"A\":[\"B\",\"C\"]}\n" );
}

