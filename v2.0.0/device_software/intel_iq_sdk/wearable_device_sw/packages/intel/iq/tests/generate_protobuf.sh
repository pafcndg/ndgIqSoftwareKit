#/bin/bash
BASEDIR=$(dirname $0)
T=$(readlink -m ${BASEDIR}/..)
PBUFDIR=${T}/protobuf/
for f in $(ls ${PBUFDIR}/*.proto)
do
	local_f=${BASEDIR}/$(basename ${f})
	# We do not need nanopb for python protobufs
	sed 's/import "nanopb.proto";//' ${f} > ${local_f}
	sed -i 's/\[(nanopb)\..*\]//' ${local_f}
done

for f in $(ls ${BASEDIR}/*.proto)
do
	protoc --python_out=${BASEDIR}/ \
		--proto_path=${BASEDIR}    \
		-I ${BASEDIR}               \
		${f}
done

rm -f ${BASEDIR}/*.proto
