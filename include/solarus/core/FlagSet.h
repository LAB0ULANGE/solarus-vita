/*
 * Copyright (C) 2006-2019 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Solarus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef FLAGSET_H
#define FLAGSET_H

#include <ostream>
#include <type_traits>

namespace Solarus {
/**
 * \brief Type-safe wrapper for a combination of enum flags.
 *
 * This class supports bitwise operations and flag checks for enum types.
 * It is enabled only if E is an enum type.
 *
 * \tparam E Enum type representing flags.
 */
template <typename E, typename = typename std::enable_if<std::is_enum<E>::value>::type>
class FlagSet {
public:
  /**
   * \brief Shorthand to the actual enum type.
   */
  using underlying_type = typename std::underlying_type<E>::type;

  FlagSet();
  FlagSet(E flag);
  FlagSet(underlying_type value);
  FlagSet operator|(E flag) const;
  FlagSet operator&(E flag) const;
  FlagSet &operator|=(E flag);
  bool has_flag(E flag) const;
  void set_flag(E flag, bool set);
  underlying_type value() const;

private:
  underlying_type flags{0}; /**< The bit flags. */
};

template <typename E, typename Enable = typename std::enable_if<std::is_enum<E>::value>::type>
std::ostream &operator<<(std::ostream &os, const FlagSet<E, Enable> &flags);
} // namespace Solarus

#include "FlagSet.inl"

#endif // FLAGSET_H
