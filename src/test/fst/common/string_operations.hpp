#pragma once

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <string_view>

namespace fst
{

[[nodiscard]]
std::string_view search(std::string_view input, std::string_view pattern);

[[nodiscard]]
std::string range_info_to_string(
	std::string_view all_code,
	std::string_view code_to_underline);

[[nodiscard]]
boost::test_tools::predicate_result compare_ranges(
	std::string_view expected,
	std::string_view actual,
	std::string_view whole_input /* <- for pretty-printing */);

// TODO improve logs for endline mismatches
[[nodiscard]]
boost::test_tools::predicate_result compare_strings(
	std::string_view expected,
	std::string_view actual);

}
