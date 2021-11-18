#!/usr/bin/env perl
use strict;
use warnings;

sub filterNames {
    my @myfiles = @{$_[0]};
    # not sure here if I should use the same name, mycluster
    my $mycluster = $_[1];
    my @myfiles_filtered = ();
    foreach (@myfiles) {
        if ($_ =~ /([0-9]*)_([0-9]+)\.[a-z]{1,5}/) {
            if ($1 == $mycluster) {
                push(@myfiles_filtered, $_);
            }
        }
    }
    return @myfiles_filtered;
}

sub findErrors {
    # not sure if I should use this name
    my $myfile = $_[0];
    my $pat = $_[1];
    my $matched = 0;
    open(my $fh, '<', $myfile);
    while (my $line = <$fh>) {
        if ($line =~ /$pat/i) {
            $matched = 1;
            close $fh;
            last;
        }
    }
    return $matched
}

# submit condor jobs using condor_submit
# redirect STDERR and STDOUT to log file
sub submit {
    print "\n";
    my $myjdl = $_[0];
    # /r option is for copy the input
    my $mylog = $myjdl =~ s/jdl/log/r;
    system("condor_submit $myjdl > $mylog 2>&1");
    print ("Submitted $myjdl\n");
    print ("Output log of this command is $mylog\n\n");

    open (my $logfile, '<', $mylog);
    while (<$logfile>) {
      print $_;
    }
    close $logfile;

    print ("End\n");
}

# analyze the log files
sub analyze {
    my $mylog = $_[0];
    print "\nAnalyzing $mylog ......\n\n";
    open(my $in_fh, "<", $mylog);
    my $mycluster = "";
    while (<$in_fh>) {
        if ( $_ =~ /cluster (.*)\./ ) {
            $mycluster = $1;
            print "The cluster is $mycluster\n\n";
        }
    }
    $mylog =~ "sub(.*)\.log";
    my $mylogdir = "log$1";
    # open the directory
    opendir(my $dh, $mylogdir);
    my @mylogs = grep( /^[^.].*\.log/
                       && -f "$mylogdir/$_",
                       readdir ($dh));
    closedir $dh;

    # need to reopen the directory, unless nothing will be found
    opendir($dh, $mylogdir);
    my @myerrs = grep( /^[^.](.*)\.err/
                       && -f "$mylogdir/$_",
                       readdir ($dh));
    closedir $dh;

    # get the filtered logs
    my @mylogs_filtered = filterNames(\@mylogs, $mycluster);
    my @mylogs_buggy = ();
    foreach (@mylogs_filtered) {
        my $matched_log = findErrors("$mylogdir/$_", "Abnormal");
        if ($matched_log) {
            if ( $_ =~ /(.*)_(.*)\..+/ ) {
                push(@mylogs_buggy, $2);
            }
        }
    }
    print "Jobs quitting abnormally are:\n";
    foreach (@mylogs_buggy) {
        print "    ", $mycluster, "_", $_, ".log\n";
    }
    print "\n";

    # get the filtered errs
    my @myerrs_filtered = filterNames(\@myerrs, $mycluster);
    my @myerrs_buggy = ();
    foreach (@myerrs_filtered) {
        my $matched_err = findErrors("$mylogdir/$_", "ERROR");
        if ($matched_err) {
            if ( $_ =~ /(.*)_(.*)\..+/ ) {
                push(@myerrs_buggy, $2);
            }
        }
    }
    print "Jobs having errors are:\n";
    foreach (@myerrs_buggy) {
        print "    ", $mycluster, "_", $_, ".err\n";
    }
    print "\n";

    # I copy the following from
    # https://www.perlmonks.org/?node_id=208210
    my (%hsh, @union_arr);
    @hsh{@mylogs_buggy, @myerrs_buggy} = ();
    @union_arr = keys %hsh;
    print "The unions of these jobs are:\n";
    foreach (@union_arr) {
        print "    ", $mycluster, "_", $_, "\n";
    }
    print "\nAnalyzed $mylog\n\n";

    return @union_arr;
}

my %functions = (
    submit => \&submit,
    analyze => \&analyze,
    );
my $function = shift;

if (exists $functions{$function}) {
    $functions{$function}->(shift);
} else {
    print "Please try commands:\n";
    print "\tsubmit\n";
}
