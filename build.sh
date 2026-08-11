#!/bin/bash
set -e
SCRIPT_DIR="$(dirname "$(realpath "$0")")"
cd "$SCRIPT_DIR"

# Check for DIST environment variable
if [ -n "$DIST" ]; then
    echo "Using distribution from DIST environment variable: $DIST"
    CHROOT="${DIST}-amd64-sbuild"
    DIST_ARG="--dist=$DIST"
    CHROOT_ARG="--chroot=$CHROOT"
else
    echo "No DIST environment variable set, using sbuild default"
    DIST_ARG=""
    CHROOT_ARG=""
fi

echo "Building hifiberry-sendspin..."
sbuild \
    --chroot-mode=unshare \
    --no-clean-source \
    --enable-network \
    $DIST_ARG \
    $CHROOT_ARG \
    --verbose
    
echo "Built packages:"
ls -lh ../hifiberry-sendspin_*.deb
