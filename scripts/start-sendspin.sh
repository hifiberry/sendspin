#!/bin/bash
# start-sendspin.sh — discovers the sound card volume control and pretty hostname,
# then launches sendspind. Runs from the sendspin systemd *user* service.
set -u

# Only run as root or the configured HiFiBerry user (mirrors start-shairport.sh)
CURRENT_USER=$(whoami)
if [ "$CURRENT_USER" != "root" ] && [ -f /etc/hifiberry.user ]; then
  AUTH_USER=$(tr -d '\n\r ' < /etc/hifiberry.user)
  if [ "$CURRENT_USER" != "$AUTH_USER" ]; then
    echo "Not starting sendspin: must run as $AUTH_USER"; exit 0
  fi
fi

# Sound card guard
if ! /usr/bin/config-soundcard --detect >/dev/null 2>&1; then
  echo "No sound card detected, not starting sendspin"; exit 0
fi

NAME=$(hostnamectl hostname --pretty 2>/dev/null)
[ -n "$NAME" ] || NAME=$(hostname 2>/dev/null)
[ -n "$NAME" ] || NAME="HiFiBerry"

MIXER=$(config-soundcard --no-eeprom --volume-control-softvol 2>/dev/null)
HWIDX=$(config-soundcard --no-eeprom --hw 2>/dev/null)
[ -n "$HWIDX" ] || HWIDX=0

exec /usr/bin/sendspind \
  --alsa-device default \
  --mixer-control "$MIXER" \
  --mixer-device "hw:$HWIDX" \
  --command-port 3547 \
  --acr-url http://localhost:1080/api/player/sendspin/update \
  --name "$NAME"
