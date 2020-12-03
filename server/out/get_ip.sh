#!/bin/sh
i=0
while [ $i -le 10 ]
do
		ETH="eth"$i
		ifconfig $ETH | grep "inet addr" > ip.config
		if [ $? -ne 0 ]
		then
				i=`expr $i + 1`
				continue
		else
				break
		fi
done	
if [ -s ip.sonfig ]
then
		echo "失败"
fi
		
