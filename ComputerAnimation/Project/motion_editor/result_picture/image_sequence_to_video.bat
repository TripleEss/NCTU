.\ffmpeg.exe -r 30 -start_number 708 -i %%05d.png -c:v libx264 -qp 1 -profile:v high444 -preset fast -pix_fmt yuv420p result.mp4
@ECHO OFF
EXIT
