# scoring script for PGN files
# prints summary statistics given a game file (one opponent
# is Arasan).
if (@ARGV) {
  open(DATA,@ARGV[0]) || die "Can't open file " . $_;
  $last_opponent = "";
  $win = 0;
  $lose = 0;
  $draw = 0;
  $tmp_win=0;
  $tmp_loss=0;
  $tmp_draw=0;
  $count=0;
  while (<DATA>)
  {
     if ($_ =~ m/\]/)
     {
        @arr = split(" ",$_,2);
        $key = substr(@arr[0],1);
        $value = @arr[1];
        $value =~ s/\]//;
        @values = split("\"",$value);
        $value = @values[1];
        if ($key eq "White")
        {
	    $white=$value;
        }
        if ($key eq "Black")
        {
	    $black=$value;
        }
        if ($key eq "Result") {
          if ($value ne "\*") {
            if ($white =~ m/Arasan v/) {
                $opponent = $black;
	    }
            else {
                $opponent = $white;
            }
	  }
          if ($count == 0) {$last_opponent = $opponent;}
        }
        if ($opponent ne $last_opponent) {
          print $last_opponent . "\t : ";
          print "\+" . ($win-$tmp_win) . " \-" . ($loss-$tmp_loss) . " \=" . ($draw-$tmp_draw) . " " . ($win+$loss+$draw-$tmp_win-$tmp_loss-$tmp_draw) . " total\t" . 100.0*($win - $tmp_win + (($draw-$tmp_draw)/2))/($win+$loss+$draw-$tmp_win-$tmp_loss-$tmp_draw) . "%\n";
          $tmp_win = $win; $tmp_loss = $loss; $tmp_draw = $draw;
          $last_opponent = $opponent;
        } 
        if ($key eq "Result") {
          if ($value ne "\*") {
            if ($white =~ m/Arasan v/) {
		if ($value eq "1-0") {
		    $win++;
		}
                elsif ($value eq "0-1") {
                    $loss++;
		}
                else {
                    $draw++;
		}
	    }
            else {
		if ($value eq "1-0") {
		    $loss++;
		}
                elsif ($value eq "0-1") {
                    $win++;
		}
                else {
                    $draw++;
		}
            }
	  }
          $count++;
        }
     }
  }
  print $last_opponent . "\t : ";
  print "\+" . ($win-$tmp_win) . " \-" . ($loss-$tmp_loss) . " \=" . ($draw-$tmp_draw) . " " . ($win+$loss+$draw-$tmp_win-$tmp_loss-$tmp_draw) . " total\t" . 100.0*($win-$tmp_win + (($draw-$tmp_draw)/2))/($win-$tmp_win+$loss-$tmp_loss+$draw-$tmp_draw) . "%\n";
}
