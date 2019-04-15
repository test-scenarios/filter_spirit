#include "parser/parser.hpp"
#include "core/version.hpp"
#include "log/null_logger.hpp"
#include "log/buffered_logger.hpp"
#include "utility/holds_alternative.hpp"

#include <gtest/gtest.h>

#include <string_view>
#include <string>

namespace
{

using namespace fs;

class parser_fixture : public testing::Test
{
protected:
	std::string minimal_input = []()
	{
		namespace v = core::version;
		return "version: " + std::to_string(v::major) + "." + std::to_string(v::minor) + "." + std::to_string(v::patch) + "\n"
		"config: {}";
	}();

	null_logger null_log;
	buffered_logger buffer_log;
};

// ---- BASIC POSITIVE TESTS ----

// parse minimal possible input
TEST_F(parser_fixture, minimal_input)
{
	const std::optional<parser::parse_data> parse_result = parser::parse(minimal_input, buffer_log);
	ASSERT_TRUE(parse_result.has_value()) << "parse of:\n" << minimal_input << "\nfailed\n" << buffer_log.flush_out();
	const parser::ast::ast_type& ast = (*parse_result).ast;

	const parser::ast::config& config = ast.config;
	ASSERT_TRUE(config.params.empty());

	const auto version = ast.version_data.get_value();
	ASSERT_EQ(version.major, core::version::major);
	ASSERT_EQ(version.minor, core::version::minor);
	ASSERT_EQ(version.patch, core::version::patch);
}

TEST_F(parser_fixture, color_definitions)
{
	const std::string input = minimal_input + "\n"
		"const color_first = RGB( 11,  22,  33)\n"
		"const color_black = RGB(  0,   1,   2, 255)\n"
		"const color_other = color_black";

	const std::optional<parser::parse_data> parse_result = parser::parse(input, buffer_log);
	ASSERT_TRUE(parse_result.has_value()) << "parse of:\n" << input << "\nfailed\n" << buffer_log.flush_out();
	namespace pa = parser::ast;
	const pa::ast_type& ast = (*parse_result).ast;

	const std::vector<pa::constant_definition>& defs = ast.constant_definitions;
	ASSERT_EQ(static_cast<int>(defs.size()), 3);

	EXPECT_EQ(defs[0].name.value, "color_first");
	EXPECT_EQ(defs[1].name.value, "color_black");
	EXPECT_EQ(defs[2].name.value, "color_other");

	ASSERT_TRUE(holds_alternative<pa::function_call>(defs[0].value.var));
	const auto& f0 = boost::get<pa::function_call>(defs[0].value.var);
	EXPECT_EQ(f0.name.value, "RGB");
	ASSERT_EQ(static_cast<int>(f0.arguments.size()), 3);

	ASSERT_TRUE(holds_alternative<pa::literal_expression>(f0.arguments[0].var));
	const auto& f0_arg0 = boost::get<pa::literal_expression>(f0.arguments[0].var);
	ASSERT_TRUE(holds_alternative<pa::integer_literal>(f0_arg0.var));
	const auto& f0_arg0_int = boost::get<pa::integer_literal>(f0_arg0.var);
	ASSERT_EQ(f0_arg0_int.value, 11);

	ASSERT_TRUE(holds_alternative<pa::literal_expression>(f0.arguments[1].var));
	const auto& f0_arg1 = boost::get<pa::literal_expression>(f0.arguments[1].var);
	ASSERT_TRUE(holds_alternative<pa::integer_literal>(f0_arg1.var));
	const auto& f0_arg1_int = boost::get<pa::integer_literal>(f0_arg1.var);
	EXPECT_EQ(f0_arg1_int.value, 22);

	ASSERT_TRUE(holds_alternative<pa::literal_expression>(f0.arguments[2].var));
	const auto& f0_arg2 = boost::get<pa::literal_expression>(f0.arguments[2].var);
	ASSERT_TRUE(holds_alternative<pa::integer_literal>(f0_arg2.var));
	const auto& f0_arg2_int = boost::get<pa::integer_literal>(f0_arg2.var);
	EXPECT_EQ(f0_arg2_int.value, 33);

	ASSERT_TRUE(holds_alternative<pa::function_call>(defs[1].value.var));
	const auto& f1 = boost::get<pa::function_call>(defs[1].value.var);
	EXPECT_EQ(f1.name.value, "RGB");
	ASSERT_EQ(static_cast<int>(f1.arguments.size()), 4);

	ASSERT_TRUE(holds_alternative<pa::literal_expression>(f1.arguments[0].var));
	const auto& f1_arg0 = boost::get<pa::literal_expression>(f1.arguments[0].var);
	ASSERT_TRUE(holds_alternative<pa::integer_literal>(f1_arg0.var));
	const auto& f1_arg0_int = boost::get<pa::integer_literal>(f1_arg0.var);
	EXPECT_EQ(f1_arg0_int.value, 0);

	ASSERT_TRUE(holds_alternative<pa::literal_expression>(f1.arguments[1].var));
	const auto& f1_arg1 = boost::get<pa::literal_expression>(f1.arguments[1].var);
	ASSERT_TRUE(holds_alternative<pa::integer_literal>(f1_arg1.var));
	const auto& f1_arg1_int = boost::get<pa::integer_literal>(f1_arg1.var);
	EXPECT_EQ(f1_arg1_int.value, 1);

	ASSERT_TRUE(holds_alternative<pa::literal_expression>(f1.arguments[2].var));
	const auto& f1_arg2 = boost::get<pa::literal_expression>(f1.arguments[2].var);
	ASSERT_TRUE(holds_alternative<pa::integer_literal>(f1_arg2.var));
	const auto& f1_arg2_int = boost::get<pa::integer_literal>(f1_arg2.var);
	EXPECT_EQ(f1_arg2_int.value, 2);

	ASSERT_TRUE(holds_alternative<pa::literal_expression>(f1.arguments[3].var));
	const auto& f1_arg3 = boost::get<pa::literal_expression>(f1.arguments[3].var);
	ASSERT_TRUE(holds_alternative<pa::integer_literal>(f1_arg3.var));
	const auto& f1_arg3_int = boost::get<pa::integer_literal>(f1_arg3.var);
	EXPECT_EQ(f1_arg3_int.value, 255);

	ASSERT_TRUE(holds_alternative<pa::identifier>(defs[2].value.var));
	const auto& iden = boost::get<pa::identifier>(defs[2].value.var);
	EXPECT_EQ(iden.value, "color_black");
}

TEST_F(parser_fixture, array_definition)
{
	const std::string input = minimal_input + "\n"
		"const currency_t1 = [\"Exalted Orb\", \"Mirror of Kalandra\", \"Eternal Orb\", \"Mirror Shard\"]";

	const std::optional<parser::parse_data> parse_result = parser::parse(input, buffer_log);
	ASSERT_TRUE(parse_result.has_value()) << "parse of:\n" << input << "\nfailed\n" << buffer_log.flush_out();
	namespace pa = parser::ast;
	const pa::ast_type& ast = (*parse_result).ast;

	const std::vector<pa::constant_definition>& defs = ast.constant_definitions;
	ASSERT_EQ(static_cast<int>(defs.size()), 1);

	EXPECT_EQ(defs[0].name.value, "currency_t1");
	ASSERT_TRUE(holds_alternative<pa::array_expression>(defs[0].value.var));
	const auto& array_expr = boost::get<pa::array_expression>(defs[0].value.var);
	ASSERT_EQ(static_cast<int>(array_expr.size()), 4);

	const std::vector<std::string> names = { "Exalted Orb", "Mirror of Kalandra", "Eternal Orb", "Mirror Shard" };
	ASSERT_EQ(array_expr.size(), names.size()); // sanity check

	for (int i = 0; i < static_cast<int>(array_expr.size()); ++i)
	{
		ASSERT_TRUE(holds_alternative<pa::literal_expression>(array_expr[i].var)) << "i = " << i;
		const auto& lit = boost::get<pa::literal_expression>(array_expr[i].var);
		ASSERT_TRUE(holds_alternative<pa::string_literal>(lit.var)) << "i = " << i;
		const auto& str = boost::get<pa::string_literal>(lit.var);
		EXPECT_EQ(str, names[i]) << "i = " << i;
	}
}

TEST_F(parser_fixture, nested_rule_definition)
{
	// note: parser does not evaluate identifier references
	// it's done later by the compiler which would error upon
	// this source but for the parser - AST is correct here
	const std::string input = minimal_input + R"(
# sample comment
SetBackgroundColor color_black

Class "Currency" {
	SetBorderColor color_currency

	BaseType currency_t1 {
		SetAlertSound sound_currency
		Show
	}

	Hide
}

Show
)";

	const std::optional<parser::parse_data> parse_result = parser::parse(input, buffer_log);
	ASSERT_TRUE(parse_result.has_value()) << "parse of:\n" << input << "\nfailed\n" << buffer_log.flush_out();
	namespace pa = parser::ast;
	const pa::ast_type& ast = (*parse_result).ast;

	const std::vector<pa::statement>& statements = ast.statements;
	ASSERT_EQ(static_cast<int>(statements.size()), 3);

	ASSERT_TRUE(holds_alternative<pa::action>(statements[0].var)); // SetBackgroundColor color_black
	const auto& st0_action = boost::get<pa::action>(statements[0].var);
	EXPECT_EQ(st0_action.action_type, lang::action_type::set_background_color);
	ASSERT_TRUE(holds_alternative<pa::identifier>(st0_action.value.var));
	const auto& st0_action_id = boost::get<pa::identifier>(st0_action.value.var);
	EXPECT_EQ(st0_action_id.value, "color_black");

	ASSERT_TRUE(holds_alternative<pa::rule_block>(statements[1].var)); // Class "Currency" {
	const auto& st1_rule_block = boost::get<pa::rule_block>(statements[1].var);
	ASSERT_EQ(static_cast<int>(st1_rule_block.conditions.size()), 1);
	const auto& st1_rule_block_cond = st1_rule_block.conditions[0];
	ASSERT_TRUE(holds_alternative<pa::string_condition>(st1_rule_block_cond.var));
	const auto& st1_rule_block_cond_str = boost::get<pa::string_condition>(st1_rule_block_cond.var);
	EXPECT_EQ(st1_rule_block_cond_str.property, lang::string_condition_property::class_);
	ASSERT_TRUE(holds_alternative<pa::literal_expression>(st1_rule_block_cond_str.value.var));
	const auto& st1_rule_block_cond_str_lit = boost::get<pa::literal_expression>(st1_rule_block_cond_str.value.var);
	ASSERT_TRUE(holds_alternative<pa::string_literal>(st1_rule_block_cond_str_lit.var));
	const auto& st1_rule_block_cond_str_lit_str = boost::get<pa::string_literal>(st1_rule_block_cond_str_lit.var);
	EXPECT_EQ(st1_rule_block_cond_str_lit_str, "Currency");

	ASSERT_EQ(static_cast<int>(st1_rule_block.statements.size()), 3);

	const auto& st1_st0 = st1_rule_block.statements[0]; // SetBorderColor color_currency
	ASSERT_TRUE(holds_alternative<pa::action>(st1_st0.var));
	const auto& st1_st0_action = boost::get<pa::action>(st1_st0.var);
	EXPECT_EQ(st1_st0_action.action_type, lang::action_type::set_border_color);
	ASSERT_TRUE(holds_alternative<pa::identifier>(st1_st0_action.value.var));
	const auto& st1_st0_action_id = boost::get<pa::identifier>(st1_st0_action.value.var);
	EXPECT_EQ(st1_st0_action_id.value, "color_currency");

	const auto& st1_st1 = st1_rule_block.statements[1]; // BaseType currency_t1
	ASSERT_TRUE(holds_alternative<pa::rule_block>(st1_st1.var));
	const auto& st1_st1_cond = boost::get<pa::rule_block>(st1_st1.var);
	ASSERT_EQ(static_cast<int>(st1_st1_cond.conditions.size()), 1);
	const auto& st1_st1_cond0 = st1_st1_cond.conditions[0];
	ASSERT_TRUE(holds_alternative<pa::string_condition>(st1_st1_cond0.var));
	const auto& st1_st1_cond0_str = boost::get<pa::string_condition>(st1_st1_cond0.var);
	EXPECT_EQ(st1_st1_cond0_str.property, lang::string_condition_property::base_type);
	ASSERT_TRUE(holds_alternative<pa::identifier>(st1_st1_cond0_str.value.var));
	const auto& st1_st1_cond0_str_lit = boost::get<pa::identifier>(st1_st1_cond0_str.value.var);
	EXPECT_EQ(st1_st1_cond0_str_lit.value, "currency_t1");

	ASSERT_EQ(static_cast<int>(st1_st1_cond.statements.size()), 2);

	const auto& st1_st1_cond_st0 = st1_st1_cond.statements[0]; // SetAlertSound sound_currency
	ASSERT_TRUE(holds_alternative<pa::action>(st1_st1_cond_st0.var));
	const auto& st1_st1_cond_st0_action = boost::get<pa::action>(st1_st1_cond_st0.var);
	EXPECT_EQ(st1_st1_cond_st0_action.action_type, lang::action_type::set_alert_sound);
	ASSERT_TRUE(holds_alternative<pa::identifier>(st1_st1_cond_st0_action.value.var));
	const auto& st1_st1_cond_st0_action_id = boost::get<pa::identifier>(st1_st1_cond_st0_action.value.var);
	EXPECT_EQ(st1_st1_cond_st0_action_id.value, "sound_currency");

	const auto& st1_st1_cond_st1 = st1_st1_cond.statements[1]; // Show
	ASSERT_TRUE(holds_alternative<pa::visibility_statement>(st1_st1_cond_st1.var));
	const auto& st1_st1_cond_st1_vs = boost::get<pa::visibility_statement>(st1_st1_cond_st1.var);
	EXPECT_EQ(st1_st1_cond_st1_vs.show, true);

	const auto& st1_st2 = st1_rule_block.statements[2]; // Hide
	ASSERT_TRUE(holds_alternative<pa::visibility_statement>(st1_st2.var));
	const auto& st1_st2_vs = boost::get<pa::visibility_statement>(st1_st2.var);
	EXPECT_EQ(st1_st2_vs.show, false);

	const auto& st2 = statements[2];
	ASSERT_TRUE(holds_alternative<pa::visibility_statement>(st2.var));
	const auto& st2_vs = boost::get<pa::visibility_statement>(st2.var);
	EXPECT_EQ(st2_vs.show, true);
}

} // namespace

