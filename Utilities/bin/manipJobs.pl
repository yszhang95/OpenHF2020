#!/usr/bin/env perl
use strict;
use warnings;

# submit condor jobs using condor_submit
# redirect STDERR and STDOUT to log file
sub submit {
    my $myjdl = $_[0];
    # /r option is for copy the input
    my $mylog = $myjdl =~ s/jdl/log/r;
    system("condor_submit $myjdl > $mylog 2>&1");
    print ("Submitted $myjdl\n");
    print ("Output log of this command is $mylog\n");
}

my %functions = (
    submit => \&submit,
    );
my $function = shift;

if (exists $functions{$function}) {
    $functions{$function}->(shift);
} else {
    print "Please try commands:\n";
    print "\tsubmit\n";
}
