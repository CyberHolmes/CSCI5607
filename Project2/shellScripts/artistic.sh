#! /usr/bin/bash

#-----------------------------------------------------------------------------
CurProc=artistic
echo Processing ${CurProc}...
OUTDIR=$OUTROOT/$NAME/${CurProc}
if [ ! -d $OUTDIR ];
then
mkdir -p $OUTDIR;
fi

./image -input ${INNAME} -edgeDetectSobel -saturation 0 -contrast -1 -output ${OUTDIR}/${NAME}_Sketch.${EXT}
./image -input ${INNAME} -quantize 2 -blur 5 -brightness 1  -output ${OUTDIR}/${NAME}_Painting1.${EXT}
./image -input ${INNAME} -noise 0.2 -quantize 1 -blur 5 -output ${OUTDIR}/${NAME}_Painting2.${EXT}
./image -input ${INNAME} -charcoalPaint -output ${OUTDIR}/${NAME}_CharcoalPaint.${EXT}