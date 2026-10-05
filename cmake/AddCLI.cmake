# Whether the user wants to build the Solarus CLI (solarus-run).
option(SOLARUS_CLI "Generate the Solarus command-line interface (CLI)" ON)

if(SOLARUS_CLI)
  add_subdirectory(cli)
endif()
