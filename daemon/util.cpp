#include "util.h"

using namespace std;

void notify(const string &title, const string &message) {
    char cmd[1024];
    sprintf(cmd, "notify-send -i keyboard -u normal '%s' '%s' -t 3000",
	    title.c_str(), message.c_str());
    system(cmd);
}

vector<string> split(const string &source, const char *delimiter, bool keepEmpty) {
    vector<string> results;

    size_t prev = 0;
    size_t next = 0;

    while ((next = source.find_first_of(delimiter, prev)) != string::npos) {
        if (keepEmpty || (next - prev != 0)) {
            results.push_back(source.substr(prev, next - prev));
        }
        prev = next + 1;
    }

    if (prev < source.size()) {
        results.push_back(source.substr(prev));
    }

    return results;
}
