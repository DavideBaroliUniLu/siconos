#set  term X11
#!tail -n 100000 result.dat > result-gp.dat
plot\
"result.dat" u 1:2 t "Ball position" w l,\
"result.dat" u 1:3 t "Ball Velocity" w l,\
"result.dat" u 1:4 t "Reaction force" w l

#,\
#"result-gp.dat" u 1:5 t "Multiplier" w l
     


