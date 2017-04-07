#!/usr/bin/env perl
use AnyEvent::Socket;

my @packets = (
  "Asterisk Call Manager/3.9\015\012",
  "Response: Success\015\012",
  "Message: Server 127.0.0.1:5038 - authentication accepted\015\012\015\012",
);

tcp_server '127.0.0.1', 5038, sub {
   my ($fh, $host, $port) = @_;
   my $data;
   while (my $pack = shift @packets) {
     syswrite $fh, $pack;
   }
   sysread $fh, $data, 1024;
   print "RCV: $data";
   exit(0);
};

AnyEvent->condvar->recv;
