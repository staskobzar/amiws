use AnyEvent::Socket;

my @packets = (
  "T1Response: Success\015\012",
  "T1Message: Authentication accepted\015\012\015\012",
  "T1Event: FullyBooted\015\012Privilege: system,all\015\012Status: Fully Booted\015\012\015\012",
  "T1Event: PeerStatus\015\012",
  "T1Privilege: system,all\015\012",
  "T1ChannelType: SIP\015\012Peer: SIP/1225\015\012",
  "T1PeerStatus: Reachable\015\012Time: 9\015\012\015\012",
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
