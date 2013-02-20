reset
set terminal wxt
#set term gif animate
#set output "animate.gif"

dir="DATOS/fix"
prefix="b"

plots=system(sprintf("ls -v ./%s/%s*",dir,prefix))

set xrange [0:30]
set yrange [0:20]
do for [name in plots]{
  plot name u 1:2:3 with circles title name
  pause 1
}


#set output
