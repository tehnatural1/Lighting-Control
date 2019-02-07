# Lighting-Control
Setting the state of GPIO pin on the RPi3 by sending the corresponding pin number to a socket on a remote host. GPIO pin state is set through direct GPIO register manipulation in C-code


## Installation

# Clone the repo
```
git clone https://github.com/tehnatural1/Lighting-Control
```

# Compile the code
```
gcc -o GPIOControl GPIOControl.c -lpthread
```

# Run
```
./GPIOControl.c
```

In order to test your setup you can send a curl request to your RPi to trigger a GPIO pin
```
curl -d '4' 192.168.10.10:9090
```
