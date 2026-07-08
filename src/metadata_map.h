#pragma once
#include "song.h"
#include <sendspin/metadata_role.h>
Song song_from_metadata(const sendspin::ServerMetadataStateObject& m);
