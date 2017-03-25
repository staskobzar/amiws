#!/usr/bin/env perl

use IO::Socket;

my $remote_host = '127.0.0.1';
my $remote_port = 5038;
my $data = '';

$socket = IO::Socket::INET->new(PeerAddr => $remote_host,
                                PeerPort => $remote_port,
                                Proto    => "tcp",
                                Type     => SOCK_STREAM)
    or die "Couldn't connect to $remote_host:$remote_port : $@\n";

  $socket->recv($data, 1024);
  print "DATA: $data";
  $socket->recv($data, 1024);
  print "DATA: $data";
  print "\n";


$socket->close();
