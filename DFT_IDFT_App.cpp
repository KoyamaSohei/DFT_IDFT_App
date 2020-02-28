#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

using namespace std;

void help(char* progName) {
	cout << "Usage:" << endl
		 << progName << " [image_path]" << endl;
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		help(argv[0]);
		return 1;
	}
	const string filename = argv[1];
	cout << filename << endl;
	return 0;
}
