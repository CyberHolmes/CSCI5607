### To compile the code, run following command:
```
make
```
### Before running the program:
```
export LD_LIBRARY_PATH=./oidn/lib/
```

### To run the program, run following command:
```
./ray <PATH/SceneFileName>
./ray <PATH/SceneFileName> -samplesize <n1> -depth <n2> -pathtrace -resolution <nw> <nh>
```
### Controls:

'p' - toggle between path trace and ray trace

'space' - toggle between denoised image and original ray/path traced image

'l' - toggle whether to use lights in path trace option, this will cause re-rendering of the image

'o' - save images (original and denoised)

The following camera control options are for ray trace only, due to path trace response time

'w': positiveMovement.z

's': negativeMovement.z

'a': negativeMovement.x

'd': positiveMovement.x

'q': positiveMovement.y

'e': negativeMovement.y  

LEFT:  negativeTurn.x

RIGHT: positiveTurn.x

UP:    positiveTurn.y

DOWN:  negativeTurn.y
