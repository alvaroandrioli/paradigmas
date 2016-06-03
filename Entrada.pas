programa OlaMundo;
	var result, sum, total: inteiro;
	var var1r, var2r, var3r, var4r: real;

	inicio
		result := 0;
		sum := 0;
		total := 100;

		enquanto result = 0 faca
			inicio
				result := ((sum + 47.2) / total) + total;
			fim;

		se ((result + sum) = 0) entao
			result := ((sum + 47.2) / total) + total;

		senao se total < sum entao
			sum := 0;

		senao
			total := 2*sum;

		repita
			result := result-sum;
		ate (result >= 0);

		escolha

		caso

		 para

	fim.
