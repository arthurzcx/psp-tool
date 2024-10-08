all=`ls *.png`

for file in $all; do
	pngcrush -ow -rem allb -reduce $file
done
