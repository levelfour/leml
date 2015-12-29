#!/bin/bash

LEML=./leml

if [ ! -f LEML ]; then
	echo "Error: type \`make\` to build leml first"
	exit 1
fi

cat << 'EOS'

  oooo              ooo        ooooo  ooooo       
  `888              `88.       .888'  `888'       
   888    .ooooo.    888b     d'888    888        
   888   d88' `88b   8 Y88. .P  888    888        
   888   888ooo888   8  `888'   888    888        
   888   888    .o   8    Y     888    888       o
  o888o  `Y8bod8P' #o8o        o888o #o888ooooood8

EOS

printf "  %-20s ||  result \n" "testcase"
printf " --------------------- || ----------\n"

for resval in test/*.result; do
	testcase=${resval%.result};
	if [ "$(./leml -jit $testcase.ml | diff $resval -)" = "" ]; then
		printf "  %-20s ||  \e[34mpassed\e[0m\n" $testcase
	else
		printf "  %-20s ||  \e[31mfailed\e[0m\n" $testcase
	fi;
done

printf "\n"
