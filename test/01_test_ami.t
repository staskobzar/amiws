use AnyEvent::Socket;

my @packets = (
  "Response: Success\015\012",
  "Message: Authentication accepted\015\012\015\012",
  "Event: FullyBooted\015\012Privilege: system,all\015\012Status: Fully Booted\015\012\015\012",
);

tcp_server '127.0.0.1', 5038, sub {
   my ($fh, $host, $port) = @_;
   while (my $pack = shift @packets) {
     print "SEND: $pack";
     syswrite $fh, $pack;
   }
   exit(0);
};

AnyEvent->condvar->recv;
