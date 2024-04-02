    lsmod | grep pseudo
    grep pseudo /proc/devices 
    sudo mknod /dev/pseudo c 239 0

    make
    sudo insmod pseudo.ko 
    sudo insmod pseudo.ko pseudo_capacity=50
    sudo dmesg
    ./testpseudo 20
    sudo rmmod pseudo 

currently (write functionality):
- writing creates permission error
- giving perm with sudo chmod 777, doesn't fix it

for clangd:
    bear -- make
