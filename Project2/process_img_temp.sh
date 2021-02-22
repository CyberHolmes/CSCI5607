#! /usr/bin/bash

INDIR=./sampleimgs
OUTROOT=./out
echo ------------------------------------
echo Available Images:
ls $INDIR
echo ------------------------------------
echo Enter the image name you would like to process:
read IMG
# echo You choose to process $IMG
NAME=${IMG%.*}
EXT=${IMG#*.}
#read -d.-t NAMESTR <<< "$IMG"
# echo File name = ${NAME}
# echo File extension = ${EXT}
INNAME=$INDIR/$IMG
# echo INNAME=$INNAME
OUTDIR=$OUTROOT/$NAME
# echo $OUTDIR


if [ ! -d $OUTDIR ];
then
mkdir -p $OUTDIR;
fi

#edgeDetect
CurProc=edgeDetect
echo Processing ${CurProc}...
OUTDIR=$OUTROOT/$NAME/${CurProc}
if [ ! -d $OUTDIR ];
then
mkdir -p $OUTDIR;
fi
./image -input ${INNAME} -${CurProc} -output ${OUTDIR}/${NAME}_${CurProc}.${EXT}
