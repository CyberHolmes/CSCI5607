#! /usr/bin/bash

#-----------------------------------------------------------------------------
#edgeDetectSobel
CurProc=edgeDetectSobel
echo Processing ${CurProc}...
OUTDIR=$OUTROOT/$NAME/${CurProc}
if [ ! -d $OUTDIR ];
then
mkdir -p $OUTDIR;
fi
./image -input ${INNAME} -${CurProc} -output ${OUTDIR}/${NAME}_${CurProc}.${EXT}
