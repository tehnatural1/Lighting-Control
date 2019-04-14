# Lighting-Control

Clone this repo
```
git clone https://github.com/tehnatural1/Lighting-Control
```

Compile the code
```
gcc -o GPIOControl GPIOControl.c -lpthread
```

Start the service
```
./GPIOControl.c
```

Disable wifi power-management by running a script, set a service to run it on boot
```
./wifi_pw_mgmt.sh
```

To run services on boot place the files located in services to /etc/systemd/system/multi-user.target.wants and run the files on boot
```
systemctl enable gpiocontrol.service
systemctl enable wifipwmgmt.service
```

To run the services immediately
```
systemctl start gpiocontrol.service
systemctl start wifipwmgmt.service
```
