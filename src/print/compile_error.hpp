#pragma once
#include "parser/parser.hpp"
#include "compiler/error.hpp"
#include <iosfwd>

namespace fs::print
{

void compile_error(
	compiler::error::error_variant error,
	const parser::lookup_data& lookup_data,
	std::ostream& error_stream);

}
