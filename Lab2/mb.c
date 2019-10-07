
int i = 0;
int a = 0;

for (; i < 10000; i++){
	
	for (int j = 0; j < 6; j++) {
		if (i % 6 == 0)
			j = 6;
	}
	
	for (int k = 0; k < 7; k++) {
		if (i % 14 == 0)
			k = 7;
	}
}
