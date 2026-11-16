#!/bin/bash

# Backward compatibility wrapper for release builds

LOCAL_scriptBasedir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)

exec bash "$LOCAL_scriptBasedir/build.sh" release
