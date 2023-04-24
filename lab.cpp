#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

int main() {
    // Create a file
    ofstream outfile("input.txt");
    if (!outfile) {
        cerr << "Error: Could not create file" << endl;
        return 1;
    }
    outfile << "Hello, world!" << endl;
    outfile.close();

    // Generate pid and ppid of the process
    pid_t pid = getpid();
    pid_t ppid = getppid();
    cout << "PID: " << pid << endl;
    cout << "PPID: " << ppid << endl;

    // Read the input file
    ifstream infile("input.txt");
    if (!infile) {
        cerr << "Error: Could not read file" << endl;
        return 1;
    }
    string input;
    getline(infile, input);
    infile.close();

    // Create pipes for communication
    int fd[2];
    if (pipe(fd) == -1) {
        cerr << "Error: Could not create pipes" << endl;
        return 1;
    }

    // Fork a child process
    pid_t child_pid = fork();
    if (child_pid == -1) {
        cerr << "Error: Could not fork process" << endl;
        return 1;
    }

    if (child_pid == 0) { // Child process
        close(fd[1]); // Close write end of pipe
        string output;
        getline(cin, output); // Get input from parent process
        close(fd[0]); // Close read end of pipe
        cout << "FIFO scheduling result: " << output << endl;
        exit(0);
    } else { // Parent process
        close(fd[0]); // Close read end of pipe
        write(fd[1], input.c_str(), input.length() + 1); // Write input to child process
        close(fd[1]); // Close write end of pipe
        wait(NULL); // Wait for child process to finish
        string result;
        getline(cin, result); // Get result from child process
        ofstream outfile("output.txt");
        if (!outfile) {
            cerr << "Error: Could not create file" << endl;
            return 1;
        }
        outfile << result << endl;
        outfile.close();
        cout << "Result written to output.txt" << endl;
    }

    return 0;
}
