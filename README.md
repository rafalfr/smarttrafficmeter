# README #

Smart Traffic Meter is a program for measuring network usage and displaying the statistics in various formats. The program runs as a system service and is managed through a website interface.

## How do I get set up?

If you are using Arch linux you can install Smart Traffic Meter from the AUR repository. Find the *smarttrafficmeter* package and install it.

If you want to compile and install the program by yourself, do the following:

### Install required libraries
* curl
+ boost
- binutils

### Clone the git repository
*git clone https://rafalfr@bitbucket.org/rafalfr/smarttrafficmeter.git*

### Compile the program

for the *release* version execute:

*make -f SmartTrafficMeter.cbp.mak linux_release*

or for the *debug* version execute:

*make -f SmartTrafficMeter.cbp.mak linux_debug*

### Install the program

Copy the compiled binary file *smarttrafficmeter* to */usr/bin/*

The compiled *smarttrafficmeter* is found in either *bin/Release* or *bin/Debug* for the *release* or *debug* compilations respectively.

Now you can start the program by executing the following command:
*smarttrafficmeter --daemon*

### Make the program run at computer startup

If you want the program to run at computer startup copy

*/install/smarttrafficmeter.service* file to */usr/lib/systemd/system/*

and then execute:

*sudo systemctl enable smarttrafficmeter*

*sudo systemctl start smarttrafficmeter*

### Managing the application
To *start/stop/enable/disable* Smart Traffic Meter execute:

*sudo systemctl start/stop/enable/disable smarttrafficmeter*

### Web interface
By default, the web interface is available at [http://127.0.0.1:7676](http://127.0.0.1:7676)

### Directories
default database directory: */usr/share/smarttrafficmeter*

default config directory: */etc/smarttrafficmeter*

default log directory: */var/log/smarttrafficmeter*
