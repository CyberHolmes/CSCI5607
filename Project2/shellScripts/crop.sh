#! /usr/bin/bash

#-----------------------------------------------------------------------------
# # Crop
CurProc=crop
echo Processing ${CurProc}...
OUTDIR=$OUTROOT/$NAME/${CurProc}
if [ ! -d $OUTDIR ];
then
mkdir -p $OUTDIR;
fi
./image -input ${INNAME} -${CurProc} -1 -1 150 150 -output ${OUTDIR}/${NAME}_${CurProc}_-1_-1_150_150.${EXT}
./image -input ${INNAME} -${CurProc} 200 150 1000 1000 -output ${OUTDIR}/${NAME}_${CurProc}_200_150_1000_1000.${EXT}
./image -input ${INNAME} -${CurProc} 100 150 250 350 -output ${OUTDIR}/${NAME}_${CurProc}_100_150_250_350.${EXT}
