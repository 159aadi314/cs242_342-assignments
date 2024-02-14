#!/usr/bin/perl                     # start

use Getopt::Long;                   # using modules of cpan and strict for better syntax error checking
use Text::Fuzzy;
use strict;

print "Welcome to spell checker! The text that has been entered in the input.txt file will now be checked for spelling errors. Final draft will be copied into output.txt file!\n";         # welcome message

my $words_dictionary = './words_dictionary.txt';                # declaring variables to handle files
my $linux_dictionary = './linux_dictionary.txt';
my $input = './input.txt';
my $output = './output.txt';

my $max_edit_distance = 2;                  # threshold of max edit distance is set at 2. Can be changed here without affecting rest of the program

open(my $fpd, "<", $words_dictionary) or die "Error: Can't open words dictionary file $words_dictionary: $!";               # files are opened in respective modes (read/write) and their file pointers are assigned
open(my $fpd1, "<", $linux_dictionary) or die "Error: Can't open Linux dictionary file $linux_dictionary: $!";              # if can't open files, we print error message and exit
open(my $fpow, ">", $output) or die "Error: Can't open output file for writing $output: $!";
open(my $fpi, "<", $input) or die "Error: Can't open input file $input: $!";


GetOptions (
    "dict=s" => \$words_dictionary,
    "ldict=s" => \$linux_dictionary,
);

my @dict_array;                                         # declaring an array dict_array to store words of words_dictionary
my $pointer_dict_arr = \@dict_array;                    # declaring an pointer to dict_array
my @linux_dict_array;                                   # declaring an array linux_dict_array to store words of linux_dictionary
my $pointer_linux_dict_arr = \@linux_dict_array;        # declaring an pointer to linux_dict_array
my %dict_hash;                                          # declaring a hash table which maps string to int (if a string is present in one of the 2 above dictionaries, it is mapped to 1, else to 0)
my $pointer_dict_hash = \%dict_hash;                    # declaring an pointer to dict_hash


while (<$fpd>)                      # we go through each line of words_dictionary, which contains a word followed by \n
{
    chomp;                              # remove the trailing newline character
    push(@$pointer_dict_arr, $_);           # we push this word obtained to dict_array and change its hash to 1 in the hash table
    $pointer_dict_hash->{$_} = 1;
}

while (<$fpd1>)                         # we go through each line of linux_dictionary, which contains a linux command/option followed by \n
{
    chomp;                                  # remove trailing newline
    push(@$pointer_linux_dict_arr, $_);     # we push this word obtained to linux_dict_array and change its hash to 1 in the hash table
    $pointer_dict_hash->{$_} = 1;
}

my $count_err=0;                    # variable to store how many errors were detected and how many were corrected
my $count_corr=0;

while(<$fpi>)                             # iterate through each line of input file containing some text to be spell checked
{
    my $line = $.;                          # store the line number in a variable line
    my @words_of_line = split /[^a-z']+/, $_;           # we split the line according to a regular expression and create an array to store all the words present in that line

    for my $word (@words_of_line)                   # then we iterate through each word of this array
    {
        if($dict_hash{$word})                   # if word is found in one of the dictionaries, we print it as it is into the output file
        {
            print $fpow "$word ";               
        }
                                            # current word is stored in $word
        else                                # otherwise we consider it a typo and proceed to provide suggestions for it
        {
            $count_err++;                           # number of errors incremented

            my $tf = Text::Fuzzy->new($word);       # we define a new memeber of class Text::Fuzzy called tf, which references to current word (which is a typo) 
            
            my @words_suggestions;                      # declare an array to store suggestions for correction from words_dictionary
            my $num_w_sugg = 0;                         # and a number to store number of suggestions got from this words_dictionary

            for(my $edit_it = 1 ; $edit_it <= $max_edit_distance ; $edit_it++ )      # for each edit distance from 1 to the threshold we find the words in the words_dictionary at that edit distance from current word (which is a typo)
            {
                for my $dict_word (@dict_array)                             # we iterate through each word in words_dictionary
                {
                    if($tf->distance($dict_word) == $edit_it)               # and calculate its edit distance from tf (ie typo word)
                    {                                                         # if it is equal to edit_it, we push it into the suggestions array
                        push(@words_suggestions, $dict_word);
                        $num_w_sugg++;              
                    }
                }

            }

            my @linux_suggestions;                  # we repeat the procedure as above to get suggestions from linux_dictionary
            my $num_l_sugg = 0;

            for(my $edit_it = 1 ; $edit_it <= $max_edit_distance ; $edit_it++ )         # iterate through edit distances and find words at that distance from typo
            {
                for my $linux_dict_word (@linux_dict_array)
                {
                    if($tf->distance($linux_dict_word) == $edit_it)                 # push these words into linux_suggestions array if they are at correct edit distance
                    {
                        push(@linux_suggestions, $linux_dict_word);
                        $num_l_sugg++;
                    }
                }
            }

            for my $past_word (@words_of_line)              # then we print all the words of that line upto that typo word
            {
                if($past_word ne $word)
                {
                    print "$past_word ";
                }
                else                            # break the loop as soon as the typo word is encountered
                {
                    print "\n";
                    last;
                }
            }

            print "Line No. $line .: $word\n";          # we print the typo along with its line number

            if($num_w_sugg)                              # then if there are any suggestions from words_dictionary, we print them in a numbered list
            {
                print "                 ^ Did you mean: \n";
            }

            for(my $counter = 1 ; $counter < $num_w_sugg+1 ; $counter++ )               # printing suggestions from words_dictionary
            {
            	print "                   $counter. $words_suggestions[$counter-1]\n";
            }

            print "\n";

            if($num_l_sugg)                                 # then if any suggestions of linux commands, we print them
            {
                print "                 ^ Did you mean one of the below Linux commands/options: \n";
            }

            for(my $counter = 1 ; $counter < $num_l_sugg+1 ; $counter++ )               # printing linux command suggestions
            {
                print "                   $counter. $linux_suggestions[$counter-1]\n";
            }

            print "\n";     
            print "Press 0 to select one of the words suggestions and 1 for one of the linux commands suggestions (if one of them is not visible, still follow this rule), then press ENTER, then press any key corresponding to the suggestion number to correct the word to that suggestion, then press ENTER again. Otherwise, press any other key to leave it unchanged.\n";
                                            # self-explanatory prompt

            my $response1 = <STDIN>;                    # response 1 is 0 if words_dicitonary is selected, 1 if linux dictionary, anything else means no change should be made to word
            chomp $response1;
            
            if($response1 == 0)                     # response 1 is 0 : selecting a suggestion from words_dictionary
            {
                my $response2 = <STDIN>;                # read another number response 2 from user
                chomp $response2;

                if ($response2 < $num_w_sugg+1 && $response2 >= 1)          # if response2 corresponds to one of the words_dictionary suggestions
                {
                    for my $past_word (@words_of_line)                      # then we search for the typo word in the current line of input file
                    {
                        if($past_word ne $word)
                        {
                            next;
                        }

                        else    
                        {
                            $past_word = "$words_suggestions[$response2-1]";            # we change that word to the suggestion denoted by response2
                            print $fpow "$past_word ";                                  # then print it to output file
                            last;
                        }
                    }

                    $count_corr++;                                                      # then increment number of corrections made
                }

                else                                                            # else response 2 is invalid, we print error message, and make no correction
                {
                    print "Invalid second number. No correction made\n";
                    print $fpow "$word ";
                }

            }
            elsif($response1 == 1)                                  # response 1 is 1 : selecting a suggestion from linux_dictionary
            {
                my $response2 = <STDIN>;                                 # read another number response 2 from user
                chomp $response2;

                if ($response2 < $num_l_sugg+1 && $response2 >= 1)          # if response2 corresponds to one of the linux_dictionary suggestions
                {
                    for my $past_word (@words_of_line)                      # then we search for the typo word in the current line of input file
                    {
                        if($past_word ne $word)
                        {
                            next;
                        }

                        else
                        {
                            $past_word = "$linux_suggestions[$response2-1]";            # we change that word to the suggestion denoted by response2
                            print $fpow "$past_word ";                                 # then print it to output file
                            last;
                        }
                    }

                    $count_corr++;                                                  # then increment number of corrections made
                }

                else
                {
                    print "Invalid second number. No correction made\n";            # else response 2 is invalid, we print error message, and make no correction
                    print $fpow "$word ";
                }
            }
            else                                            # if response 1 is anything except 0 or 1, we make no correction
            {   
                print $fpow "$word ";                           # and we simply print word as it is output file
            }

        }
    }

    print $fpow "\n";    
}

print "Spell checking successful! No. of errors found: $count_err, No. of corrections made: $count_corr.\n";         # final message to depict number of errors detected and corrected

close(fpi);                                                 # closing all the declared file pointers
close(fpow);
close(fpd1);            
close(fpd);