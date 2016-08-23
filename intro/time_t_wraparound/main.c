#include <stdio.h>
#include <time.h>
#include <limits.h>
#include <math.h>

int main(int argc, char** argv) {
	time_t now = time(NULL);	
	time_t endOfTime = LONG_MAX;
	double daysInMonthArray[] = {31.0, 28.0, 31.0, 30.0, 31.0, 30.0, 31.0, 31.0, 30.0, 31.0, 30.0, 31.0};



	
	struct tm* nowGM = gmtime(&now);
	double diffNowEndOfTime = difftime(endOfTime, now);	
	double secondsPerDay = 60.0 * 60.0 * 60.0 * 24.0;
	double secondsPerYear = secondsPerDay * 365.0;
	double diffNowEndOfTimeYearsQuotiant = diffNowEndOfTime / secondsPerYear;

	double diffNowEndOfTimeYears = floor(diffNowEndOfTimeYearsQuotiant);
	double diffNowEndOfTimeYearsRemainder = diffNowEndOfTimeYearsQuotiant - diffNowEndOfTimeYears;

	double months = diffNowEndOfTimeYearsRemainder * 12.0;	
	double monthsRemainder = months - floor(months);
	int monthsInt = (int)months;	


	double days = monthsRemainder * 31.0; //safe default!

	if ( monthsInt < 12.0 ) {
		days = monthsRemainder * daysInMonthArray[monthsInt]; //I know it's october as of the time of writing this! we better would be a lookup table
	}	
	
	double daysRemainder = days - floor(days);

	double hours = daysRemainder * 24.0;
	double hoursRemainder = hours - floor(hours);

	double minutes = hoursRemainder * 60.0;




	
	printf("Years left of UNIX demon dogging: %ld\n", (long)diffNowEndOfTimeYears);	
	printf("Months: %d\n", monthsInt);
	printf("Days: %d\n", (int)days);
	printf("Hours: %d\n", (int)hours);
	printf("Minutes: %d\n", (int)minutes);

	
	

	return 0;
}