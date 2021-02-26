#! /usr/bin/bash

#-----------------------------------------------------------------------------
#mosaic
CurProc=mosaic
echo Processing ${CurProc}...
OUTDIR=$OUTROOT/$NAME/${CurProc}
if [ ! -d $OUTDIR ];
then
mkdir -p $OUTDIR;
fi
for i in `seq 5 5 20`
do
./image -input ${INNAME} -${CurProc} $i -output ${OUTDIR}/${NAME}_${CurProc}_${i}.${EXT}
done