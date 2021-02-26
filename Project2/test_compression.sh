#! /usr/bin/bash
INDIR=./compressionTestImgs
OUTROOT=${INDIR}/binaries
echo ------------------------------------
echo Available Images:
ls $INDIR
echo ------------------------------------
echo Enter the image file name including extension you would like to process:
read IMG
# echo You choose to process $IMG
NAME=${IMG%.*}
EXT=${IMG#*.}

INNAME=$INDIR/$IMG
OUTDIR=$OUTROOT/$NAME

if [ ! -d $OUTROOT ];
then
mkdir -p $OUTROOT;
fi

if [ ! -d $OUTDIR ];
then
mkdir -p $OUTDIR;
fi
for i in `seq 1 1 8`
do
./image -input ${INNAME} -FloydSteinbergDither $i -ppm_depth $i -output ${OUTDIR}/${NAME}_binary_${i}.ppm
./image -input ${OUTDIR}/${NAME}_binary_${i}.ppm -output ${OUTDIR}/${NAME}_binary_${i}_readback.png
done