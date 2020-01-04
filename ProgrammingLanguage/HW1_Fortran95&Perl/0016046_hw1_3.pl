#0016046_hw2_1
#Perl
#bubble sort: smaller -> larger

sub bsort
{   
    for(my $i=0;$i<$#_;$i++)
    {
        for(my $j=0;$j<$#_-$i;$j++)
        {   
            if($_[$j]>$_[$j+1])
            {  
               my $temp=$_[$j];  #swap
               $_[$j]=$_[$j+1];
               $_[$j+1]=$temp;
            }
        }
    }
}

print "Enter integers in a line:";
@a=split ' ',<>;   #split input line by ' ' into array
chomp @a;          #remove "\n"
bsort(@a);
print "After sorting: @a\n";
