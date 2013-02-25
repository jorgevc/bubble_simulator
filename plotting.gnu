reset
set terminal wxt
#set term gif animate
#set output "animate.gif"

dir="./DATOS/apps/"
prefix="a"

plots=system(sprintf("ls -v ./%s%s* | xargs -n1 basename",dir,prefix))

do for [name in plots]{
set multiplot layout 1,2
	set xrange [-5:105]
	set yrange [-5:105]
	set size ratio -1
	plot dir.name u 1:2:3 with circles title name
	set xrange [0:6]
	set yrange [0:1]
	set size ratio 1
	plot dir."SD_".name u 1:2 title name
unset multiplot
  pause 1
}


#set output
