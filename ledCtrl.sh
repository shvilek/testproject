#!/bin/bash
 while true
 do
 PS3='Please enter your choice: '
options=("SetState" "GetState" "SetColor" "GetColor" "SetRate" "GetRate" "Quit")
stateOptions=("on" "off")
colorOptions=("red"  "green"  "blue")
rateOptions=("0"  "1"  "2"  "3"  "4"  "5")
 select opt in "${options[@]}" 
do
    case $opt in
        "SetState")
	select opt in "${stateOptions[@]}";
	do
	 	./client set-led-state  $opt
		break;
	done
	break;
	;;
        "GetState")
            ./client get-led-state
            ;;
        "SetColor")
	select opt in "${colorOptions[@]}";
	do
	 	./client set-led-color  $opt
		break;
	done
	break;
	;;

	"GetColor")
	./client get-led-color
	;;

	"SedRate")
	select opt in "${rateOptions[@]}";
	do
	 	./client set-led-rate  $opt
		break;
	done
	break;
	;;

	"GetRate")
	./client get-led-rate
	;;
        "Quit")
            exit
            ;;
        *) echo invalid option;;
    esac
done
 done