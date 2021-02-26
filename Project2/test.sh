#! /usr/bin/bash

INDIR=./sampleimgs
OUTROOT=./out
echo ------------------------------------
echo Available Images:
ls $INDIR
echo ------------------------------------
echo Enter the image file name including extension you would like to process:
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

if [ ! -d $OUTROOT ];
then
mkdir -p $OUTROOT;
fi

if [ ! -d $OUTDIR ];
then
mkdir -p $OUTDIR;
fi

. ./shellScripts/brighten.sh
. ./shellScripts/extractChannel.sh
. ./shellScripts/crop.sh
. ./shellScripts/quantize.sh
. ./shellScripts/noise.sh
. ./shellScripts/noiseSaltPepper.sh
. ./shellScripts/contrast.sh
. ./shellScripts/saturation.sh
. ./shellScripts/sharpen.sh
. ./shellScripts/randomDither.sh
. ./shellScripts/blur.sh
. ./shellScripts/edgeDetect.sh
. ./shellScripts/edgeDetectSobel.sh
. ./shellScripts/fsDither.sh
. ./shellScripts/scale.sh
. ./shellScripts/rotate.sh
. ./shellScripts/orderedDither.sh
. ./shellScripts/mosaic.sh
. ./shellScripts/artistic.sh