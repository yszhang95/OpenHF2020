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
            if ($1 eq $mycluster) {
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
            if ($line =~ /OpenHF2020\.tar\.gz/) {
                $matched = 0;
            }
            if ($line =~ /\$HOME\/\.root\.mimes/) {
                $matched = 0;
            }
            if ($line =~ /Cannot find tree with name/) {
                $matched = 0;
                close $fh;
                last;
            }
            if ($matched) {
		# print $line;
                close $fh;
                last;
            }
        }
    }
    return $matched
}

sub findNotExistingTreeErrors {
    # not sure if I should use this name
    my $myfile = $_[0];
    my $pat = $_[1];
    my $matched = 0;
    open(my $fh, '<', $myfile);
    while (my $line = <$fh>) {
        if ($line =~ /$pat/i) {
            $matched = 1;
            if ($line =~ /OpenHF2020\.tar\.gz/) {
                $matched = 0;
            }
            if ($line =~ /\$HOME\/\.root\.mimes/) {
                $matched = 0;
            }
            if ($matched) {
                close $fh;
                last;
            }
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
        if ( $_ =~ "cluster ([0-9]*)" ) {
            $mycluster = $1;
            print "  The cluster is $mycluster\n\n";
        }
    }
    $mylog =~ "sub(.*)\.log";
    my $mylogdir = "log$1";
    $mylogdir =~ s/_resub//g;
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
        my $passed_log = findErrors("$mylogdir/$_", "return value 0");
        my $matched_log = findErrors("$mylogdir/$_", "Abnormal.*");
        if ($matched_log and !$passed_log) {
            if ( $_ =~ /(.*)_(.*)\..+/ ) {
                push(@mylogs_buggy, $2);
            }
        }
        $passed_log = 1;
        $matched_log = 0;
    }
    print "  Jobs quitting abnormally are:\n";
    foreach (@mylogs_buggy) {
        print "    ", $mycluster, "_", $_, ".log\n";
    }
    print "\n";
    my @mylogs_removed = ();
    foreach (@mylogs_filtered) {
        my $passed_log = findErrors("$mylogdir/$_", "return value 0");
        my $matched_log = findErrors("$mylogdir/$_", "remove");
        if ($matched_log and !$passed_log) {
            if ( $_ =~ /(.*)_(.*)\..+/ ) {
                push(@mylogs_removed, $2);
            }
        }
        $passed_log = 1;
        $matched_log = 0;
    }
    print "  Jobs removed are:\n";
    foreach (@mylogs_removed) {
        print "    ", $mycluster, "_", $_, ".log\n";
    }
    print "\n";
    push(@mylogs_buggy, @mylogs_removed);

    # get the filtered errs
    my @myerrs_filtered = filterNames(\@myerrs, $mycluster);
    my @myerrs_buggy = ();
    my @myerrs_buggy_inclusive = ();
    foreach (@myerrs_filtered) {
      #my $matched_err = findErrors("$mylogdir/$_", "ERROR.*[^(OpenHF2020.tar.gz)]");
        my $matched_err = findErrors("$mylogdir/$_", "[Ee][Rr][Rr][Oo][Rr]((?!OpenHF2020.tar.gz).)*");
        my $matched_err_inclusive = findNotExistingTreeErrors("$mylogdir/$_", "[Ee][Rr][Rr][Oo][Rr]((?!OpenHF2020.tar.gz).)*");
        if ($matched_err) {
            if ( $_ =~ /(.*)_(.*)\..+/ ) {
                push(@myerrs_buggy, $2);
            }
        }
        if ($matched_err_inclusive) {
            if ( $_ =~ /(.*)_(.*)\..+/ ) {
                push(@myerrs_buggy_inclusive, $2);
            }
        }
    }
    print "  Jobs having errors are:\n";
    foreach (@myerrs_buggy_inclusive) {
        print "    ", $mycluster, "_", $_, ".err\n";
    }
    print "\n";
    print "  After excluding errors 'not existing tree', jobs having errors are:\n";
    foreach (@myerrs_buggy) {
        print "    ", $mycluster, "_", $_, ".err\n";
    }
    print "\n";

    # I copy the following from
    # https://www.perlmonks.org/?node_id=208210
    my (%hsh, @union_arr);
    @hsh{@mylogs_buggy, @myerrs_buggy_inclusive} = ();
    @union_arr = keys %hsh;
    @union_arr = sort @union_arr;

    print "  The unions of these jobs are:\n";
    foreach (@union_arr) {
        print "    ", $mycluster, "_", $_, "\n";
    }
    print "\n";

    # I copy the following from
    # https://www.oreilly.com/library/view/perl-cookbook/1565922433/ch04s09.html
    my @union_errs = (); my @isect_errs = ();
    my %union_errs = (); my %isect_errs = ();
    my @union_logs = (); my @isect_logs = ();
    my %union_logs = (); my %isect_logs = ();
    my @diff_errs  = (); my @diff_logs  = ();

    foreach my $ele (@myerrs_buggy, @myerrs_buggy_inclusive) { $union_errs{$ele}++ && $isect_errs{$ele}++ }
    @union_errs = keys %union_errs;
    @isect_errs = keys %isect_errs;
    foreach (my $u = @union_errs) {
	foreach (my $i = @isect_errs) {
	    if ( $u != $i ) {
		push(@diff_errs, $u);
	    }
	}
    }


    foreach my $ele (@diff_errs, @mylogs_buggy) { $union_logs{$ele}++ && $isect_logs{$ele}++ }
    foreach (my $u = @union_logs) {
	foreach (my $i = @isect_logs) {
	    if ( $u ne $i ) {
		print "diff succeed\n";
		push(@diff_logs, $u);
	    }
	}
    }

    my (%hsh_excl, @union_arr_excl);
    @hsh_excl{@diff_logs, @myerrs_buggy} = ();
    @union_arr_excl = keys %hsh_excl;
    @union_arr_excl = sort @union_arr_excl;

    print "  After excluding the error 'not existing tree', the remaining of these jobs are:\n";
    foreach (@union_arr_excl) {
        print "    ", $mycluster, "_", $_, "\n";
    }
    print "\nAnalyzed $mylog\n\n";

    # return @union_arr;
    return @union_arr_excl;
}

sub resubmit {
    my $myproc = 0;
    my ($mylog, $opt)= @_;

    my @failures = analyze $mylog;
    my $myjdl = $mylog =~ s/log/jdl/r;
    my $myresubjdl = $myjdl =~ s/\.jdl/_resub.jdl/r;
    my $old = $/;
    local $/ = undef;

    print "Generating $myresubjdl for resubmission ......\n";

    open(my $input_fh, "<", $myjdl) or die "Cannot find $myjdl";
    open(my $output_fh, ">", $myresubjdl)
        or die "Cannot write to $myresubjdl";
    while (my $mycommonlines = <$input_fh>) {
        if ($mycommonlines =~ /(#.*?Job setup.*)\n#.*?Tasks/s) {
            print $output_fh "$1\n";
            last;
        }
    }
    close $input_fh;
    print $output_fh "# Tasks\n";
    close $output_fh;

    foreach (@failures) {
        open($input_fh, "<", $myjdl) or die "Cannot find $myjdl";
        open ($output_fh, '>>', $myresubjdl)
        or die "Cannot append to $myresubjdl";
        my $mymultilines = <$input_fh>;
        # Process number has to be followed by \n
        my $mymatched_str = "#Process.*?$_\n(.*?)queue\n";
        # print $mymultilines;
        if ($mymultilines =~
            /$mymatched_str/s) {
            print $output_fh "\n";
            print $output_fh "#Process $myproc\n";
            print $output_fh "$1";
            print $output_fh "queue\n";
            $myproc = $myproc + 1;
        }
        close $output_fh;
    }
    $/ = $old;
    print "\nFinished generating $myresubjdl\n\n";

    if ($opt eq "--submit") {
        print "Trying to resubmit $myresubjdl ......\n";
        submit($myresubjdl);
        print "Resubmitted done\n\n";
    }
}

my %functions = (
    submit => \&submit,
    analyze => \&analyze,
    resubmit => \&resubmit,
    );
my $function = shift;

if (exists $functions{$function}) {
    $functions{$function}->(shift, shift || 0);
} else {
    print "Please try commands:\n";
    print "\tsubmit\n";
}
