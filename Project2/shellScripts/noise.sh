#! /usr/bin/bash

#-----------------------------------------------------------------------------
# noise
CurProc=noise
echo Processing ${CurProc}...
OUTDIR=$OUTROOT/$NAME/${CurProc}
if [ ! -d $OUTDIR ];
then
mkdir -p $OUTDIR;
fi
for i in `seq 0.1 0.1 1.0`
do
./image -input ${INNAME} -${CurProc} $i -output ${OUTDIR}/${NAME}_${CurProc}_${i}.${EXT}
done
