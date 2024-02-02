#!/usr/bin/env bash

set -eo pipefail

EXPO_UPDATES_PACKAGE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd -P)"

DEST="$CONFIGURATION_BUILD_DIR"
RESOURCE_BUNDLE_NAME="EXUpdates.bundle"

# For classic main project build phases integration, will be no-op to prevent duplicated fingerprint creation.
#
# `$PROJECT_DIR` is passed by Xcode as the directory to the xcodeproj file.
# in classic main project setup it is something like /path/to/app/ios
# in new style pod project setup it is something like /path/to/app/ios/Pods
PROJECT_DIR_BASENAME=$(basename $PROJECT_DIR)
if [ "x$PROJECT_DIR_BASENAME" != "xPods" ]; then
  exit 0
fi

# If PROJECT_ROOT is not specified, fallback to use Xcode PROJECT_DIR
PROJECT_ROOT=${PROJECT_ROOT:-"$PROJECT_DIR/../.."}
PROJECT_ROOT=${PROJECT_ROOT:-"$EXPO_UPDATES_PACKAGE_DIR/../.."}

cd "$PROJECT_ROOT" || exit
# We should get the physical path (/var/folders -> /private/var/folders) for metro to resolve correct files
PROJECT_ROOT="$(pwd -P)"

"${EXPO_UPDATES_PACKAGE_DIR}/scripts/with-node.sh" "${EXPO_UPDATES_PACKAGE_DIR}/scripts/createFingerprint.js" ios "$PROJECT_ROOT" "$DEST/$RESOURCE_BUNDLE_NAME"
