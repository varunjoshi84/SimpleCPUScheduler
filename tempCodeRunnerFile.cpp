#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

// Structure to hold process details
struct Process {
    int pid, arrival, burst, priority, remaining; // pid = process ID, arrival = when it arrives, burst = how long it runs
};

// Structure for Gantt chart pieces (timeline segments)
struct GanttPiece {
    int pid, start, end; // pid = which process, start = when it begins, end = when it finishes
};

// Function to display the Gantt chart (visual timeline)
void showGantt(vector<GanttPiece> gantt) {
    cout << "\nHere’s the Gantt Chart (like a timeline):\n";
    cout << "|";
    for (int i = 0; i < gantt.size(); i++) { // Loop through all pieces
        cout << " P" << gantt[i].pid << " |"; // Show process ID
    }
    cout << "\n0"; // Start time is 0
    for (int i = 0; i < gantt.size(); i++) {
        cout << "    " << gantt[i].end; // Show end times below
    }
    cout << "\n";
}

// Function to calculate and show waiting and turnaround times
void calculateResults(vector<Process> processes, vector<GanttPiece> gantt) {
    float totalWaiting = 0, totalTurnaround = 0; // Keep track of totals
    int n = processes.size(); // Number of processes
    for (int i = 0; i < n; i++) { // Loop through each process
        int finishTime = 0;
        for (int j = 0; j < gantt.size(); j++) { // Find when this process finishes
            if (gantt[j].pid == processes[i].pid && gantt[j].end > finishTime) {
                finishTime = gantt[j].end;
            }
        }
        int turnaround = finishTime - processes[i].arrival; // Time from arrival to finish
        int waiting = turnaround - processes[i].burst; // Time waiting (not running)
        cout << "P" << processes[i].pid << ": Waiting Time = " << waiting 
             << ", Turnaround Time = " << turnaround << "\n";
        totalWaiting += waiting;
        totalTurnaround += turnaround;
    }
    cout << "Average Waiting Time = " << totalWaiting / n << "\n"; // Average for all processes
    cout << "Average Turnaround Time = " << totalTurnaround / n << "\n";
}

// FCFS = First Come First Serve (whoever arrives first runs first)
void doFCFS(vector<Process> processes) {
    vector<GanttPiece> gantt; // Timeline storage
    int currentTime = 0; // Current time in simulation
    cout << "\nRunning FCFS...\n";
    for (int i = 0; i < processes.size(); i++) {
        if (currentTime < processes[i].arrival) { // Wait if process hasn’t arrived
            currentTime = processes[i].arrival;
        }
        gantt.push_back({processes[i].pid, currentTime, currentTime + processes[i].burst}); // Add to timeline
        currentTime += processes[i].burst; // Move time forward
    }
    showGantt(gantt);
    calculateResults(processes, gantt);
}

// SJF = Shortest Job First (run the quickest job available)
void doSJF(vector<Process> processes) {
    vector<GanttPiece> gantt;
    int currentTime = 0;
    int done = 0; // Count finished processes
    vector<bool> finished(processes.size(), false); // Track which are done
    cout << "\nRunning SJF...\n";
    while (done < processes.size()) { // Keep going until all are done
        int shortest = -1; // Index of shortest job
        int minBurst = 9999; // Big number to find smallest burst
        for (int i = 0; i < processes.size(); i++) {
            if (!finished[i] && processes[i].arrival <= currentTime && processes[i].burst < minBurst) {
                shortest = i;
                minBurst = processes[i].burst;
            }
        }
        if (shortest == -1) { // No job ready yet
            currentTime++;
        } else {
            gantt.push_back({processes[shortest].pid, currentTime, currentTime + processes[shortest].burst});
            currentTime += processes[shortest].burst;
            finished[shortest] = true;
            done++;
        }
    }
    showGantt(gantt);
    calculateResults(processes, gantt);
}

// Round Robin = Everyone gets a turn with a time limit (quantum)
void doRoundRobin(vector<Process> processes, int quantum) {
    vector<GanttPiece> gantt;
    int currentTime = 0;
    int done = 0;
    vector<Process> proc = processes; // Copy processes to modify remaining time
    cout << "\nRunning Round Robin with quantum " << quantum << "...\n";
    while (done < processes.size()) { // Until all are finished
        bool didSomething = false; // Check if we did any work
        for (int i = 0; i < proc.size(); i++) {
            if (proc[i].remaining > 0 && proc[i].arrival <= currentTime) { // If process isn’t done and is ready
                didSomething = true;
                int runTime = min(quantum, proc[i].remaining); // Run for quantum or remaining time
                gantt.push_back({proc[i].pid, currentTime, currentTime + runTime});
                currentTime += runTime;
                proc[i].remaining -= runTime; // Reduce remaining time
                if (proc[i].remaining == 0) { // If done
                    done++;
                }
            }
        }
        if (!didSomething) { // If nothing to do, move time forward
            currentTime++;
        }
    }
    showGantt(gantt);
    calculateResults(processes, gantt);
}

// Priority = Highest priority runs first
void doPriority(vector<Process> processes) {
    vector<GanttPiece> gantt;
    int currentTime = 0;
    int done = 0;
    vector<bool> finished(processes.size(), false);
    cout << "\nRunning Priority Scheduling...\n";
    while (done < processes.size()) {
        int best = -1; // Index of highest priority
        int maxPriority = -1; // Track highest priority found
        for (int i = 0; i < processes.size(); i++) {
            if (!finished[i] && processes[i].arrival <= currentTime && processes[i].priority > maxPriority) {
                best = i;
                maxPriority = processes[i].priority;
            }
        }
        if (best == -1) { // No process ready
            currentTime++;
        } else {
            gantt.push_back({processes[best].pid, currentTime, currentTime + processes[best].burst});
            currentTime += processes[best].burst;
            finished[best] = true;
            done++;
        }
    }
    showGantt(gantt);
    calculateResults(processes, gantt);
}

// Main function where program starts
int main() {
    cout << "Welcome to the CPU Scheduler Simulator!\n";
    int numProcesses;
    cout << "How many processes do you want? ";
    cin >> numProcesses;

    // Basic input validation for number of processes
    if (cin.fail() || numProcesses <= 0) {
        cout << "Invalid number of processes! Please enter a positive integer.\n";
        return 1;
    }

    vector<Process> processes;
    for (int i = 0; i < numProcesses; i++) { // Input details for each process
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

        processes.push_back({i + 1, arrival, burst, priority, burst}); // Add process to list
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
        cout << "Oops! That’s not a valid choice.\n";
    }

    cout << "\nAll done! Hope that helped you see how scheduling works!\n";
    return 0; // End program
}