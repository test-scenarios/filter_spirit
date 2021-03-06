/**
 * @file AST adaptations
 *
 * @brief This file is intended only for BOOST_FUSION_ADAPT_STRUCT macros
 *
 * @details
 * - adaptations must be in global scope
 * - DO NOT adapt 1-member structs, use inheritance or overload operator= instead
 *   (Spirit is not intended to be used with 1-element fusions)
 * - there are some tricks available (partial adapations, rearranged order of adapted members)
 *   lookup Boost Fusion documentation for more details
 */
#pragma once

#include <fs/parser/ast.hpp>

#include <boost/fusion/include/adapt_struct.hpp>

// uncomment if adapting any such members
// but prefer Boost counterparts
// #include <boost/fusion/include/std_pair.hpp>
// #include <boost/fusion/include/std_array.hpp>
// #include <boost/fusion/include/std_tuple.hpp>

// for pretty-print of AST types
// #include <boost/fusion/include/io.hpp>
// #include <boost/fusion/sequence/io.hpp>
// #include <boost/optional/optional_io.hpp>

// allow Boost Fusion to pretty print AST types
// using boost::fusion::operator<<;
// using boost::fusion::operators::operator<<;

// ---- whitespace ----

// (nothing to adapt)

// ---- fundamental tokens ----

// (nothing to adapt)

// ---- literal types ----

// (nothing to adapt)

// ---- expressions ----

BOOST_FUSION_ADAPT_STRUCT(
	fs::parser::ast::function_call,
	name, arguments)

BOOST_FUSION_ADAPT_STRUCT(
	fs::parser::ast::price_range_query,
	name, arguments)

BOOST_FUSION_ADAPT_STRUCT(
	fs::parser::ast::value_expression,
	primary_expr, postfix_exprs)

// ---- definitions ----

BOOST_FUSION_ADAPT_STRUCT(
	fs::parser::ast::constant_definition,
	name, value)

// ---- rules ----

BOOST_FUSION_ADAPT_STRUCT(
	fs::parser::ast::comparison_condition,
	property, comparison_type, value)

BOOST_FUSION_ADAPT_STRUCT(
	fs::parser::ast::array_condition,
	property, exact_match, value)

BOOST_FUSION_ADAPT_STRUCT(
	fs::parser::ast::boolean_condition,
	property, value)

BOOST_FUSION_ADAPT_STRUCT(
	fs::parser::ast::unary_action,
	action_type, value)

// ---- filter structure ----

BOOST_FUSION_ADAPT_STRUCT(
	fs::parser::ast::rule_block,
	conditions, statements)

BOOST_FUSION_ADAPT_STRUCT(
	fs::parser::ast::filter_structure,
	definitions, statements)
