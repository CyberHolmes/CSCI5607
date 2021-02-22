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
#-----------------------------------------------------------------------------
# Blur
CurProc=blur
echo Processing ${CurProc}...
OUTDIR=$OUTROOT/$NAME/${CurProc}
if [ ! -d $OUTDIR ];
then
mkdir -p $OUTDIR;
fi
for i in `seq 1 2 15`
do
./image -input ${INNAME} -${CurProc} $i -output ${OUTDIR}/${NAME}_${CurProc}_${i}.${EXT}
done
#-----------------------------------------------------------------------------
# # Brightness
CurProc=brightness
echo Processing ${CurProc}...
OUTDIR=$OUTROOT/$NAME/${CurProc}
if [ ! -d $OUTDIR ];
then
mkdir -p $OUTDIR;
fi
for i in `seq 0.2 0.5 2.5`
do
./image -input ${INNAME} -${CurProc} $i -output ${OUTDIR}/${NAME}_${CurProc}_${i}.${EXT}
done
#-----------------------------------------------------------------------------
# # Crop
CurProc=crop
echo Processing ${curProc}...
OUTDIR=$OUTROOT/$NAME/${CurProc}
if [ ! -d $OUTDIR ];
then
mkdir -p $OUTDIR;
fi
./image -input ${INNAME} -${CurProc} -1 -1 150 150 -output ${OUTDIR}/${NAME}_${CurProc}_-1_-1_150_150.${EXT}
./image -input ${INNAME} -${CurProc} 200 150 1000 1000 -output ${OUTDIR}/${NAME}_${CurProc}_200_150_1000_1000.${EXT}
./image -input ${INNAME} -${CurProc} 100 150 250 350 -output ${OUTDIR}/${NAME}_${CurProc}_100_150_250_350.${EXT}
# #-----------------------------------------------------------------------------
# # Extract Channel

CurProc=extractChannel
echo Processing ${CurProc}...
OUTDIR=$OUTROOT/$NAME/${CurProc}
if [ ! -d $OUTDIR ];
then
mkdir -p $OUTDIR;
fi
for i in {0..2}
do
./image -input ${INNAME} -${CurProc} $i -output ${OUTDIR}/${NAME}_${CurProc}_${i}.${EXT}
done

#-----------------------------------------------------------------------------
#Quantize
CurProc=quantize
echo Processing ${CurProc}...
OUTDIR=$OUTROOT/$NAME/${CurProc}
if [ ! -d $OUTDIR ];
then
mkdir -p $OUTDIR;
fi
for i in {1..8}
do
./image -input ${INNAME} -${CurProc} $i -output ${OUTDIR}/${NAME}_${CurProc}_${i}.${EXT}
done
#-----------------------------------------------------------------------------
# # ppm_depth
CurProc=ppm_depth
echo Processing ${CurProc}...
OUTDIR=$OUTROOT/$NAME/${CurProc}
if [ ! -d $OUTDIR ];
then
mkdir -p $OUTDIR;
fi
for i in {1..8}
do
./image -input ${INNAME} -${CurProc} $i -output ${OUTDIR}/${NAME}_${CurProc}_${i}.${EXT}
done
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
#-----------------------------------------------------------------------------
#saturation
CurProc=saturation
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
#-----------------------------------------------------------------------------
#randomDither
CurProc=randomDither
echo Processing ${CurProc}...
OUTDIR=$OUTROOT/$NAME/${CurProc}
if [ ! -d $OUTDIR ];
then
mkdir -p $OUTDIR;
fi
for i in `seq 1 1 8`
do
./image -input ${INNAME} -${CurProc} $i -output ${OUTDIR}/${NAME}_${CurProc}_${i}.${EXT}
done
#-----------------------------------------------------------------------------
#OrderedDither
CurProc=orderedDither
echo Processing ${CurProc}...
OUTDIR=$OUTROOT/$NAME/${CurProc}
if [ ! -d $OUTDIR ];
then
mkdir -p $OUTDIR;
fi
for i in `seq 1 1 8`
do
./image -input ${INNAME} -${CurProc} $i -output ${OUTDIR}/${NAME}_${CurProc}_${i}.${EXT}
done
#-----------------------------------------------------------------------------
#FSDither
CurProc=FloydSteinbergDither
echo Processing ${CurProc}...
OUTDIR=$OUTROOT/$NAME/${CurProc}
if [ ! -d $OUTDIR ];
then
mkdir -p $OUTDIR;
fi
for i in `seq 1 1 8`
do
./image -input ${INNAME} -${CurProc} $i -output ${OUTDIR}/${NAME}_${CurProc}_${i}.${EXT}
done
# #-----------------------------------------------------------------------------
# #Sharpen
CurProc=sharpen
echo Processing ${CurProc}...
OUTDIR=$OUTROOT/$NAME/${CurProc}
if [ ! -d $OUTDIR ];
then
mkdir -p $OUTDIR;
fi
for i in `seq 1 2 25`
do
./image -input ${INNAME} -${CurProc} $i -output ${OUTDIR}/${NAME}_${CurProc}_${i}.${EXT}
done
#-----------------------------------------------------------------------------
#edgeDetect
CurProc=edgeDetect
echo Processing ${CurProc}...
OUTDIR=$OUTROOT/$NAME/${CurProc}
if [ ! -d $OUTDIR ];
then
mkdir -p $OUTDIR;
fi
./image -input ${INNAME} -${CurProc} -output ${OUTDIR}/${NAME}_${CurProc}.${EXT}

#-----------------------------------------------------------------------------
#Scale+sampling
CurProc=scale
echo Processing ${CurProc}...
OUTDIR=$OUTROOT/$NAME/${CurProc}
if [ ! -d $OUTDIR ];
then
mkdir -p $OUTDIR;
fi
SampleOptions=("IMAGE_SAMPLING_POINT" "IMAGE_SAMPLING_BILINEAR" "IMAGE_SAMPLING_GAUSSIAN")
list=(0.5 0.8 1 1.5 2)

for sx in ${list[@]}
do
    for sy in ${list[@]}
    do
        for option in ${SampleOptions[@]}
        do
            ./image -input ${INNAME} -sampling ${option} -${CurProc} ${sx} ${sy} -output ${OUTDIR}/${NAME}_${CurProc}_${sx}_${sy}_${option}.${EXT}
        done
    done
done

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

for angle in `seq -270 20 270`
do
    for option in ${SampleOptions[@]}
    do
        ./image -input ${INNAME} -sampling ${option} -${CurProc} ${angle} -output ${OUTDIR}/${NAME}_${CurProc}_${angle}_${option}.${EXT}
    done
done
#-----------------------------------------------------------------------------