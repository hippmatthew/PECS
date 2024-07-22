#!/bin/zsh

log()
{
  local INPUT=$1

  echo "* uninstall: ${INPUT}"
}

INCL_DIR=/usr/local/include/vecs
LIB_DIR=/usr/local/lib

log "uninstalling vecs library"

if [[ ! "$(id -u)" == "0" ]]
then
	log "uninstall script needs to be run as root"
	exit 1
fi

rm -r $INCL_DIR
log "uninstalled headers"

rm -f $LIB_DIR/libvecs.a
log "uninstalled library"

log "done."