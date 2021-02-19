#! /usr/bin/bash

if [ ! -e /image ]
then
  #g++ -fsanitize=address -std=c++14 main.cpp pixel.cpp image.cpp -o image
  g++ -std=c++14 main.cpp pixel.cpp image.cpp -o image
fi

./image -input ./SampleImages/goldy.ppm -quantize 2 -output ./Results/goldy_quantize_2.png
./image -input ./SampleImages/goldy.ppm -quantize 2 -output ./Results/goldy_quantize_2.ppm
./image -input ./SampleImages/goldy.ppm -ppm_depth 2 -output ./Results/goldy_depth_2.ppm
./image -input ./SampleImages/goldy.ppm -ppm_depth 8 -output ./Results/goldy_depth_8.ppm
./image -input ./SampleImages/goldy.ppm -crop 250 50 100 150 -output ./Results/goldy_cropped.ppm
./image -input ./SampleImages/goldy.ppm -crop 250 50 100 150 -extractChannel 1 -output ./Results/goldy_cropped_green.jpeg
./image -input ./SampleImages/goldy.ppm -brightness 1.4 -crop 250 50 100 150 -extractChannel 0 -output ./Results/goldy_cropped_bright_red.jpg

./image -input ./SampleImages/sunzoom.png -quantize 2 -output ./Results/sunzoom_quantize_2.png
./image -input ./SampleImages/sunzoom.png -quantize 2 -output ./Results/sunzoom_quantize_2.ppm
./image -input ./SampleImages/sunzoom.png -ppm_depth 2 -output ./Results/sunzoom_depth_2.ppm
./image -input ./SampleImages/sunzoom.png -ppm_depth 8 -output ./Results/sunzoom_depth_8.ppm
./image -input ./SampleImages/sunzoom.png -crop 250 50 100 150 -output ./Results/sunzoom_cropped.ppm
./image -input ./SampleImages/sunzoom.png -crop 250 50 100 150 -extractChannel 1 -output ./Results/sunzoom_cropped_green.jpeg
./image -input ./SampleImages/sunzoom.png -brightness 1.4 -crop 250 50 100 150 -extractChannel 0 -output ./Results/sunzoom_cropped_bright_red.jpg