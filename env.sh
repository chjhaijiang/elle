#!/bin/sh

INFINIT_SOURCE_DIR=$(git rev-parse --show-toplevel)
INFINIT_BUILD_DIR=$PWD

ELLE_LOG_COMPONENTS="infinit.*"
ELLE_LOG_COMPONENTS="${ELLE_LOG_COMPONENTS},reactor.network.*"
ELLE_LOG_COMPONENTS="${ELLE_LOG_COMPONENTS},elle.HTTPClient"

DISTCC_HOSTS="infinit.im"

infinit_env_init() {
	export INFINIT_BUILD_DIR=${INFINIT_BUILD_DIR}
	export INFINIT_SOURCE_DIR=${INFINIT_SOURCE_DIR}
	export ELLE_LOG_COMPONENTS="${ELLE_LOG_COMPONENTS}"
	export ELLE_LOG_LEVEL=DEBUG
	export DISTCC_HOSTS="${DISTCC_HOSTS}"

	export OLD_PYTHONPATH=$PYTHONPATH
	export OLD_RPROMPT=$RPROMPT

	export PYTHONPATH=${INFINIT_BUILD_DIR}/lib/python:$PYTHONPATH
	export RPROMPT="infinit env ${RPROMPT}"
}

infinit_env_clean() {
	unset INFINIT_BUILD_DIR
	unset INFINIT_SOURCE_DIR
	unset ELLE_LOG_COMPONENTS
	unset ELLE_LOG_LEVEL
	unset DISTCC_HOSTS

	export PYTHONPATH=${OLD_PYTHONPATH}
	export RPROMPT=${OLD_RPROMPT}
}

infinit_env_init

. ${INFINIT_SOURCE_DIR}/scripts/framework.sh

