reset
set terminal wxt
#set term gif animate
#set output "animate.gif"


dir="DATOS"
prefix="billar"

plots=system(sprintf("find ./%s -name '%s*'",dir,prefix))

do for [name in plots]{
  plot name u 1:2:3 with circles title name
  pause 1
}


#set output


