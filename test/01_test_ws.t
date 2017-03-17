#!/usr/bin/env perl
use AnyEvent::Socket;
use AnyEvent::WebSocket::Client;
use JSON;
use Test::More;

my @packets = (
  "Response: Success\015\012",
  "Message: Authentication accepted\015\012\015\012",
  "Event: FullyBooted\015\012Privilege: system,all\015\012Status: Fully Booted\015\012\015\012",
);

my @tests = (
  "Foo",
  "Bar",
  "Baz"
);
plan tests => scalar @tests;

sub cb_func{
    our $conn = eval { shift->recv };
    die $@ if ($@);
    $conn->on (each_message => sub {
        my ($conn, $msg) = @_;
        if ($msg->is_text or $msg->is_binary) {

          if(my $line = shift @tests){
            is($msg->body, $line, "'".$msg->body . "' == '$line'");
          }
          if (!@tests) {
            done_testing();
            exit()
          }

          $conn->send("OK\015\012");

        } else {
          print "Invalid message\n";
          exit(1);
        }
      });
}

AnyEvent::WebSocket::Client->new()->
  connect('ws://localhost:8000')->
  cb(\&cb_func);

tcp_server '127.0.0.1', 5038, sub {
   my ($fh, $host, $port) = @_;
   while (my $pack = shift @packets) {
     print "SEND: $pack";
     syswrite $fh, $pack;
   }
};

AnyEvent->condvar->recv;

