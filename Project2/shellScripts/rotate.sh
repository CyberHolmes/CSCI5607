#! /usr/bin/bash

#-----------------------------------------------------------------------------
#rotate+sampling
CurProc=rotate
echo Processing ${CurProc}...
OUTDIR=$OUTROOT/$NAME/${CurProc}
if [ ! -d $OUTDIR ];
then
mkdir -p $OUTDIR;
fi
SampleOptions=("IMAGE_SAMPLING_POINT" "IMAGE_SAMPLING_BILINEAR" "IMAGE_SAMPLING_GAUSSIAN")

for angle in `seq -200 45 200`
do
    for option in `seq 0 1 2` #${SampleOptions[@]}
    do
        ./image -input ${INNAME} -sampling ${option} -${CurProc} ${angle} -output ${OUTDIR}/${NAME}_${CurProc}_${angle}_${option}.${EXT}
    done
done
