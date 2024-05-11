program LIST_SERVICE{
	version LIST_SERVICE_V1{
		int print(string username<256>, string operation<256>, string date<256>, string file<256>) = 0;
	} = 1;
} = 99;