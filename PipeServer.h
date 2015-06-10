#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <string>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <functional> 
#include <cctype>
#include <locale>
#include <algorithm>
#define MAX_BUFFER 1024

enum LedState {
	lsOn,
	lsOff
};
enum Color {
	cRed,
	cGreen,
	cBlue,
	cUnk
};

struct LED {
	LED() : state(lsOff), color(cRed), rate(0) { /* Do Nothing */ }
        LedState state;
	Color color;
	int rate;
};

// trim from start
static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
}

class Parser {
public:
	std::vector<std::string> parse(const std::string& aArg) {
		std::vector<std::string> ret;
		int pos = aArg.find(' ');
		const char* p = aArg.c_str();
		const char* b = aArg.c_str()+aArg.size();
		while (pos != std::string::npos) {
			ret.push_back(std::string(p, pos));
			pos+=1;
			p+=pos;
			pos = aArg.find(pos, ' ');
		}
		//std::cout << "aArg:" << aArg  << "'P' " << p << *p << std::endl;
		if (*p && p !=  b)
			ret.push_back(std::string(p, b-p));
		//std::cout << "aArg size " << ret.size() << std::endl;
		
		return ret;
	}
 
};
class Executor {
public:
 	typedef enum {
		cmdSetLedState,
		cmdGetLedState,
		cmdSetLedColor,
		cmdGetLedColor,
		cmdSetLedRate,
		cmdGetLedRate,
		cmdUnk
	} CmdType;
public:
	bool execute(const std::vector<std::string> & aCmd, std::string & out) {
		switch(str2cmd(aCmd[0].c_str())) {
			case cmdSetLedState: return aCmd.size() == 2 && checkSetLedParam(aCmd[1]) ? setLedState(aCmd[1]) : onFail(); break;
			case cmdGetLedState: return aCmd.size() == 1 ? getLedState(out): onFail(); break;
			case cmdSetLedColor: return aCmd.size() == 2 && checkColorParam(aCmd[1]) ? setLedColor(aCmd[1]) : onFail(); break;
			case cmdGetLedColor: return aCmd.size() == 1 ? getLedColor(out) : onFail(); break;
			case cmdSetLedRate: return aCmd.size() == 2 && checkRateParam(aCmd[1]) ? setLedRate(aCmd[1]) : onFail(); break;
			case cmdGetLedRate: return aCmd.size() == 1 ? getLedRate(out) : onFail();  break;
			default: return onFail();
		}
	}
	bool  onFail() { return false; }
protected:
	bool setLedState(const std::string& aParam) { led.state = str2state(aParam); return true; }
	bool setLedColor(const std::string& aParam) { led.color = str2color(aParam); return true; } 
	bool setLedRate(const std::string& aParam) { led.rate = atoi(aParam.c_str()); return true; }
 
	bool getLedState(std::string & out) { out = state2str(led.state); return true;}
	bool getLedColor(std::string & out) { out = color2str(led.color); return true;}
	bool getLedRate(std::string & out) { out = char(led.rate + 0x30); return true;}

	bool checkSetLedParam(const std::string& aParam) { return aParam == "on" || aParam == "off"; }
	bool checkColorParam(const std::string& aParam) { return aParam == "red" || aParam == "green" || aParam == "blue"; }
	bool checkRateParam(const std::string& aParam) { int rate = atoi(aParam.c_str()); return rate >=0 && rate <= 5; }
	
	LedState str2state(const std::string& aStr) {
		if (aStr == "on") return lsOn;
		else return lsOff;
	}
	Color str2color(const std::string& aStr) {
		if (aStr == "red") return cRed;
		else if (aStr == "green") return cGreen;
		else return cBlue;
	}
	const char* state2str(LedState state) {
		switch(state) {
			case lsOn: return "on";
			case lsOff: return "off";
		}
	}
	const char* color2str(Color color) {
		switch(color) {
			case cRed: return "red";
			case cGreen: return "green";
			case cBlue: return "blue";
		}
	}
	CmdType str2cmd(const char* aCmd) {
		static const char* set_led_state_str="set-led-state";
		static const char* get_led_state_str="get-led-state";
		static const char* set_led_color_str="set-led-color";
		static const char* get_led_color_str="get-led-color";
		static const char* set_led_rate_str="set-led-rate";
		static const char* get_led_rate_str="get-led-rate";
		if (strcmp(aCmd, set_led_state_str) == 0) return cmdSetLedState;	
		if (strcmp(aCmd, get_led_state_str) == 0) return cmdGetLedState;	
		if (strcmp(aCmd, set_led_color_str) == 0) return cmdSetLedColor;	
		if (strcmp(aCmd, get_led_color_str) == 0) return cmdGetLedColor;	
		if (strcmp(aCmd, set_led_rate_str) == 0) return cmdSetLedRate;	
		if (strcmp(aCmd, get_led_rate_str) == 0) return cmdGetLedRate;	
	}
protected:	
	LED led;
};
class PipeServer {
public:
	void StartThread() {
		running_ = true;
		pthread_create(&thread_id, NULL, &PipeServer::StartThread_, this);
	}
	void Stop(void) { running_ = false; sleep(2); }
protected:
	pthread_t thread_id;
	bool running_;
	Parser parser_;
	Executor executor_;
protected:
	static void* StartThread_(void* v) {
		PipeServer* pipe_server=(PipeServer*)(v);
		pipe_server->Run();
		return NULL;
	}				
	int Run() {
		int fds,fdc;
    		const char * myfifos = "/tmp/myfifo_server";
    		const char * myfifoc = "/tmp/myfifo_client";
    		/* create the FIFO (named pipe) */
    		mkfifo(myfifos, 0666);
    		mkfifo(myfifoc, 0666);

    		char buf[MAX_BUFFER];
  		//  while (1) {
    		fds = open(myfifos, O_WRONLY);
    	    	fdc = open(myfifoc, O_RDONLY);
    	    	while(running_) {   
 			int count = read(fdc, buf, MAX_BUFFER);
			if (count>0) {
				buf[count] = '\0';
        			//write(fds, buf, strlen(buf));
				std::string ts_(buf, count);
				trim(ts_);
				std::vector<std::string> res = parser_.parse(ts_);//trim(std::string(buf, count)));
				for (int i = 0; i < res.size(); ++i) {
					std::cout << res[i] << std::endl;
				}
				std::string out;
				bool ret = executor_.execute(res, out);
				if (ret) out = "OK " + out;
				else out = "FAILED";
				trim(out);
				write(fds, out.c_str(), out.size()); 
			}
	     	}
        	close(fdc);
        	close(fdc);
    
        	/* remove the FIFO */
    		unlink(myfifos);
    		unlink(myfifoc);
		return 0;
	}
};
