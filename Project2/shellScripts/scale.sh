#! /usr/bin/bash

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
# list1=(0.1 0.5 2 5 1.5)
# list2=(0.1 0.5 2 5 1)

# for i in `seq 1 1 ${#list1[@]}`
# do
#     sx=${list1[${i}]}
#     sy=${list2[${i}]}
#         for option in ${SampleOptions[@]}
#         do
#             ./image -input ${INNAME} -sampling ${option} -${CurProc} ${sx} ${sy} -output ${OUTDIR}/${NAME}_${CurProc}_${sx}_${sy}_${option}.${EXT}
#         done
#     done
# done
list1=(0.2 0.5 2 5 1.5)
list2=(0.2 0.5 2 5 1)

for i in `seq 0 1 3`
do
    sx=${list1[${i}]}
    sy=${list2[${i}]}
    for option in `seq 0 1 2` #${SampleOptions[@]}
    do
        # echo sx=${sx} sampling=${option}
        ./image -input ${INNAME} -sampling ${option} -${CurProc} ${sx} ${sy} -output ${OUTDIR}/${NAME}_${CurProc}_${sx}_${sy}_${option}.${EXT}
    done
done