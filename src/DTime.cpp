//  copyright (C) 2013-2013 Julian Wergieluk  <julian@wergieluk.com>

#include "DTime.h"
#include <ctime>
#include <cmath>
#include "io.h"

#include "catch.hpp"

bool DTime::fromString(const string& s, double& d) {
	if( s.length()< 16 ) return false;

	string date;
	bool comma_placed=false;

	for(auto i=0; i<s.length(); i++) {
		if( s[i]=='0' || s[i]=='1' || s[i]=='2' || s[i]=='3' || s[i]=='4' || s[i]=='5' || s[i]=='6' || s[i]=='7' || s[i]=='8' ||s[i]=='9') {
			date += s[i];
		} else if( s[i]==' ' && !comma_placed ) {
			date += '.';
			comma_placed=true;
		}
	}

	if( !Io::s2d_(d, date)) return false;
	d=normalize(d);
	return true;
}

double DTime::normalize(double t) {
	double time = normalizeTime(t-floor(t));
	return normalizeDate( (int)(floor(t)) )+ time;
}

double DTime::normalizeTime(double t) {
	double days, hours, minutes, seconds;
	getTimeComponents(t, days, hours, minutes, seconds);

	if(seconds>=60.) {
		seconds-=60.;
		minutes+=1.;
	}
	if(minutes>=60.) {
		minutes-=60.;
		hours+=1.;
	}
	while(hours>=24.) {
		hours-=24.;
		days+=1.;
	}

	double r=days + 0.01*floor(hours) + 0.0001*floor(minutes) + 0.000001*floor(seconds);
//	printf("t %f  d %f  h %f  m %f s %f   ->  %f\n", t, days, hours, minutes, seconds,r);
	return r;
}

int DTime::normalizeDate(int t) {
	int y, m, d;

	y = t / 10000;
	m = (t % 10000)/100;
	d = (t % 100 );

//		printf("# %d   %d %d %d ", t, y,m,d);

	y += ((m-1)/12);
	m = ((m-1) % 12)+1;

	while ( d > daysInMonth(y, m) ) {
		d -= daysInMonth(y, m);
		m++;
		if( m>12) {
			y++;
			m=1;
		}
	}

//		printf("  normalized to  %d %d %d\n", y, m, d);
	return  10000*y + 100*m + d;
}

int DTime::daysInMonth(int year, int month) {
	if ( month==1 ) return 31;
	if ( month==2 ) {   // annus bissextus   (latin for pain in the ass)   http://de.wikipedia.org/wiki/Schaltjahr#Gregorianischer_Kalender
		if( year % 400 == 0 ) return 29;
		if( year % 100 == 0 ) return 28;
		if( year % 4 == 0 )   return 29;
		return 28;
	}
	if ( month==3 ) return 31;
	if ( month==4 ) return 30;
	if ( month==5 ) return 31;
	if ( month==6 ) return 30;
	if ( month==7 ) return 31;
	if ( month==8 ) return 31;
	if ( month==9 ) return 30;
	if ( month==10 ) return 31;
	if ( month==11 ) return 30;
	if ( month==12 ) return 31;
	return 0;
}

void DTime::getTimeComponents(double t, double& days, double& hours, double& minutes, double& seconds) {
	days = floor(t);
	double d = 100.*(t-days)+0.000001;
	hours= d;
	d = 100.*(d-floor(d))+0.0001;
	minutes= d;
	d = 100.*(d-floor(d))+0.01;
	seconds= d;
//	printf("gtc: %f = d%f  h%f m%f s%f\n", t, days, hours, minutes, seconds);
}

void DTime::getTimeComponents(double t, int& days, int& hours, int& minutes, int& seconds) {
	days = floor(t);
	double d = 100.*(t-days)+0.000001;
	hours= (int)floor(d);
	d = 100.*(d-floor(d))+0.0001;
	minutes= (int)floor(d);
	d = 100.*(d-floor(d))+0.01;
	seconds= (int)floor(d);
//	printf("gtc: %f = d%d  h%d m%d s%d\n", t, days, hours, minutes, seconds);
}

int DTime::getYear(double t) { return (  (int)floor(t)   )/10000; }
int DTime::getMonth(double t) { return ((   (int)floor(t)  ) % 10000)/100; }
int DTime::getDay(double t) { return (int)floor(t) % 100; }

int DTime::getHour(double t) {
	double days, hours, minutes, seconds;
	getTimeComponents(t, days, hours, minutes, seconds);
	return hours;
}
int DTime::getMinute(double t) {
	double days, hours, minutes, seconds;
	getTimeComponents(t, days, hours, minutes, seconds);
	return minutes;
}
int DTime::getSecond(double t) {
	double days, hours, minutes, seconds;
	getTimeComponents(t, days, hours, minutes, seconds);
	return seconds;
}

double DTime::getDayFraction(double t) {
	int d0, h0, m0, s0;
	getTimeComponents(t, d0, h0, m0, s0);
	return h0/24. + m0/(24.*60.) + s0/(24.*60.*60.);
}

double DTime::dayFractionToDTime(double t) {
	double day= floor(t-floor(t));
	t = 24.*(t-floor(t));
	double hour= floor(t);
	t = 60.*(t-floor(t))+0.0001;
	double minute=floor(t);
	t = 60.*(t-floor(t))+0.01;
	double second=floor(t);
//	printf("dftdt %f %f %f %f : %f\n", day, hour, minute, second, day + 0.01*hour + 0.0001*minute + 0.000001*second);
	return day + 0.01*hour + 0.0001*minute + 0.000001*second;
}

double DTime::len(double t0, double t1) {
	int days= daysBetween(t0, t1);
	double f= getDayFraction(t1) - getDayFraction(t0);

//	printf("len: [%f %f]  frac [%f %f]  db %d  f %f    len %f\n", t0, t1, getDayFraction(t0), getDayFraction(t1), days, f, days + dayFractionToDTime(f));
	return days + dayFractionToDTime(f);
}

double DTime::lenInDays(double t0, double t1) {
//	printf("lenInDays: %f %f \n", getDayFraction(t0), getDayFraction(t1));
	return (double)daysBetween(t0, t1) + getDayFraction(t1) - getDayFraction(t0);
}


int DTime::daysBetween(double t0, double t1) {
	int d0=(int)floor(t0);
	int d1=(int)floor(t1);
	int diff=0;
	while(d0<d1) {	// TODO optimize
		d0=nextDay(d0);
		diff++;
	}
	return diff;
}

int DTime::weekDay(int t) {
	// http://en.wikipedia.org/wiki/Zeller%27s_congruence
	int y = getYear(t);
	int m = getMonth(t);
	int d = getDay(t);

	if(m==1) { m=13; y--; }
	if(m==2) { m=14; y--; }

	int j = y / 100;
	int k = y % 100;

	int wd =  d + 13*(m+1)/5 + k + k/4 + j/4 + 5*j;
	wd = wd % 7;  // 0 = Saturday, 1 = Sunday, 2 = Monday, ...
	return 1 + ((wd+5)%7);   // return ISO day-of-the week
}

double DTime::now() {
	time_t t;
	time(&t);
	struct tm * ti;
	ti = localtime(&t);
	return  0.000001*ti->tm_sec + 0.0001*ti->tm_min + 0.01*ti->tm_hour +
			ti->tm_mday + 100.*(ti->tm_mon+1.) + 10000.*ti->tm_year + 19000000.;
}

string DTime::hoursToStr(double t) {
	char buf[500];
	snprintf(buf, 500, "%.0f:%02.0f", floor(t), floor((t-floor(t))*60)  );
	return string(buf);
}


TEST_CASE( "Normalizations", "[DTime]") {
	REQUIRE( DTime::normalize(20131316.2466) == 20140117.0106);
	REQUIRE( DTime::normalize(20131316.235959) == 20140116.235959);
}

