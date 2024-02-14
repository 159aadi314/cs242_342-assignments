
# Author: AADI AARYA CHANDRA
# Date and Time: Aug 19,2021 13:00
# Title: AWK Script to sort given data in increasing order according to sum of two fields

{
	name_of_entry[NR-1] = $1;        # declare four arrays: name_of_entry array stores the strings given in first field
	col2_numbers[NR-1] = $2;         # col2_numbers stores numbers in second field
	col3_numbers[NR-1] = $3;         # col3_numbers stores numbers in third field
	sum_array[NR-1] = $2 + $3;       # sum_array stores sum of the two numbers in second and third fields
					  # NR-1 index is for zero indexing of the arrays
}

END{
													# at END, NR value is equal to number of entries in data
	for(it1 = 0 ; it1 < NR ; it1 = it1 + 1)							# SELECTION SORT is used to sort the given data. it1 indicates current index of outer for loop
	{
		id_min = it1;   									# for every iteration of outer for loop, we assume the array is sorted till that index.
													# for the remaining unsorted array we run another for loop to find the minimum element of sum_array
		for(it2 = it1 ; it2 < NR ; it2 = it2 + 1)						# id_min denotes index of the minimum element after it1. it2 indicated current index of inner loop
		{
			if(sum_array[it2] < sum_array[id_min])					# updating the id_min as and when we find a smaller element in sum_array after it1.
			{
				id_min = it2;
			}
			
			else if(sum_array[it2] == sum_array[id_min])					
			{
				if(col2_numbers[it2] < col2_numbers[id_min])				# in case two sums are equal, we sort according to the number in second field
				{
					id_min = it2;
				}
				else if(col2_numbers[it2] == col2_numbers[id_min])			
				{
					if(name_of_entry[it2] < name_of_entry[id_min])		# in case the two numbers are also equal, we sort according to the name field alphabetically
					{
						id_min = it2;
					}
					
				}
				
			}
			
		}
		
		tmp = sum[it1];									# swapping sum_array elements corresponding to it1 and id_min after we have found the minimum sum
		sum_array[it1]=sum_array[id_min];							# in the as yet unsorted array
		sum_array[id_min]=tmp;
		
		tmp1 = name_of_entry[it1];								# swapping name field entries corresponding to it1 and id_min
		name_of_entry[it1]=name_of_entry[id_min];										
		name_of_entry[id_min]=tmp1;
		
		tmp2 = col2_numbers[it1];								# swapping second field entries corresponding to it1 and id_min
		col2_numbers[it1]=col2_numbers[id_min];
		col2_numbers[id_min]=tmp2;
		
		tmp3 = col3_numbers[it1];								# swapping third field entries corresponding to it1 and id_min
		col3_numbers[it1]=col3_numbers[id_min];
		col3_numbers[id_min]=tmp3;
		
		
	}                                                                                            # after the outer for loop is executed, the sorting process is finished
	print "After sorting by sum of 2nd and 3rd fields"
	printf("\n");
	for(it = 0; it < NR ; it = it + 1)								# printing the sorted data
	{
		print name_of_entry[it]"\t"col2_numbers[it]"\t"col3_numbers[it]" "; 
	}
	
}
