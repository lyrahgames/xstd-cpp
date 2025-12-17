# Changelog

## [0.1.0]

_Initial release._

### Added

- Add version macros and `version` namespace in `version.hpp` header
- Add `string_from_file` function in `io.hpp` header
- Add `match` struct template in `match.hpp` header
- Add `matches` concept in `match.hpp` header
- Add `meta::type_tag` struct template in `meta/tag.hpp` header
- Add `meta::value_tag` struct template in `meta/tag.hpp` header
- Add `meta::as_value` meta function in `meta/tag.hpp` header
- Add `meta::as_type` meta function in `meta/tag.hpp` header
- Add `meta::as_signature` meta function in `meta/tag.hpp` header
- Add `meta::string` struct template in `meta/string.hpp` header
- Add `meta::type_list` struct template in `meta/type_list.hpp` header
- Add `meta::value_list` struct template in `meta/value_list.hpp` header
- Add `meta::index_list` struct template in `meta/index_list.hpp` header
- Add `meta::string_list` struct template in `meta/string_list.hpp` header
- Add `meta::name_list` struct template in `meta/name_list.hpp` header
- Add `meta::radix_tree` struct template in `meta/radix_tree.hpp` header
- Add `size` function template overload for `meta::type_list` and `meta::value_list` instances
- Add `empty` function template overload for `meta::type_list` and `meta::value_list` instances
- Add `element` function template overload for `meta::type_list` and `meta::value_list` instances
- Add `front` function template overload for `meta::type_list` and `meta::value_list` instances
- Add `back` function template overload for `meta::type_list` and `meta::value_list` instances
- Add `all_of` function template overload for `meta::type_list` and `meta::value_list` instances
- Add `any_of` function template overload for `meta::type_list` and `meta::value_list` instances
- Add `none_of` function template overload for `meta::type_list` and `meta::value_list` instances
- Add `contained` function template overload for `meta::type_list` and `meta::value_list` instances
- Add `element_wise_unique` function template overload for `meta::type_list` and `meta::value_list` instances
- Add `concat` function template overload for `meta::type_list` and `meta::value_list` instances
- Add `operator+` function template overload for `meta::type_list` and `meta::value_list` instances
- Add `push_front` function template overload for `meta::type_list` and `meta::value_list` instances
- Add `push_back` function template overload for `meta::type_list` and `meta::value_list` instances
- Add `pop_front` function template overload for `meta::type_list` and `meta::value_list` instances
- Add `operator--` function template overload for `meta::type_list` and `meta::value_list` instances
- Add `pop_back` function template overload for `meta::type_list` and `meta::value_list` instances
- Add `operator--(int)` function template overload for `meta::type_list` and `meta::value_list` instances
- Add `reverse` function template overload for `meta::type_list` and `meta::value_list` instances
- Add `operator~` function template overload for `meta::type_list` and `meta::value_list` instances
- Add `trim_front` function template overload for `meta::type_list` and `meta::value_list` instances
- Add `trim_back` function template overload for `meta::type_list` and `meta::value_list` instances
- Add `range` function template overload for `meta::type_list` and `meta::value_list` instances
- Add `insert` function template overload based on index and predicate for `meta::type_list` and `meta::value_list` instances
- Add `remove` function template overload for `meta::type_list` and `meta::value_list` instances
- Add `remove_all` function template overload for `meta::type_list` and `meta::value_list` instances
- Add `swap` function template overload for `meta::type_list` and `meta::value_list` instances
- Add `merge` function template overload for `meta::type_list` and `meta::value_list` instances
- Add `sort` function template overload for `meta::type_list` and `meta::value_list` instances
- Add `transform` function template overload for `meta::type_list` and `meta::value_list` instances
- Add `fold` function template overload for `meta::type_list` and `meta::value_list` instances
- Add `for_each` function template overload for `meta::type_list` and `meta::value_list` instances
- Add `for_each_until` function template overload for `meta::type_list` and `meta::value_list` instances
- Add `index_list_from_iota` function template overload
- Add `bounded` function template overload for `meta::index_list` instances
- Add `radix_tree_from` function template overload
- Add `insert` function template overload for `meta::radix_tree` instances
- Add `visit` function template overload for `meta::radix_tree` instances
- Add `traverse` function template overload for `meta::radix_tree` instances
- Add `_xs` literal suffix for `meta::string`
- Add `view_from` function template overload for `meta::string`
- Add `operator+` function template overload for `meta::string`
- Add `substring` function template overload for `meta::string`
- Add `prefix` function template overload for `meta::string`
- Add `suffix` function template overload for `meta::string`
- Add `prefix_match_index` function template overload for `meta::string`
- Add `tail` function template overload for `meta::string`

<!-- [0.1.0]: https://github.com/lyrahgames/xstd-cpp/tags/0.1.0 -->
