#!/bin/bash
PKG_DEST_jocstrap=$(PKG_DEST_ jocstrap) \
PKG_DEST_rhino=$(PKG_DEST_ rhino) \
    exec ant "$@"
