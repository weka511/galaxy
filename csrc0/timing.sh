#!/usr/bin/bash
for i in 1 2 3 4 5 6 7 8 9 10
 do
    ./galaxy.exe --threads 4 --max_iter 10 --numbodies 100000  --img_iter 1
	./galaxy.exe --threads 2 --max_iter 10 --numbodies 100000 --img_iter 1
	./galaxy.exe --max_iter 10 --numbodies 100000 --img_iter 1

 done
 