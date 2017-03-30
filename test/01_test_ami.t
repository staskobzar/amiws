use AnyEvent::Socket;

my @packets = (
  "Asterisk Call Manager/3.9\015\012",
  #"Response: Success\015\012",
  #"Message: Server 127.0.0.1:5038 - authentication accepted\015\012\015\012",
  #"Event: FullyBooted\015\012Privilege: system,all\015\012Status: Fully Booted\015\012\015\012",
  #"Event: PeerStatus\015\012",
  #"Privilege: system,all\015\012",
  #"ChannelType: SIP\015\012Peer: SIP/1225\015\012",
  #"PeerStatus: Reachable\015\012Time: 9\015\012\015\012",
);

tcp_server '127.0.0.1', 5038, sub {
   my ($fh, $host, $port) = @_;
   my $data;
   while (my $pack = shift @packets) {
     #print "SEND: $pack";
     syswrite $fh, $pack;
   }
   sysread $fh, $data, 1024;
   print "RCV: $data";
   exit(0);
};

AnyEvent->condvar->recv;
