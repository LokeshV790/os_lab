#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

int main() {
    ofstream outfile("input.txt");
    if (!outfile) {
        cerr << "Could not create file" << endl;
        return 1;
    }
    outfile << "This is a parent!" << endl;
    outfile << "This is a child!" << endl;
    outfile << "This is a parent!" << endl;
    outfile.close();
    pid_t pid = getpid();
    pid_t ppid = getppid();
    cout << "PID: " << pid << endl;
    cout << "PPID: " << ppid << endl;
    ifstream infile("input.txt");
    if (!infile) {
        cerr << "Could not read file" << endl;
        return 1;
    }
    string input;
    getline(infile, input);
    infile.close();
    int fd[2];
    if (pipe(fd) == -1) {
        cerr << "Could not create pipes" << endl;
        return 1;
    }

    pid_t child_pid = fork();
    if (child_pid == -1) {
        cerr << "Could not fork process" << endl;
        return 1;
    }

    if (child_pid == 0) { 
        close(fd[1]); 
        string output;
        getline(cin, output); 
        close(fd[0]); 
        cout << "FIFO scheduling result: " << output << endl;
        exit(0);
    } else { 
        close(fd[0]);
        write(fd[1], input.c_str(), input.length() + 1); 
        close(fd[1]);
        wait(NULL); 
        string result;
        getline(cin, result); 
        ofstream outfile("output.txt");
        if (!outfile) {
            cerr << "Could not create file" << endl;
            return 1;
        }
        outfile << result << endl;
        outfile.close();
        cout << "Result written to output.txt" << endl;
    }

    return 0;
}
