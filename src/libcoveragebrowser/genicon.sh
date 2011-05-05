#!/bin/bash -xv
RES=( 16 24 32 48 64 96 128 256 512 )
DIR=$PWD
rm -rf /tmp/coveragemeter_logo
mkdir -p  /tmp/coveragemeter_logo
cd  /tmp/coveragemeter_logo
for R in ${RES[@]}
do
  inkscape $DIR/images/testcocoon.svg --export-png=logo_${R}x${R}.png --export-width=$R --export-height=$R --without-gui &
  inkscape $DIR/../executionagent/images/executionagent.svg --export-png=executionagent_${R}x${R}.png --export-width=$R --export-height=$R --without-gui &
  inkscape $DIR/images/coveragemeter_uninstall.svg --export-png=uninstall_${R}x${R}.png --export-width=$R --export-height=$R --without-gui &
  inkscape $DIR/../toolselector/images/toolselector.svg --export-png=toolselector_${R}x${R}.png --export-width=$R --export-height=$R --without-gui &
  inkscape $DIR/../Windows/console.svg --export-png=console_${R}x${R}.png --export-width=$R --export-height=$R --without-gui &
  wait
done 

icotool -c -o $DIR/coveragemeter_logo.ico $(ls logo_*.png)
icotool -c -o $DIR/../Windows/coveragemeter.ico logo_16x16.png logo_24x24.png logo_32x32.png  logo_64x64.png logo_128x128.png
icotool -c -o $DIR/../executionagent/executionagent.ico executionagent_16x16.png executionagent_24x24.png executionagent_32x32.png  executionagent_64x64.png executionagent_128x128.png
icotool -c -o $DIR/../Windows/console.ico console_16x16.png console_24x24.png console_32x32.png  console_64x64.png console_128x128.png
icotool -c -o $DIR/../toolselector/toolselector.ico toolselector_16x16.png toolselector_24x24.png toolselector_32x32.png  toolselector_64x64.png toolselector_128x128.png
icotool -c -o $DIR/../Windows/coveragemeter_uninstall.ico uninstall_16x16.png uninstall_24x24.png uninstall_32x32.png uninstall_64x64.png uninstall_128x128.png 
png2icns $DIR/coveragemeter_logo.icns  logo_16x16.png logo_32x32.png logo_128x128.png logo_256x256.png logo_512x512.png
png2icns $DIR/../executionagent/executionagent.icns  executionagent_16x16.png executionagent_32x32.png executionagent_128x128.png executionagent_256x256.png executionagent_512x512.png
