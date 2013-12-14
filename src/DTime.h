//  copyright (C) 2013-2013 Julian Wergieluk  <julian@wergieluk.com>

#ifndef DTIME_H_
#define DTIME_H_

using namespace std;
#include <string>

class DTime {
public:

	static bool fromString(const string& s, double& d);
	static double normalize(double t);
	static double normalizeTime(double t);
	static int normalizeDate(int t);
	static int daysInMonth(int year, int month);

	static void getTimeComponents(double t, double& days, double& hours, double& minutes, double& seconds);
	static void getTimeComponents(double t, int& days, int& hours, int& minutes, int& seconds);

//	static int getYear(int t) { return t /10000; }
//	static int getMonth(int t) { return (t % 10000)/100; }
//	static int getDay(int t) { return t % 100; }

	static int getHour(double t);
	static int getMinute(double t);
	static int getSecond(double t);

	static double getDayFraction(double t);
	static double dayFractionToDTime(double t);
	static double len(double t0, double t1);
	static double lenInDays(double t0, double t1);

	static double nextDay(double t) { t+=1.; return normalizeDate(t); }

	static int daysBetween(double t0, double t1);

//	static int weekDay(int t) {
//		// http://en.wikipedia.org/wiki/Zeller%27s_congruence
//		int y = getYear(t);
//		int m = getMonth(t);
//		int d = getDay(t);
//
//		if(m==1) { m=13; y--; }
//		if(m==2) { m=14; y--; }
//
//		int j = y / 100;
//		int k = y % 100;
//
//		int wd =  d + 13*(m+1)/5 + k + k/4 + j/4 + 5*j;
//		wd = wd % 7;  // 0 = Saturday, 1 = Sunday, 2 = Monday, ...
//		return 1 + ((wd+5)%7);   // return ISO day-of-the week
//	}

	static double now();

	static void test();
};





#endif /* DTIME_H_ */
