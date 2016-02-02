#!/bin/bash

LEML=./leml
EPS=0.0000000001

function abs() {
	echo $(echo $1 | awk '{
		if($1>=0) {
			print $1
		} else {
			printf("%f",$1*-1)
		}
	}')
}

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
	diff=$(echo "$(./leml -jit $testcase.ml) - $(cat $resval)" | bc);
	cmpres=$(echo "$(abs $diff) < $EPS" | bc | sed '/^\s*$/d');
	if [ $cmpres == 1 ]; then
		printf "  %-20s ||  \e[34mpassed\e[0m\n" $testcase
	else
		printf "  %-20s ||  \e[31mfailed\e[0m\n" $testcase
	fi;
done

printf "\n"
