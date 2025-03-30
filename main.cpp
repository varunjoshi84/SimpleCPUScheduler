#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

// Structure to hold process details
struct Process {
    int pid, arrival, burst, priority, remaining;
};

// Structure for Gantt chart pieces (timeline segments)
struct GanttPiece {
    int pid, start, end;
};

// Function to display the Gantt chart (visual timeline)
void showGantt(vector<GanttPiece> gantt) {
    cout << "\nHere's the Gantt Chart (like a timeline):\n";
    cout << "|";
    for (int i = 0; i < gantt.size(); i++) {
        cout << " P" << gantt[i].pid << " |";
    }
    cout << "\n0";
    for (int i = 0; i < gantt.size(); i++) {
        cout << "    " << gantt[i].end;
    }
    cout << "\n";
}

// Function to calculate and show waiting and turnaround times
void calculateResults(vector<Process> processes, vector<GanttPiece> gantt) {
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
        cout << "P" << processes[i].pid << ": Waiting Time = " << waiting 
             << ", Turnaround Time = " << turnaround << "\n";
        totalWaiting += waiting;
        totalTurnaround += turnaround;
    }
    cout << "Average Waiting Time = " << totalWaiting / n << "\n";
    cout << "Average Turnaround Time = " << totalTurnaround / n << "\n";
}

// FCFS = First Come First Serve
void doFCFS(vector<Process> processes) {
    vector<GanttPiece> gantt;
    int currentTime = 0;
    cout << "\nRunning FCFS...\n";
    for (int i = 0; i < processes.size(); i++) {
        if (currentTime < processes[i].arrival) {
            currentTime = processes[i].arrival;
        }
        GanttPiece piece;
        piece.pid = processes[i].pid;
        piece.start = currentTime;
        piece.end = currentTime + processes[i].burst;
        gantt.push_back(piece);
        currentTime += processes[i].burst;
    }
    showGantt(gantt);
    calculateResults(processes, gantt);
}

// SJF = Shortest Job First
void doSJF(vector<Process> processes) {
    vector<GanttPiece> gantt;
    int currentTime = 0;
    int done = 0;
    vector<bool> finished(processes.size(), false);
    cout << "\nRunning SJF...\n";
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
            GanttPiece piece;
            piece.pid = processes[shortest].pid;
            piece.start = currentTime;
            piece.end = currentTime + processes[shortest].burst;
            gantt.push_back(piece);
            currentTime += processes[shortest].burst;
            finished[shortest] = true;
            done++;
        }
    }
    showGantt(gantt);
    calculateResults(processes, gantt);
}

// Round Robin
void doRoundRobin(vector<Process> processes, int quantum) {
    vector<GanttPiece> gantt;
    int currentTime = 0;
    int done = 0;
    vector<Process> proc = processes;
    cout << "\nRunning Round Robin with quantum " << quantum << "...\n";
    while (done < processes.size()) {
        bool didSomething = false;
        for (int i = 0; i < proc.size(); i++) {
            if (proc[i].remaining > 0 && proc[i].arrival <= currentTime) {
                didSomething = true;
                int runTime = min(quantum, proc[i].remaining);
                GanttPiece piece;
                piece.pid = proc[i].pid;
                piece.start = currentTime;
                piece.end = currentTime + runTime;
                gantt.push_back(piece);
                currentTime += runTime;
                proc[i].remaining -= runTime;
                if (proc[i].remaining == 0) {
                    done++;
                }
            }
        }
        if (!didSomething) {
            currentTime++;
        }
    }
    showGantt(gantt);
    calculateResults(processes, gantt);
}

// Priority Scheduling
void doPriority(vector<Process> processes) {
    vector<GanttPiece> gantt;
    int currentTime = 0;
    int done = 0;
    vector<bool> finished(processes.size(), false);
    cout << "\nRunning Priority Scheduling...\n";
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
            GanttPiece piece;
            piece.pid = processes[best].pid;
            piece.start = currentTime;
            piece.end = currentTime + processes[best].burst;
            gantt.push_back(piece);
            currentTime += processes[best].burst;
            finished[best] = true;
            done++;
        }
    }
    showGantt(gantt);
    calculateResults(processes, gantt);
}

int main() {
    cout << "Welcome to the CPU Scheduler Simulator!\n";
    int numProcesses;
    cout << "How many processes do you want? ";
    cin >> numProcesses;

    if (cin.fail() || numProcesses <= 0) {
        cout << "Invalid number of processes! Please enter a positive integer.\n";
        return 1;
    }

    vector<Process> processes;
    for (int i = 0; i < numProcesses; i++) {
        int arrival, burst, priority;
        cout << "For Process P" << (i + 1) << ":\n";
        
        cout << "Enter arrival time: ";
        cin >> arrival;
        if (cin.fail() || arrival < 0) {
            cout << "Invalid arrival time! Must be non-negative.\n";
            return 1;
        }

        cout << "Enter burst time: ";
        cin >> burst;
        if (cin.fail() || burst <= 0) {
            cout << "Invalid burst time! Must be positive.\n";
            return 1;
        }

        cout << "Enter priority (higher number = more important): ";
        cin >> priority;
        if (cin.fail()) {
            cout << "Invalid priority! Must be an integer.\n";
            return 1;
        }

        Process p;
        p.pid = i + 1;
        p.arrival = arrival;
        p.burst = burst;
        p.priority = priority;
        p.remaining = burst;
        processes.push_back(p);
    }

    int choice;
    cout << "\nPick a scheduling algorithm:\n";
    cout << "1. FCFS\n2. SJF\n3. Round Robin\n4. Priority\nYour choice: ";
    cin >> choice;

    if (cin.fail()) {
        cout << "Invalid choice! Please enter a number.\n";
        return 1;
    }

    if (choice == 1) {
        doFCFS(processes);
    } else if (choice == 2) {
        doSJF(processes);
    } else if (choice == 3) {
        int quantum;
        cout << "Enter time quantum for Round Robin: ";
        cin >> quantum;
        if (cin.fail() || quantum <= 0) {
            cout << "Invalid quantum! Must be positive.\n";
            return 1;
        }
        doRoundRobin(processes, quantum);
    } else if (choice == 4) {
        doPriority(processes);
    } else {
        cout << "Oops! That's not a valid choice.\n";
    }

    cout << "\nAll done! Hope that helped you see how scheduling works!\n";
    return 0;
<<<<<<< HEAD
}
=======
}
>>>>>>> 18baad5ce1ba2acba3a306bfeb30d2a9feec5b8f
