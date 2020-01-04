ffmpeg -i reachable_PseudoinverseSolver.mp4 -i reachable_DampedLeastSquaresSolver.mp4 -filter_complex "[0:v:0]pad=iw*2:ih[bg]; [bg][1:v:0]overlay=w" output.mp4
@ECHO OFF
EXIT
