/*
Usage: eadu ip file
ip: ipv4 address of arduino due to which we will transfer firmware file
file: path to firmware file to transfer to arduino due

Notes:

TODO:
catch errors
open firmware
create socket
connect
write to arduino to say that we are sending firmware
wait for answer from arduino
send data (page per page or as a whole)
verify data sent
say to arduino that firmware transfer is done
close socket
*/