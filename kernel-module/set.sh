make
sudo rmmod pseudo
sudo insmod pseudo.ko 
grep pseudo /proc/devices 
sudo mknod /dev/pseudo c 239 0
sudo chmod 777 /dev/pseudo
