# README #

Smart Traffic Meter is a program for measuring network usage and displaying the statistics in various formats. The program runs as a system service and is managed through a website interface.

## How do I get set up?

If you are using Arch linux you can install Smart Traffic Meter from the AUR repository. Find the *smarttrafficmeter* package and install it.

If you want to compile and install the program by yourself, do the following:

### Install required libraries
* curl
+ boost
- binutils

Make sure that you have installed development files for the above dependencies.

### Clone the git repository
*git clone --depth 1 https://github.com/rafalfr/smarttrafficmeter.git*

### Compile the program
If you use *Code::Blocs* IDE, you can open the *SmartTrafficMeter.cbp* project file and build the *release* or *debug* version.

If you do not use *Code::Blocks*, you can build the program using the *make* tool. For the *release* version execute:

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
