/*
 * Copyright (C) 2021-2021 Std::Gregwar, Solarus - http://www.solarus-games.org
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
#pragma once

#include <map>
#include <optional>
#include <memory>
#include <vector>

namespace Solarus {

/**
 * @brief Class that extends behavior of a Map of vector
 *
 * Not feature complete, created to help the management of a vector of map for Solarus::Controls bindings
 */
template<class Key, class T, class Compare=std::less<Key>, class Allocator=std::allocator<std::pair<const Key, std::vector<T>>>>
class VecMap {
  public:

    using Vec = std::vector<T>;
    using Wrapped = std::map<Key, Vec, Compare, Allocator>;
    using Map = Wrapped;

    VecMap() = default;
    VecMap(std::initializer_list<typename Wrapped::value_type> list) : map(std::forward(list)) {}

    // Do brace access that looks like map<K,T> considering only first slot of the vectors
    const T& operator[](const Key& k) const {
      auto& vec = map[k];

      if (vec.size() == 0) {
        vec.push_back({}); // Default construct the T
      }

      return vec.front();
    }

    // Do brace access that looks like map<K,T> considering only first slot of the vectors
    T& operator[](const Key& k) {
      auto& vec = map[k];

      if (vec.size() == 0) {
        vec.push_back({}); // Default construct the T
      }

      return vec.front();
    }

    void push_back(const Key& k, const T& v) {
      map[k].push_back(v);
    }

    void erase(const Key& k) {
      map.erase(k);
    }

    // Get the underlying map of vec
    const auto& underlying() const {
        return map;
    }

    auto set_underlying(const Map& map) {
      this->map = map;
    }

    // Actual way to access underlying vectors
    const auto& vec_for(const Key& k) const {
      static Vec empty;
      auto it = map.find(k);
      if (it == map.end()){
        return empty;
      }
      return it->second;
    }

    void set_vec_for(const Key& k, const Vec& v) {
      map.insert_or_assign(k, v);
    }

    std::optional<Key> search_fronts_for(const T& v) const {
      for (const auto& kvp : map) {
        if (kvp.second.size() && kvp.second.front() == v) {
          return kvp.first;
        }
      }
      return {};
    }

    std::optional<Key> search_all_for(const T& v) const {
      for (const auto& kvp : map) {
        for (const auto& av : kvp.second) {
          if (av == v) {
            return kvp.first;
          }
        }
      }
      return {};
    }

    std::optional<T> find_front(const Key& k) const {
      const auto& it = map.find(k);
      if (it != map.end() && it->second.size()) {
        return it->second.front();
      }
      return {};
    }

    template<class F>
    void for_each_front(F&& f) const {
      for (const auto& kvp : map) {
        if (kvp.second.size()) {
          f(kvp.first, kvp.second.front());
        }
      }
    }

    template<class F>
    void for_each(F&& f) const {
      for (const auto& [k, vec] : map) {
        for (const auto& v : vec) {
          f(k, v);
        }
      }
    }

  private:
    mutable Wrapped map;
};

}
