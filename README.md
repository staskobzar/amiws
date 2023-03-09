## amiws - Asterisk Manager Interface (AMI) to web-socket proxy

[![Build Status](https://travis-ci.org/staskobzar/amiws.svg?branch=master)](https://travis-ci.org/staskobzar/amiws)
![](https://img.shields.io/badge/license-GPL_3-green.svg "License")
[![codecov](https://codecov.io/gh/staskobzar/amiws/branch/master/graph/badge.svg)](https://codecov.io/gh/staskobzar/amiws)
![verion-2.2.1](https://img.shields.io/badge/version-2.2.1-blue.svg)


### Introduction
*amiws* is simple proxy from AMI to WEB. It can connect to one or more Asterisk PBXs via AMI (Asterisk Manager Interface), read messages from AMI stream and send actions/commands to it. Received messages are parsed and converted to JSON.
*amiws* also provides HTTP/WebSocket interface and sends JSON messages to all connected users via HTTP.


Here is simple workflow scheme:
![amiws workflow](https://github.com/staskobzar/amiws/blob/master/doc/amiws.workflaw.png)


*amiws* proxy can help to build interactive, real-time dashboards for Asterisk PBX single or multiple servers. The example of a simple dashboard can be found in directory "web_root". Here is how it would look like when connecting two Asterisk PBXs:
![web interface](https://github.com/staskobzar/amiws/blob/master/doc/amiws.user.screen.png)


### Features
* AMI protocol implementation
* Plain TCP or SSL/TLS connection to AMI
* Logging with syslog
* HTTP and WebSocket server
* SSL/TLS encrypted connection for HTTP and WebSocket
* WWW Digest authentication with username/password for HTTP(s)
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
* [mongoose](https://github.com/cesanta/mongoose) - awesome embedded networking library from [Cesanta](https://www.cesanta.com/).
* [frozen](https://github.com/cesanta/frozen) - awesome JSON parser and emitter from [Cesanta](https://www.cesanta.com/).
* [re2c](http://re2c.org/) - awesome lexer generator for AMI protocol implementation.
* [cmocka](https://cmocka.org/) - awesome unit testing framework for C.
* [lemon](http://www.hwaci.com/sw/lemon/lemon.html) - parser generator to process YAML tokens

They do not need to be installed. *mongoose* and *frozen* are already included. *re2c* and *cmocka* are only needed for developers.

### Building and install
```
autoreconf -if
./configure
make && make install
```

In case of problems, try to run script ```./bootstrap```.

To enable SSL support:
```
./configure --with-ssl
make && make install
```

To create source documentation:
```
make doc
```

To run unit tests (requires cmocka):
```
make check
```

This repo also provides init scripts for [System V](https://github.com/staskobzar/amiws/blob/master/etc/amiws.sysv.init)
and [systemctl](https://github.com/staskobzar/amiws/blob/master/etc/amiws.service) in "etc/" directory.

### Configuration

Program behaviour is controlled by configuration file. Configuration parameters are described in sample file ["amiws.annotated.yaml"](https://github.com/staskobzar/amiws/blob/master/etc/amiws.annotated.yaml) in directory "etc" of this repository.

### JSON message

Example of *amiws* JSON message:

```JSON
{
   "type": 3,
   "server_id": 1,
   "server_name": "asterisk01.local",
   "ssl": false,
   "data": {
      "Event": "Hangup",
      "Privilege": "call,all",
      "Channel": "SIP/sipp-00000a6a",
      "ChannelState": "6",
      "ChannelStateDesc": "Up",
      "CallerIDNum": "16464964806",
      "CallerIDName": "Alice",
      "ConnectedLineNum": "<unknown>",
      "ConnectedLineName": "<unknown>",
      "Language": "en",
      "AccountCode": "",
      "Context": "from-sip",
      "Exten": "55213477583",
      "Priority": "8",
      "Uniqueid": "1495919040.5315",
      "Linkedid": "1495919040.5315",
      "Cause": "0",
      "Cause-txt": "Unknown"
   }
}
```

* *type* - AMI message type (see below)
* *server_id* - Asterisk server ID. Sequential number generated by *amiws* for each AMI connection.
* *server_name* - AMI connection server name as in configuration file.
* *ssl* - Flag to indicate id AMI connection is SSL encrypted.
* *data* - AMI message itself.


_AMI message types_:
- 0 - Unknown
- 1 - Prompt
- 2 - Action
- 3 - Event
- 4 - Response
- 5 - Respons with command output
- 6 - Queues list. AMI returns queues list not as AMI packet but as a text (like command: queue show). That's why here is special type.

AMI description in details can be found in [Asterisk wiki](https://wiki.asterisk.org/wiki/display/AST/Home).

*amiws* also accepts JSON messages and sends them back to all Asterisk servers:
```javascript
sock.send(JSON.stringify({"Action": "CoreStatus", "ActionID": "12345"}));
```
Beware that this will send same Action to all AMI servers!

Keep this in mind when you deploy *amiws* on public Internet and protect access to it.

#### Special header AMIServerID
To send message to specified AMI server you can use header ```AMIServerID```.
This should correspond to the ```server_id``` in requests.

Example:
```javascript
sock.send(JSON.stringify({"Action": "CoreStatus", "AMIServerID": 1}));
```

#### Sending multiple variables
In some cases, you will need to send multiple variables. This can be done with a list:

Example:
```javascript
sock.send(JSON.stringify({"Action": "CoreStatus", "Variable": ["Trunk=provider07", "Hop=hop315"]}));
```

List is unnecessary when there is only one variable:
```javascript
sock.send(JSON.stringify({"Action": "CoreStatus", "Variable": "Trunk=provider07"}));
```

### SSL/TLS transport
SSL/TLS transport usage is described in this blog article:
http://staskobzar.blogspot.ca/2017/05/amiws-asterisk-manager-iterface-to-web.html

Some more information can be found in annotated configuration file.

### Digest Authentication
HTTP content can be protected with username/password. When this option is enabled, anyone trying to access web-page will have to provide username and password to proceed.

Configuration parameters to set are:
```
auth_domain   : example.com
auth_file     : /var/www/.htaccess
```

Parameter *auth_file* is a path to the file with username, password and domain. This file can be generated with utility "htdigest" which is the part of Apache httpd server package. For example:
```
$ htdigest -c /var/www/.htaccess example.com admin
```

Second argument, domain "example.com", must be the same as set in parameter *auth_domain*. 
