#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

/*A set of candles of a given length horizon is generated according to the following (simplified) algorithm:*/

/*The open price of the first candlestick is determined or randomly generated in the specified range [minClose, maxClose].*/

/*Randomly, depending on the value of the probability of the next candlestick upCandlesProb (50% by default), the direction of the candlestick is determined. If random () ≤ upCandlesProb, then an up candle (with open ≤ close) will be generated, otherwise a down candle (with open > close) will be generated.*/

/*After determining the direction, the close value of the candlestick is randomly generated and for the candle’s “body” must be |open - close| ≤ maxCandleBody.*/

/*Randomly, depending on the probability of price outliers of the maxOutlier candlestick (default is 3%), the candlestick’s high and low values ​​are generated. If random () ≤ maxOutlier, then a candlestick with price outliers will be generated: the “tails” of the candlestick may turn out to be large enough to simulate real price “outliers” in the market. If the candlestick is without anomalies, then “tails” will be generated in the range of no more than half of the candlestick body.*/

/*The close value of the generated candle becomes the open price of the next candle.*/

/*Then steps 2-5 are repeated until the entire price chain of the specified length horizon is generated.*/

/*All parameters can be set after initializing an instance of the PriceGenerator() class. The result of price generation is saved in the self.prices field in the Pandas DataFrame format and can be used for further analysis.*/

#define HORIZON   250
#define MIN_CLOSE 20.0f
#define MAX_CLOSE 80.0f

typedef struct {
	float open;
	float low;
	float high;
	float close;
} CandleStick;

float float_rand(float min, float max) {
    float scale = rand() / (float) RAND_MAX; /* [0, 1.0] */
    return min + scale * ( max - min );      /* [min, max] */
}

// A function that returns a random float between 0.0, and 1.0
float frandomf() {
    return (float) rand() / (float) RAND_MAX; /* [0, 1.0] */
}

CandleStick generate_candle(float open) {
	CandleStick candle = {0};

	candle.open           = open;
	float max_candle_body = 100.0f;
	float max_outlier     = 0.03f;

	float up_candles_prob = 0.5f;
	bool direction_up     = frandomf() <= up_candles_prob;

	float body_length = frandomf() * max_candle_body;
	if (direction_up) {
		// Generate uptrend candle
		candle.close = candle.open + body_length;
	} else {
		// Generate downtrend candle
		candle.close = candle.open - body_length;
	}

	bool is_outlier = frandomf() <= max_outlier;
	if (is_outlier) {
		// Handle case for outliers
		if (direction_up) {
			candle.high = candle.close + frandomf() * max_candle_body / 2;
			candle.low  = candle.open  - frandomf() * max_candle_body / 2;
		} else {
			candle.high = candle.open  + frandomf() * max_candle_body / 2;
			candle.low  = candle.close - frandomf() * max_candle_body / 2;
		}
	} else {
		// Handle case for normal candles
		if (direction_up) {
			candle.high = candle.close + frandomf() * body_length / 2;
			candle.low  = candle.open  - frandomf() * body_length / 2;
		} else {
			candle.high = candle.open  + frandomf() * body_length / 2;
			candle.low  = candle.close - frandomf() * body_length / 2;
		}
	}

	return candle;
}

int main() {
	CandleStick chart[HORIZON] = {0};
	float       open_first     = float_rand(MIN_CLOSE, MAX_CLOSE);
	float       last_close     = open_first;

	for (int i = 0; i < HORIZON; i++) {
		CandleStick cs = generate_candle(last_close);
		chart[i] = cs;
		last_close = cs.close;
	}

	FILE *fptr;
	fptr = fopen("./candlesticks_.h", "w");
	if (!fptr) {
		fprintf(stderr, "[ERROR]: Could not open file \"out\" to write");
	}

	fprintf(fptr, "#ifndef MARSIM_IMPLEMENTATION" "\n");
	fprintf(fptr, "#include \"marsim.h\"" "\n");
	fprintf(fptr, "#endif" "\n");
	fprintf(fptr, "" "\n");
	fprintf(fptr, "#include \"nob.h\"" "\n");
	fprintf(fptr, "" "\n");
	fprintf(fptr, "const CandleStick candlesticks[250] = {" "\n");

	for (int j = 0; j < HORIZON; j++) {
		fprintf(fptr, "    { %4.3ff, %4.3ff, %4.3ff, %4.3ff },\n", chart[j].open, chart[j].low, chart[j].high, chart[j].close);
	}

	fprintf(fptr, "};" "\n");

	fclose(fptr);
	printf("[INFO]: Process Complete\n");

	return 0;
}
