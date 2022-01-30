set datafile separator ","
set ytics 20 nomirror tc lt 1
set ylabel 'pwm (%)' tc lt 1
set y2tics 10 nomirror tc lt 2
set y2label 'temp (deg F)' tc lt 2
set xlabel 'minutes'
set title "3 heaters on wood; b_i: 13.0V"
plot "output.txt" using ($1/60):($2/255*100) linetype 1 title "pwm", "output.txt" using ($1/60):($3*9/5+32) linetype 2 axes x1y2 title"temp"
pause 5 
reread

