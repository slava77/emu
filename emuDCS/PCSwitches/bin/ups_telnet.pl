#!/usr/local/bin/perl

use Net::Telnet; 

$IP = "192.168.30.101";
#$COMMAND can be 'on', 'off' or 'reboot'
$COMMAND = shift @ARGV;
$SWITCH = shift @ARGV;

$telnet = new Net::Telnet (Timeout=>5, Prompt=>'/> $/'); 
$telnet->open($IP); 
$telnet->login('admin', 'admin');
$telnet->cmd("1");
$telnet->cmd("1");
$telnet->cmd("2");
$telnet->cmd("2");
$command = $SWITCH + 2;
$telnet->cmd($command);
if($COMMAND eq "on" || $COMMAND eq "off") {
  $telnet->cmd("2");

} else {
  $telnet->cmd("3");
}
$telnet->print('x');
$telnet->print('x');
$telnet->print('x');
$telnet->print('x');
$telnet->print('x');
$telnet->close();
