if (@ARGV) {
  open(DATA,@ARGV[0]) || die "Can't open file " . $_;
   
  @filename = split('-',@ARGV[0]);
  print "\n" . @filename[1] . "\n";
  $move = "";
  $time = 0;
  $total_time = 0;
  $num_correct = 0;
  $tests = 1;
  $test_name = "";
  %names = ( 'arasan' => 'arasan21',
             'ECM' => 'ecmgcp',
             'WAC' => 'wacnew',
             'IQ' => 'iq4',
             'PET' => 'pet',
             'E_E_T' => 'eet',
	     'LCT' => 'lapuce2',
	     'test' => 'bt2630');
  while (<DATA>)
  {
     if (/^\"/)
     {
        if ($tests) {
           my @vals = split;
           $id = @vals[0];
           while (($key,$value) = each(%names)) {
             if ($id =~ /$key/) {
   		$test_name = $value;
   		break;
             }
   	  }
	  $tests = 0;
       }
     }
     elsif (m/correct :/) {
	my @vals = split;
        $tests = 1;
	print $test_name . "\t" . @vals[2];
	if ($test_name eq "bt2630") {
         print "\trating = " . (2630 - ((900*(30-$num_correct)) + $total_time)/30);
	}
	elsif ($test_name eq "lapuce2") {
          print "\trating = " . (1900 + $total_score);
	}
	$num_correct = 0;
	$total_time = 0;
	print "\n";
     }
     elsif ($test_name eq "lapuce2") {
        if (/^[0-9]+\t*/)
        {
           split;
           $newtime = @_[1];
           $newmove = @_[2];
           $newmove =~ s/!//;
           $newmove =~ s/\?//;
           if ($newmove ne $move)
           {
             $move = $newmove;
             $time = $newtime;
           }
        }
        elsif (/^result:/)
        {
           if (m/\+\+ solved/)
           {
              $num_correct++;
              $score = 0;
              if ($time <= 9)
              {
                $score = 30;
              }
              elsif ($time <= 29)
              {
                $score = 25;
              }
              elsif ($time <= 89)
              {
                $score = 20;
              }
              elsif ($time <= 179)
              {
                $score = 15;
              }
              elsif ($time <= 389)
              {
                $score = 10;
              }
              else
              {
                $score = 5;
              }
              $total_score += $score;
              
              $time = 0;
              $move = "";
           }
        }
     }
     elsif ($test_name eq "bt2630") {
        if (/^[0-9]+\t*/)
        {
           split;
           $newtime = @_[1];
           $newmove = @_[2];
           $newmove =~ s/!//;
           $newmove =~ s/\?//;
           if ($newmove ne $move)
           {
             $move = $newmove;
             $time = $newtime;
           }
        }
        elsif (/^result:/)
        {
           if (m/\+\+ solved/)
           {
              $num_correct++;
     #         print "correct - time = " . $time . "\n";
              $total_time += $time;
              $time = 0;
              $move = "";
           }
        }
    }
  }
}
