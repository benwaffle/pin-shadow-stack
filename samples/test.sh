for bin in $(file * | grep "ELF" | cut -d ':' -f 1); do 
	tput setaf 6; echo "test ./$bin"; tput sgr0
	../../pin-2.14/pin.sh -t ../obj-intel64/shadow_stack.so -- ./$bin 1 2 3 4
	ret=$?
	if [[ $ret -eq 0 ]]; then
		tput setaf 2; echo -e "success\n"; tput sgr0
	else
		tput setaf 1; echo -e "returned $ret\n"; tput sgr0
	fi
done
