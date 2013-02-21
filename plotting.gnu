reset
set terminal wxt
#set term gif animate
#set output "animate.gif"

dir="DATOS/apps"
prefix="a"

plots=system(sprintf("ls -v ./%s/%s*",dir,prefix))

set xrange [-5:105]
set yrange [-5:105]
set size ratio -1
do for [name in plots]{
  plot name u 1:2:3 with circles title name
  pause 1
}


#set output
