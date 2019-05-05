#pragma once

#include "fs/lang/types.hpp"
#include "fs/lang/type_traits.hpp"
#include "fs/utility/type_list.hpp"
#include "fs/utility/if_constexpr_workaround.hpp"
#include "fs/compiler/error.hpp"
#include "fs/compiler/detail/type_constructors.hpp"

#include <array>
#include <variant>
#include <type_traits>

namespace fs::compiler::detail
{

namespace impl
{

	template <typename T>
	std::variant<T, error::error_variant> attempt_to_promote(const lang::object& object, lang::traits::promotion_list<>)
	{
		return error::type_mismatch{
			lang::type_to_enum<T>(),
			lang::type_of_object(object.value),
			object.value_origin};
	}

	template <typename T, typename P, typename... Ps>
	std::variant<T, error::error_variant> attempt_to_promote(const lang::object& object, lang::traits::promotion_list<P, Ps...>)
	{
		if (std::holds_alternative<P>(object.value))
			return type_constructor<T>::call(
				object.value_origin,
				std::array<lang::position_tag, 1>{object.value_origin},
				std::get<P>(object.value));
		else
			return attempt_to_promote<T>(object, lang::traits::promotion_list<Ps...>{});
	}

	template <typename T>
	std::variant<T, error::error_variant> get_non_array_value_as(const lang::object& object)
	{
		if (std::holds_alternative<T>(object.value))
			return std::get<T>(object.value);
		else
			return attempt_to_promote<T>(object, typename lang::traits::type_traits<T>::allowed_promotions{});
	}

}

template <typename T>
std::variant<T, error::error_variant> get_value_as(const lang::object& object)
{
	if ECLIPSE_CONSTEXPR (std::is_same_v<T, lang::array_object>)
	{
		if (!object.is_array())
			return object.promote_to_array();

		return std::get<lang::array_object>(object.value);
	}
	else
	{
		return impl::get_non_array_value_as<T>(object);
	}
}

}
