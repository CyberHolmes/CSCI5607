#! /usr/bin/bash

#-----------------------------------------------------------------------------
#contrast
CurProc=contrast
echo Processing ${CurProc}...
OUTDIR=$OUTROOT/$NAME/${CurProc}
if [ ! -d $OUTDIR ];
then
mkdir -p $OUTDIR;
fi
for i in `seq -2 0.2 2`
do
./image -input ${INNAME} -${CurProc} $i -output ${OUTDIR}/${NAME}_${CurProc}_${i}.${EXT}
done
