//  copyright (C) 2012-2013 Julian Wergieluk  <julian@wergieluk.com>

#ifndef IO_H_
#define IO_H_

#include <string>
#include <vector>
#include <cstdio>

using namespace std;


class Io {
public:

	static bool file2vs(const string& path, vector<string>& vs) {
		FILE* in;
		if ((in = fopen(path.c_str(), "r"))==NULL) return false;

		char buf[5000*20], c;
		size_t i = 0;

		while( (c = fgetc(in)) != EOF ) {
			if(c=='\r') continue;
			if(c == '\n') {
				buf[i] = '\0';
				i = 0;
				vs.push_back(string(buf));
			} else {
				if( i < sizeof(buf) - 1) {
					buf[i++] = c;
				} else {
					fclose(in);
					return false;
				}
			}
		}

		if(i > 0) {
			buf[i] = '\0';
			vs.push_back(string(buf));
		}

		fclose(in);
		return true;
	}

	static string trim(const string& s) {
		int head=0; 	int tail=s.size()-1;

		while ( head<=tail && (
				s[head]=='\r' ||
				s[head]=='\t' ||
				s[head]=='\n' ||
				s[head]==' '))
		{ head++; }

		while ( head<=tail && (
				s[tail]=='\r' ||
				s[tail]=='\t' ||
				s[tail]=='\n' ||
				s[tail]==' '))
		{ tail--; }

		if( head>tail) {
			return string("");
		} else {
			return s.substr(head, tail-head+1);
		}
	}

	static bool s2d_(double& ret, const string& s) {
		char *pEnd;
		string buf=trim(s);
		if( buf.size()==0 ) return false;
		ret = strtod(buf.c_str(), &pEnd);
		if (pEnd[0]!='\0') return false;
		return true;
	}

	static vector<string> s2vs(string str, const string& delim=" ") {
		vector<string> ret;

		int cutAt;
		while( (cutAt = str.find(delim)) != str.npos ) {
				ret.push_back(str.substr(0,cutAt));
				str = str.substr(cutAt + delim.length());
		}
		ret.push_back(str);

		return ret;
	}


};

#endif /* IO_H_ */
