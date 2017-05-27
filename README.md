## amiws - Asterisk Manager Iterface (AMI) to web-socket proxy

[![Build Status](https://travis-ci.org/staskobzar/amiws.svg?branch=master)](https://travis-ci.org/staskobzar/amiws)
![](https://img.shields.io/badge/license-GPL_3-green.svg "License")


### Introduction
*amiws* is simple proxy from AMI to WEB. It can connect to one or more Asterisk PBXs via AMI (Asterisk Manager Interface), read messages from AMI stream and send actions/commands to it. Received messages are parsed and converted to JSON. 
*amiws* also provides HTTP/WebSocket interface and sends JSON messages to all connected users via HTTP.


Here is simple workflow scheme:
![amiws workflow](https://github.com/staskobzar/amiws/blob/master/doc/amiws.workflaw.png)


*amiws* proxy can help to build interctive, realtime dashboards for Asterisk PBX single or multiple servers. The example of a simple dashboard can be found in directory "web_root". Here is how it would look like when connecting two Asterisk PBXs:
![web interface](https://github.com/staskobzar/amiws/blob/master/doc/amiws.user.screen.png)


### Features
* AMI protocol implementation
* Plain TCP or SSL/TLS connection to AMI
* Logging with syslog
* HTTP and WebSocket server
* SSL/TLS encypted connection for HTTP and WebSocket
* WWW Digest authenticatation with username/password for HTTP(s)
* YAML configuration file

### Command line arguments
Usage: ```amiws [OPTIONS]```

Options:
<pre>
-h, --help              Printf this help message.
-f, --file FILENAME     Configuration file. By default '/etc/amiws.yaml'
-d, --daemon            Daemonize process.
-p, --pidfile FILENAME  PID file when run as daemon. By default '/tmp/amiws.pid'
-D, --wdir PATH         Working directory when run as daemon. By default '/tmp'
</pre>

### Dependences and libraries
* ```libyaml``` - to read YAML configuration files.
* ```openssl``` - TLS/SSL for AMI and HTTP. Only if compiled with ssl support.

*amiws* relies on several greate projects:
* [mogoose](https://github.com/cesanta/mongoose) - awesome embedded networking library from [Cesanta](https://www.cesanta.com/).
* [fozen](https://github.com/cesanta/frozen) - awesome JSON parser and emitter from [Cesanta](https://www.cesanta.com/).
* [re2c](http://re2c.org/) - awesome lexer generator for AMI protocol implementation. 
* [cmocka](https://cmocka.org/) - awesome unit testing framework for C.

They do not need to me installed. *mongoose* and *frozen* are already included. *re2c* and *cmocka* are only needed for developers.

### Building and install
```
./configure
make && make install
```

To enable SSL support:
```
./configure --with-ssl
make && make install
```

### Configuration

Program behaviour is controlled by configuration file. Configuration parameters are described in sample file ["amiws.annotated.yaml"](https://github.com/staskobzar/amiws/blob/master/etc/amiws.annotated.yaml) in directory "etc" of this repository. 


