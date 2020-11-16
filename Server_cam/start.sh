#!/bin/bash
lsmod
modprobe fspwm
if [ ! -f "/dev/pwm" ]; 
then
	mknod /dev/pwm c 256 7
fi 
cd /bin
./smart_baby_room
