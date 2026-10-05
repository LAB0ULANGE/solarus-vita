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

#include "solarus/core/FlagSet.h"

namespace Solarus {

/**
 * \brief Constructs a FlagSet from an enum value.
 * \param flag The enum flag to initialize the FlagSet with.
 */
template <typename E, typename Enable>
FlagSet<E, Enable>::FlagSet() = default;

/**
 * \brief Constructs a FlagSet from an enum value.
 * \param flag The enum flag to initialize the FlagSet with.
 */
template <typename E, typename Enable>
FlagSet<E, Enable>::FlagSet(E flag)
    : flags(static_cast<underlying_type>(flag)) {}

/**
 * \brief Constructs a FlagSet from an enum underlying type value.
 * \param flag The value to initialize the FlagSet with.
 */
template <typename E, typename Enable>
FlagSet<E, Enable>::FlagSet(underlying_type value) : flags(value) {}

/**
 * \brief Bitwise OR operator for combining flags.
 *
 * \param flag The flag to combine with the current FlagSet.
 * \return A new FlagSet containing the combined flags.
 */
template <typename E, typename Enable>
FlagSet<E, Enable> FlagSet<E, Enable>::operator|(E flag) const {
  return FlagSet(flags | static_cast<underlying_type>(flag));
}

/**
 * \brief Bitwise AND operator for checking flag intersection.
 *
 * \param flag The flag to check for intersection.
 * \return A new FlagSet containing the intersection of flags.
 */
template <typename E, typename Enable>
FlagSet<E, Enable> FlagSet<E, Enable>::operator&(E flag) const {
  return FlagSet(flags & static_cast<underlying_type>(flag));
}

/**
 * \brief Bitwise OR-assignment operator for adding flags.
 *
 * \param flag The flag to add to the current FlagSet.
 * \return A reference to the modified FlagSet.
 */
template <typename E, typename Enable>
FlagSet<E, Enable> &FlagSet<E, Enable>::operator|=(E flag) {
  flags |= static_cast<underlying_type>(flag);
  return *this;
}

/**
 * \brief Checks if a specific flag is set.
 *
 * \param flag The flag to check.
 * \return True if the flag is set, false otherwise.
 */
template <typename E, typename Enable>
bool FlagSet<E, Enable>::has_flag(E flag) const {
  return (flags & static_cast<underlying_type>(flag)) != 0;
}

/**
 * \brief Set or unset a specific flag.
 *
 * \param flag The flag to set or unset.
 * \param set True to set the flag, false otherwise.
 */
template <typename E, typename Enable>
void FlagSet<E, Enable>::set_flag(E flag, bool set) {
  if (set) {
    flags |= static_cast<underlying_type>(flag);
  } else {
    flags &= ~static_cast<underlying_type>(flag);
  }
}

/**
 * \brief Conversion to the enum underlying type.
 *
 * \return The combined flags as the underlying type.
 */
template <typename E, typename Enable>
typename FlagSet<E, Enable>::underlying_type FlagSet<E, Enable>::value() const {
  return flags;
}

/**
 * \brief Stream insertion operator for FlagSet.
 *
 * \tparam E Enum type representing flags.
 * \param os Output stream.
 * \param flags The FlagSet to output.
 * \return The output stream.
 */
template <typename E, typename Enable>
std::ostream &operator<<(std::ostream &os, const FlagSet<E, Enable> &flags) {
  os << flags.value();
  return os;
}
} // namespace Solarus
