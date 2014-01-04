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

	static int getYear(double t);
	static int getMonth(double t);
	static int getDay(double t);

	static int getHour(double t);
	static int getMinute(double t);
	static int getSecond(double t);

	static double getDayFraction(double t);
	static double dayFractionToDTime(double t);
	static double len(double t0, double t1);
	static double lenInDays(double t0, double t1);

	static double nextDay(double t) { t+=1.; return normalizeDate(t); }

	static int daysBetween(double t0, double t1);

	static int weekDay(int t);

	static double now();

	static string hoursToStr(double t);
};





#endif /* DTIME_H_ */
