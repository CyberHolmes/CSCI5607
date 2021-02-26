#! /usr/bin/bash

#-----------------------------------------------------------------------------
#OrderedDither
CurProc=orderedDither
echo Processing ${CurProc}...
OUTDIR=$OUTROOT/$NAME/${CurProc}
if [ ! -d $OUTDIR ];
then
mkdir -p $OUTDIR;
fi
for i in `seq 1 1 8`
do
./image -input ${INNAME} -${CurProc} $i -output ${OUTDIR}/${NAME}_${CurProc}_${i}.${EXT}
done
