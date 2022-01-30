set datafile separator ","
set ylabel 'temp C'
set xlabel 'seconds'
plot "output.txt" using ($1/1000):($3) linetype 1
pause 5
reread
