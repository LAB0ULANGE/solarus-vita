#!/usr/bin/env bash
# Helper script for managing packages and releases in GitLab.
# script by gitlab.com/hhromic

set -Eeuo pipefail

declare -r PROJECT_ID=${PROJECT_ID:-solarus-games/solarus}
declare -r PACKAGE_NAME=${PACKAGE_NAME:-${PROJECT_ID##*/}}

declare -r GITLAB_API_V4=https://gitlab.com/api/v4
declare -r GITLAB_API_PROJECTS=$GITLAB_API_V4/projects
declare -r GITLAB_API_JOB_ARTIFACTS=$GITLAB_API_PROJECTS/%s/jobs/artifacts
declare -r GITLAB_API_PACKAGES=$GITLAB_API_PROJECTS/%s/packages
declare -r GITLAB_API_GENERIC_PACKAGES=$GITLAB_API_PACKAGES/generic
declare -r GITLAB_API_RELEASES=$GITLAB_API_PROJECTS/%s/releases
declare -r GITLAB_API_TAGS=$GITLAB_API_PROJECTS/%s/repository/tags

log() { echo "$@" >&2; }
die() { log "$@"; exit 1; }
show_usage() { cat >&2; exit 1; }

check_req_tools() {
  local _tool
  for _tool in jq curl mktemp unzip; do
    if ! command -v "$_tool" >/dev/null; then
      die "error: missing required tool for this script: $_tool"
    fi
  done
}

urlfmt() {
  local -r _format=$1
  local _arg _args
  for _arg in "${@:2}"; do  # percent-encode all arguments before formatting
    _args+=("$(jq --arg v "$_arg" -nr '$v | @uri')")
  done
  # shellcheck disable=SC2059
  printf "$_format" "${_args[@]}"
}

# https://github.com/semver/semver/issues/981
is_semver() {
  local -r _d='0|[1-9][0-9]*'  # semver digit
  local -r _pw='[0-9]*[a-zA-Z-][0-9a-zA-Z-]*'  # semver pre-release word
  local -r _mw='[0-9a-zA-Z-]+'  # semver build-metadata word
  [[ "$1" =~ ^($_d)\.($_d)\.($_d)(-(($_d|$_pw)(\.($_d|$_pw))*))?(\+($_mw(\.$_mw)*))?$ ]]
}

curl_gitlab() {
  if [[ -z ${GITLAB_TOKEN:-} ]]; then
    die "error: missing required environment variable: GITLAB_TOKEN"
  fi
  curl --silent --fail-with-body --location \
    --header "PRIVATE-TOKEN: $GITLAB_TOKEN" "$@"
}

# https://docs.gitlab.com/api/job_artifacts/#download-the-artifacts-archive
api::jobs::artifacts::download() {
  local -r _pid=$1
  local -r _ref_name=$2
  local -r _job=$3
  curl_gitlab --request GET \
    --data-urlencode job="$_job" \
    "$(urlfmt "$GITLAB_API_JOB_ARTIFACTS/%s/download" "$_pid" "$_ref_name")"
}

# https://docs.gitlab.com/api/packages/#list-packages
api::packages::list() {
  local -r _pid=$1
  local -r _name=$2
  local -r _version=$3
  curl_gitlab --request GET \
    --data-urlencode package_name="$_name" \
    --data-urlencode package_version="$_version" \
    --data-urlencode include_versionless=false \
    "$(urlfmt "$GITLAB_API_PACKAGES" "$_pid")"
}

# https://docs.gitlab.com/api/packages/#list-package-files
api::packages::list_package_files() {
  local -r _pid=$1
  local -r _id=$2
  curl_gitlab --request GET \
    "$(urlfmt "$GITLAB_API_PACKAGES/%s/package_files" "$_pid" "$_id")"
}

# https://docs.gitlab.com/user/packages/generic_packages/#publish-a-single-file
api::generic_packages::upload_file() {
  local -r _pid=$1
  local -r _name=$2
  local -r _version=$3
  local -r _file=$4
  local -r _file_name=$5
  curl_gitlab --request PUT \
    --upload-file "$_file" \
    "$(urlfmt "$GITLAB_API_GENERIC_PACKAGES/%s/%s/%s" "$_pid" "$_name" "$_version" "$_file_name")"
}

# https://docs.gitlab.com/api/releases/#create-a-release
api::releases::create() {
  local -r _pid=$1
  curl_gitlab --request POST \
    --header "Content-Type: application/json" \
    --data-binary @- \
    "$(urlfmt "$GITLAB_API_RELEASES" "$_pid")"
}

# https://docs.gitlab.com/api/tags/#get-a-single-repository-tag
api::tags::get() {
  local -r _pid=$1
  local -r _tag_name=$2
  curl_gitlab --request GET \
    "$(urlfmt "$GITLAB_API_TAGS/%s" "$_pid" "$_tag_name")"
}

releaser() {
  [[ $# -lt 1 ]] && show_usage << __EOF__
Usage: $0 COMMAND [ARGUMENT]...

Available commands:

    job        manage CI/CD pipeline job resources in GitLab
    package    manage package resources in GitLab
    release    manage release resources in GitLab

__EOF__

  case "$1" in
    job) job "${@:2}" ;;
    package) package "${@:2}" ;;
    release) release "${@:2}" ;;
    *) die "error: unknown command: $1" ;;
  esac
}

job() {
  [[ $# -lt 1 ]] && show_usage << __EOF__
Usage: $0 job COMMAND [ARGUMENT]...

Available commands:

    download-artifact    download the job artifact from a CI/CD pipeline job in GitLab

__EOF__

  case "$1" in
    download-artifact) job::download_artifact "${@:2}" ;;
    *) die "error: unknown job command: $1" ;;
  esac
}

job::download_artifact() {
  [[ $# -lt 3 ]] && show_usage << __EOF__
Usage: $0 job download-artifact REF_NAME JOB OUTPUT_DIR

Environment variables:

    PROJECT_ID    the project ID or project path to manage (default: $PROJECT_ID)

Important: REF_NAME must be exactly as it appears in GitLab, e.g. "v2.0.0" or "dev".
__EOF__

  local -r _ref_name=$1
  local -r _job=$2
  local -r _output_dir=$3

  local _tmp_dir
  _tmp_dir=$(mktemp -d)
  # shellcheck disable=SC2064
  trap "rm -rf \"$_tmp_dir\"" RETURN EXIT

  local -r _artifact_file=$_tmp_dir/artifact.zip
  if ! api::jobs::artifacts::download "$PROJECT_ID" "$_ref_name" "$_job" > "$_artifact_file"; then
    jq -c < "$_artifact_file" && return 1
  fi
  unzip -d "$_output_dir" "$_artifact_file"
}

package() {
  [[ $# -lt 1 ]] && show_usage << __EOF__
Usage: $0 package COMMAND [ARGUMENT]...

Available commands:

    upload           upload files for a generic package in GitLab
    gen-links        generate release assets links for a generic package in GitLab
    gen-checksums    generate file checksums for a generic package in GitLab

__EOF__

  case "$1" in
    upload) package::upload "${@:2}" ;;
    gen-links) package::gen_links "${@:2}" ;;
    gen-checksums) package::gen_checksums "${@:2}" ;;
    *) die "error: unknown package command: $1" ;;
  esac
}

package::upload() {
  [[ $# -lt 2 ]] && show_usage << __EOF__
Usage: $0 package upload VERSION FILE [FILE]...

Environment variables:

    PROJECT_ID      the project ID or project path to manage (default: $PROJECT_ID)
    PACKAGE_NAME    the generic package name to manage (default: $PACKAGE_NAME)

Important: VERSION must be in the form MAJOR.MINOR.PATCH (without any "v" prefix).
__EOF__

  local -r _version=$1
  if ! is_semver "$_version"; then
    die "error: version argument is not in valid semver format: $_version"
  fi

  local _file
  for _file in "${@:2}"; do
    log "uploading file for generic package $PACKAGE_NAME/$_version: $_file"
    api::generic_packages::upload_file \
      "$PROJECT_ID" "$PACKAGE_NAME" "$_version" "$_file" "${_file##*/}" | jq -c
  done
}

package::gen_links() {
  [[ $# -lt 1 ]] && show_usage << __EOF__
Usage: $0 package gen-links VERSION

Environment variables:

    PROJECT_ID      the project ID or project path to manage (default: $PROJECT_ID)
    PACKAGE_NAME    the generic package name to manage (default: $PACKAGE_NAME)

Important: VERSION must be in the form MAJOR.MINOR.PATCH (without any "v" prefix).
__EOF__

  local -r _version=$1
  if ! is_semver "$_version"; then
    die "error: version argument is not in valid semver format: $_version"
  fi

  local _response _id
  if ! _response=$(api::packages::list "$PROJECT_ID" "$PACKAGE_NAME" "$_version"); then
    jq -c <<< "$_response" && return 1
  fi
  _id=$(jq '.[].id' <<< "$_response")
  if [[ -z $_id ]]; then
    die "error: package $PACKAGE_NAME/$_version not found"
  fi

  local -r _base_url=$(urlfmt "$GITLAB_API_PACKAGES/generic/%s/%s" \
    "$PROJECT_ID" "$PACKAGE_NAME" "$_version")
  api::packages::list_package_files "$PROJECT_ID" "$_id" \
    | jq -c --arg base_url "$_base_url" \
        'map({name:.file_name, url:"\($base_url)/\(.file_name | @uri)", link_type:"package"})'
}

package::gen_checksums() {
  [[ $# -lt 1 ]] && show_usage << __EOF__
Usage: $0 package gen-checksums VERSION

Environment variables:

    PROJECT_ID      the project ID or project path to manage (default: $PROJECT_ID)
    PACKAGE_NAME    the generic package name to manage (default: $PACKAGE_NAME)

Important: VERSION must be in the form MAJOR.MINOR.PATCH (without any "v" prefix).
__EOF__

  local -r _version=$1
  if ! is_semver "$_version"; then
    die "error: version argument is not in valid semver format: $_version"
  fi

  local _response _id
  if ! _response=$(api::packages::list "$PROJECT_ID" "$PACKAGE_NAME" "$_version"); then
    jq -c <<< "$_response" && return 1
  fi
  _id=$(jq '.[].id' <<< "$_response")
  if [[ -z $_id ]]; then
    die "error: package $PACKAGE_NAME/$_version not found"
  fi

  api::packages::list_package_files "$PROJECT_ID" "$_id" \
    | jq -r '.[] | "\(.file_sha256)  \(.file_name)"'
}

release() {
  [[ $# -lt 1 ]] && show_usage << __EOF__
Usage: $0 release [ARGUMENT]...

    create    create a new release in GitLab

__EOF__

  case "$1" in
    create) release::create "${@:2}" ;;
    *) die "error: unknown release command: $1" ;;
  esac
}

release::create() {
  [[ $# -lt 2 ]] && show_usage << __EOF__
Usage: $0 release create VERSION NOTES_FILE [RELEASED_AT]

Environment variables:

    PROJECT_ID      the project ID or project path to manage (default: $PROJECT_ID)
    PACKAGE_NAME    the generic package name to manage (default: $PACKAGE_NAME)

Important: VERSION must be in the form MAJOR.MINOR.PATCH (without any "v" prefix).
__EOF__

  local -r _version=$1
  local -r _notes_file=$2
  local -r _released_at=${3:-}
  local -r _tag_name=v$_version
  if ! is_semver "$_version"; then
    die "error: version argument is not in valid semver format: $_version"
  fi
  if ! api::tags::get "$PROJECT_ID" "$_tag_name" >/dev/null; then
    die "error: tag not found in repository: $_tag_name"
  fi

  local _notes _links _body _response
  _notes=$(<"$_notes_file")$'\n' || return
  _links=$(package::gen_links "$_version") || return
  _body=$(jq -cn \
    --arg tag_name "$_tag_name" --arg description "$_notes" --arg released_at "$_released_at" \
    --argjson links "$_links" \
    '{tag_name:$tag_name,description:$description,assets:{links:$links},
      released_at:($released_at | if . != "" then . else null end)}')
  if ! _response=$(api::releases::create "$PROJECT_ID" <<< "$_body"); then
    jq -c <<< "$_response" && return 1
  fi
  jq -r '"release created: \(.name) (\(.commit.id))"' <<< "$_response"
}

check_req_tools
releaser "$@" || exit
