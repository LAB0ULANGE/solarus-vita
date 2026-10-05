string(TIMESTAMP PROJECT_BUILD_YEAR "%Y")
set(SOLARUSLAUNCHER_DISPLAY_NAME "Solarus Launcher")
set(SOLARUSLAUNCHER_EXECUTABLE_NAME "${PROJECT_NAME}")
set(SOLARUSLAUNCHER_COPYRIGHT "© 2006-${PROJECT_BUILD_YEAR} Solarus Labs.")
set(SOLARUSLAUNCHER_APP_IDENTIFIER "org.solarus-games.solarus-launcher")
set(SOLARUSLAUNCHER_ORGANISATION "Solarus Labs")
set(SOLARUSLAUNCHER_ORGANISATION_DOMAIN "solarus-games.org")
set(SOLARUSLAUNCHER_LICENSE "GPL-3.0 and CC-BY-SA-4.0") # Should be SPDX identifiers.
set(SOLARUSLAUNCHER_TRADEMARKS "Licensed under ${SOLARUSLAUNCHER_LICENSE}")
set(SOLARUSLAUNCHER_EMAIL "solarus-labs@solarus-games.org")
set(SOLARUSLAUNCHER_LINKS_X "https://x.com/solarusgames")
set(SOLARUSLAUNCHER_LINKS_MASTODON "https://mastodon.gamedev.place/@solarus")
set(SOLARUSLAUNCHER_LINKS_YOUTUBE "https://www.youtube.com/c/ChristophoZS")
set(SOLARUSLAUNCHER_LINKS_DISCORD "https://discord.gg/yYHjJHt")
set(SOLARUSLAUNCHER_LINKS_SOURCE_CODE "https://www.gitlab.com/solarus-games/solarus")
set(SOLARUSLAUNCHER_LINKS_BUGTRACKER "https://www.gitlab.com/solarus-games/solarus-launcher/-/issues")
set(SOLARUSLAUNCHER_LINKS_CONTACT "https://www.solarus-games.org/about/contact")
set(SOLARUSLAUNCHER_LINKS_RELEASES "https://www.solarus-games.org/download/#solarus-launcher")
set(SOLARUSLAUNCHER_LINKS_HELP "https://docs.solarus-games.org")
set(SOLARUSLAUNCHER_LINKS_DONATION "https://www.solarus-games.org/about/donate/")
set(SOLARUSLAUNCHER_UPDATE_ENDPOINT "https://gitlab.com/api/v4/projects/solarus-games%2Fsolarus/releases/permalink/latest")
set(SOLARUSLAUNCHER_QM_PREFIX "${PROJECT_NAME}")

set(SOLARUSLAUNCHER_DOCUMENT_NAME "Solarus Quest")
set(SOLARUSLAUNCHER_DOCUMENT_EXTENSION "solarus")
set(SOLARUSLAUNCHER_DOCUMENT_IDENTIFIER "org.solarus-games.solarus-quest")

# Generate a config header file with all the definitions above.
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/common/SolarusLauncherConfig.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/sources/SolarusLauncherConfig.h"
  @ONLY
)
