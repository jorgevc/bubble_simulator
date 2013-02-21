reset
set terminal wxt
#set term gif animate
#set output "animate.gif"

dir="DATOS/fix"
prefix="i"

plots=system(sprintf("ls -v ./%s/%s*",dir,prefix))

set xrange [-4:16]
set yrange [0:20]
do for [name in plots]{
  plot name u 1:2:3 with circles title name
  pause 1
}


#set output
