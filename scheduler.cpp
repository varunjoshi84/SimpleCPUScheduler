#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
using namespace std;

struct Process {
    int pid, arrival, burst, priority, remaining;
    Process(int p, int a, int b, int pr, int r) : pid(p), arrival(a), burst(b), priority(pr), remaining(r) {}
};

struct GanttPiece {
    int pid, start, end;
    GanttPiece(int p, int s, int e) : pid(p), start(s), end(e) {}
};

void showGantt(vector<GanttPiece>& gantt, ofstream& out) {
    out << "|";
    for (int i = 0; i < gantt.size(); i++) {
        out << " P" << gantt[i].pid << " |";
    }
    out << "\n0";
    for (int i = 0; i < gantt.size(); i++) {
        out << "    " << gantt[i].end;
    }
    out << "\n";
}

void calculateResults(vector<Process>& processes, vector<GanttPiece>& gantt, ofstream& out) {
    float totalWaiting = 0, totalTurnaround = 0;
    int n = processes.size();
    for (int i = 0; i < n; i++) {
        int finishTime = 0;
        for (int j = 0; j < gantt.size(); j++) {
            if (gantt[j].pid == processes[i].pid && gantt[j].end > finishTime) {
                finishTime = gantt[j].end;
            }
        }
        int turnaround = finishTime - processes[i].arrival;
        int waiting = turnaround - processes[i].burst;
        out << "P" << processes[i].pid << ": Waiting Time = " << waiting 
            << ", Turnaround Time = " << turnaround << "\n";
        totalWaiting += waiting;
        totalTurnaround += turnaround;
    }
    out << "Average Waiting Time = " << totalWaiting / n << "\n";
    out << "Average Turnaround Time = " << totalTurnaround / n << "\n";
}

void doFCFS(vector<Process>& processes, ofstream& out) {
    vector<GanttPiece> gantt;
    int currentTime = 0;
    for (int i = 0; i < processes.size(); i++) {
        if (currentTime < processes[i].arrival) currentTime = processes[i].arrival;
        GanttPiece piece(processes[i].pid, currentTime, currentTime + processes[i].burst);
        gantt.push_back(piece);
        currentTime += processes[i].burst;
    }
    showGantt(gantt, out);
    calculateResults(processes, gantt, out);
}

void doSJF(vector<Process>& processes, ofstream& out) {
    vector<GanttPiece> gantt;
    int currentTime = 0, done = 0;
    vector<bool> finished(processes.size(), false);
    while (done < processes.size()) {
        int shortest = -1;
        int minBurst = 9999;
        for (int i = 0; i < processes.size(); i++) {
            if (!finished[i] && processes[i].arrival <= currentTime && processes[i].burst < minBurst) {
                shortest = i;
                minBurst = processes[i].burst;
            }
        }
        if (shortest == -1) {
            currentTime++;
        } else {
            GanttPiece piece(processes[shortest].pid, currentTime, currentTime + processes[shortest].burst);
            gantt.push_back(piece);
            currentTime += processes[shortest].burst;
            finished[shortest] = true;
            done++;
        }
    }
    showGantt(gantt, out);
    calculateResults(processes, gantt, out);
}

void doRoundRobin(vector<Process>& processes, int quantum, ofstream& out) {
    vector<GanttPiece> gantt;
    int currentTime = 0, done = 0;
    vector<Process> proc = processes;
    while (done < processes.size()) {
        bool didSomething = false;
        for (int i = 0; i < proc.size(); i++) {
            if (proc[i].remaining > 0 && proc[i].arrival <= currentTime) {
                didSomething = true;
                int runTime = min(quantum, proc[i].remaining);
                GanttPiece piece(proc[i].pid, currentTime, currentTime + runTime);
                gantt.push_back(piece);
                currentTime += runTime;
                proc[i].remaining -= runTime;
                if (proc[i].remaining == 0) done++;
            }
        }
        if (!didSomething) currentTime++;
    }
    showGantt(gantt, out);
    calculateResults(processes, gantt, out);
}

void doPriority(vector<Process>& processes, ofstream& out) {
    vector<GanttPiece> gantt;
    int currentTime = 0, done = 0;
    vector<bool> finished(processes.size(), false);
    while (done < processes.size()) {
        int best = -1;
        int maxPriority = -1;
        for (int i = 0; i < processes.size(); i++) {
            if (!finished[i] && processes[i].arrival <= currentTime && processes[i].priority > maxPriority) {
                best = i;
                maxPriority = processes[i].priority;
            }
        }
        if (best == -1) {
            currentTime++;
        } else {
            GanttPiece piece(processes[best].pid, currentTime, currentTime + processes[best].burst);
            gantt.push_back(piece);
            currentTime += processes[best].burst;
            finished[best] = true;
            done++;
        }
    }
    showGantt(gantt, out);
    calculateResults(processes, gantt, out);
}

int main() {
    ifstream inFile("input.txt");
    ofstream outFile("output.txt");
    if (!inFile || !outFile) {
        cerr << "Error opening files!\n";
        return 1;
    }

    int numProcesses, algo, quantum;
    inFile >> numProcesses >> algo >> quantum;
    vector<Process> processes;
    for (int i = 0; i < numProcesses; i++) {
        int arrival, burst, priority;
        inFile >> arrival >> burst >> priority;
        Process p(i + 1, arrival, burst, priority, burst);
        processes.push_back(p);
    }

    switch (algo) {
        case 1: doFCFS(processes, outFile); break;
        case 2: doSJF(processes, outFile); break;
        case 3: doRoundRobin(processes, quantum, outFile); break;
        case 4: doPriority(processes, outFile); break;
        default: outFile << "Invalid algorithm choice!\n";
    }

    inFile.close();
    outFile.close();
    return 0;
}