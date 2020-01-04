#0016046_hw2_2
#Perl
#Fibonacci

sub fib
{   
    if($_[0]==0)
    {
        $h{"fib($_[0])"}=0;
        return $h{"fib($_[0])"};
    }
    elsif($_[0]==1)
    {
        $h{"fib($_[0])"}=1;
        return $h{"fib($_[0])"};
    }
    else
    {
        if(!exists $h{"fib($_[0])"})
        {
            $h{"fib($_[0])"}=fib($_[0]-1)+fib($_[0]-2);
        }
        return $h{"fib($_[0])"};
    }
}

do{
    print "Enter an integer>=0:";
    $a=<>;
    chomp $a;
    %h=();    #clear hash
    fib($a);
    print "fib(",$a,")=",fib($a),"\n";
    print "Hash table created so far\n";
    while(($key,$value)=each %h)
    {
       print "$key","=>","$value\n";
    }
}while(1)



