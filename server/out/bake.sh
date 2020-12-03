#!/bin/sh
if [ $# -ne 1 ]
then
		echo "usage: $0 <current directory>"
		exit
fi
workdir=$1
dstdir=/mnt/hgfs/myshare/bake/$workdir  
if [ -d $dstdir ]
then
		rm -r "$dstdir"
fi
mkdir $dstdir
echo $dstdir
cp -r * $dstdir 
