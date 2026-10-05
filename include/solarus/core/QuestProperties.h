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

#ifndef SOLARUS_QUEST_PROPERTIES_H
#define SOLARUS_QUEST_PROPERTIES_H

#include "solarus/core/Common.h"
#include "solarus/core/Size.h"
#include "solarus/lua/LuaData.h"
#include "solarus/core/EnumInfo.h"
#include "solarus/core/FlagSet.h"

#include <iosfwd>
#include <string>
#include <vector>

struct lua_State;

namespace Solarus {

class MainLoop;

/**
 * \brief This class stores properties of a quest (the quest.dat file).
 *
 * These properties are general information about a quest.
 * They include the Solarus compatibility version,
 * the writing directory for savegames of this quest,
 * and some metadata describing the quest.
 */
class SOLARUS_API QuestProperties : public LuaData {

  public:

    QuestProperties();

    bool import_from_lua(lua_State* l) override;
    bool export_to_lua(std::ostream& out) const override;

    std::pair<int, int> get_solarus_version_major_minor() const;
    const std::string& get_solarus_version() const;
    void set_solarus_version(const std::string& solarus_version);

    const std::string& get_quest_write_dir() const;
    void set_quest_write_dir(const std::string& quest_write_dir);

    const std::string& get_title() const;
    void set_title(const std::string& title);

    const std::string& get_short_description() const;
    void set_short_description(const std::string& short_description);

    const std::string& get_long_description() const;
    void set_long_description(const std::string& long_description);

    const std::string& get_author() const;
    void set_author(const std::string& author);

    const std::string& get_quest_version() const;
    void set_quest_version(const std::string& quest_version);

    const std::string& get_initial_release_date() const;
    void set_initial_release_date(const std::string& initial_release_date);

    const std::string& get_release_date() const;
    void set_release_date(const std::string& release_date);

    const std::string& get_website() const;
    void set_website(const std::string& website);

    const Size& get_normal_quest_size() const;
    void set_normal_quest_size(const Size& normal_quest_size);

    const Size& get_min_quest_size() const;
    void set_min_quest_size(const Size& min_quest_size);

    const Size& get_max_quest_size() const;
    void set_max_quest_size(const Size& max_quest_size);

    const std::string& get_license() const;
    void set_license(const std::string& license);

    const std::vector<std::string>& get_languages() const;
    void set_languages(const std::vector<std::string>& languages);

    uint64_t get_min_players() const;
    void set_min_players(uint64_t min_players);

    uint64_t get_max_players() const;
    void set_max_players(uint64_t max_players);

    const std::vector<std::string>& get_genres() const;
    void set_genres(const std::vector<std::string>& genres);

    /**
     * \brief Age rating of the target audience (PEGI/ESRB-like).
     */
    enum class AgeRating {
      All,        /**< Suitable for all ages */
      Warning,    /**< Parental guidance is suggested */
      Restricted, /**< Exclusively for older audience */
    };

    AgeRating get_age() const;
    void set_age(AgeRating age);

    /**
     * \brief Control schemes supported by the quest.
     *
     * They may be combined, i.e. are not mutually exclusive.
     */
    enum class Control : unsigned int {
      None     = 0,       /**< The quests doesn't support any kind of controls. */
      Keyboard = 1 << 0,  /**< The quests supports keyboard controls. */
      Mouse    = 1 << 1,  /**< The quests supports mouse controls. */
      Joypad   = 1 << 2,  /**< The quests supports joypad controls. */
      Other    = 1 << 3,  /**< The quests supports other controls. */
    };

    const FlagSet<Control>& get_controls() const;
    void set_controls(const FlagSet<Control>& controls);

    bool is_dynamic_timestep() const;
    void set_dynamic_timestep(bool dynamic);

    bool is_subpixel_camera() const;
    void set_subpixel_camera(bool subpixel);

  private:

    std::string solarus_version;         /**< Version of Solarus the quest is
                                          * compatible with. */
    std::string quest_write_dir;         /**< Directory where to save files of
                                          * the quest, relative to
                                          * FileTools::get_base_write_dir(). */
    std::string title;                   /**< Title of the quest. */
    std::string short_description;       /**< One-line description of the
                                          * quest. */
    std::string long_description;        /**< Longer description of the
                                          * quest. */
    std::string author;                  /**< Who created the quest. */
    std::string quest_version;           /**< Version of the quest. */
    std::string release_date;            /**< Date of the last quest release
                                          * (YYYY-MM-DD). */
    std::string initial_release_date;    /**< Date of the first quest release
                                          * (YYYY-MM-DD). */
    std::string website;                 /**< URL of the quest website. */
    Size normal_quest_size;              /**< Default quest size. */
    Size min_quest_size;                 /**< Minimum quest size. */
    Size max_quest_size;                 /**< Maximum quest size. */
    std::string license;                 /**< License. */
    std::vector<std::string> languages;  /**< Languages the quest is translated
                                          * in. */
    uint64_t min_players = 1;            /**< Minimum quest player count. */
    uint64_t max_players = 1;            /**< Maximum quest player count. */
    std::vector<std::string> genres;     /**< Genres of the quest. */
    AgeRating age = AgeRating::All;      /**< Age rating of the target audience (PEGI/ESRB-like) */
    FlagSet<Control> controls;           /**< Supported control schemes by the quest. */

    bool use_dynamic_timestep = false;   /**< Quest requests engine in dynamic timestep */
    bool use_subpixel_camera = false;    /**< Quest requests engine to do subpixel camera movement */
};

template <>
struct SOLARUS_API EnumInfoTraits<QuestProperties::AgeRating> {
  static const std::string pretty_name;

  static const EnumInfo<QuestProperties::AgeRating>::names_type names;
};
}

#endif

