# sendspin

`sendspind` is the HiFiBerryOS **Sendspin player** — a C++ daemon that lets
[Music Assistant](https://music-assistant.io/) (or any Sendspin server) stream
audio to a HiFiBerry device over its native **Sendspin** multi-room protocol.
Unlike AirPlay on HiFiBerryOS, Sendspin carries full track metadata (title,
artist, album, cover art, progress) reliably, and supports **bidirectional
transport and volume control**.

It wraps the [`sendspin-cpp`](https://github.com/Sendspin/sendspin-cpp) library
(the same core that runs on ESP32) and adds the glue needed to behave like a
first-class HiFiBerryOS player: an ALSA sink, ALSA-mixer volume, a metadata push
to [audiocontrol](https://github.com/hifiberry/acr), a transport-command
endpoint, and mDNS advertisement.

## What it does

- **Renders audio.** Advertises itself over mDNS as a Sendspin player; Music
  Assistant discovers it and streams FLAC/Opus/PCM, which the daemon decodes and
  writes to the sound card.
- **Reports now-playing.** Pushes track metadata + play/pause/position/cover-art
  to audiocontrol's generic-player API, so the HiFiBerry web UI shows what's
  playing.
- **Accepts transport commands.** The HiFiBerry UI's play/pause/next/previous
  buttons reach the daemon and are forwarded to Music Assistant.
- **Syncs volume both ways.** Volume set in Music Assistant moves the HiFiBerry
  hardware volume, and local volume changes are reported back.

## Architecture

The daemon is a thin composition of small, independently-testable units:

| Unit | Responsibility |
|------|----------------|
| `options` | Command-line parsing → `Options` |
| `alsa_sink` | Writes decoded PCM to the ALSA `default` device |
| `volume_control` | ALSA `snd_mixer` get/set/monitor + mute |
| `volume_scale` | Sendspin 0–255 ↔ ALSA mixer range (pure) |
| `acr_reporter` | Non-blocking libcurl POST queue to audiocontrol |
| `report_json` / `metadata_map` | Build the audiocontrol update JSON (pure) |
| `command_server` | Minimal HTTP listener on `127.0.0.1:3547` |
| `http_request` / `command_map` | Parse the incoming command request (pure) |
| `mdns` | `_sendspin._tcp` advertisement via `dns_sd` |
| `listeners` / `main` | Wire the sendspin-cpp roles + the units + main loop |

### Data flows

- **Audio (MA → speaker):** `on_stream_start` configures the ALSA `default` PCM
  from the negotiated stream params; `on_audio_write` writes decoded PCM directly
  (push model).
- **Metadata (speaker → audiocontrol → UI):** `on_metadata` → `POST
  /api/player/sendspin/update` with `song_changed` (artwork under the
  `artwork_url` key, which audiocontrol maps to `cover_art_url`); state and
  position are posted from the main loop.
- **Commands (UI → MA):** audiocontrol POSTs `{"command":"play|pause|stop|next|
  previous"}` to `http://127.0.0.1:3547/command`; the daemon enqueues it and the
  **main loop** dispatches it to the Sendspin controller role (Sendspin has no
  seek command, so seek is not exposed).
- **Volume (both ways):** MA volume → the ALSA mixer control; a once-per-second
  poll reports local mixer changes back to MA (set-if-changed to avoid feedback
  loops). audiocontrol already monitors the same hardware control, so the UI
  tracks it automatically.

## Audio & volume model (HiFiBerryOS convention)

- **Output goes to the ALSA `default` device**, never a `hw:` device — on
  HiFiBerryOS `default` routes through PipeWire for mixing.
- **Volume rides the ALSA mixer control discovered by `config-soundcard`** on the
  card's `hw:<index>` — the same physical control audiocontrol's global volume
  drives (e.g. `Digital` on a DAC+/Amp2). Discovery happens in the
  `start-sendspin` wrapper, not in the binary.

## Command-line flags

```
sendspind
  --alsa-device   default                                  (PCM output device)
  --mixer-control <name>                                   (ALSA mixer control; empty = no volume)
  --mixer-device  hw:0                                     (mixer card)
  --command-port  3547                                     (transport-command HTTP port)
  --acr-url       http://localhost:1080/api/player/sendspin/update
  --name          <pretty hostname>                        (advertised Sendspin name)
  --sendspin-port 8928
  --version
```

In production these are filled in by `/usr/bin/start-sendspin`, which discovers
the card, mixer control, and pretty hostname and then `exec`s the binary.

## HiFiBerryOS integration

Packaged as `hifiberry-sendspin`, a **plugin-type** deb structured like
`shairport-sync`. It installs:

- `/usr/bin/sendspind` and `/usr/bin/start-sendspin`
- `/usr/lib/systemd/user/sendspin.service` — a **user** service (runs in the
  HiFiBerry user session, `After=pipewire.service`), toggled on via the web UI
- `/etc/hifiberry/players.d/sendspin.json` — web-UI player registry ("Music
  Assistant")
- `/etc/configserver/conf.d/sendspin.json` — grants the web UI control of the
  service
- `/etc/audiocontrol/players.d/sendspin.json` — registers the audiocontrol
  generic player with `command_url` `http://127.0.0.1:3547/command`

The transport path requires audiocontrol **≥ 0.7.15** (the release that adds the
generic player's outbound `command_url`). Cover art requires **≥ 0.7.14** (the
`artwork_url` alias).

## Building

Requires a C++17 toolchain, CMake, and the dev packages `libasound2-dev`,
`libcurl4-openssl-dev`, `nlohmann-json3-dev`, `libavahi-compat-libdnssd-dev`.
`sendspin-cpp` and its dependencies (ArduinoJson, micro-flac, micro-opus,
IXWebSocket) are fetched by CMake `FetchContent` (pinned).

```sh
# Build + run the unit tests
cmake -S . -B build
cmake --build build -j"$(nproc)"
ctest --test-dir build --output-on-failure

# Build the Debian package
./build.sh                 # wraps dpkg-buildpackage -us -uc -b
```

The daemon is built for arm64 (Raspberry Pi). The pure units (`options`,
`volume_scale`, `report_json`, `command_map`, `http_request`, `metadata_map`,
`alsa_format_for_bits`) are covered by CTest unit tests; the ALSA/mDNS/network
units are compile-verified and exercised on-device.

## Runtime dependencies

`libasound2`, `libcurl4`, `libavahi-compat-libdnssd1`, `hifiberry-configurator`
(for `config-soundcard` discovery). Avahi provides the mDNS backend; PipeWire
provides the `default` ALSA device.

## Related

- [`sendspin-cpp`](https://github.com/Sendspin/sendspin-cpp) — the protocol
  library this daemon wraps.
- [`hifiberry/acr`](https://github.com/hifiberry/acr) — audiocontrol, which
  renders the metadata and forwards transport commands here.
