// Toggle quantum visibility
document.getElementById('algorithm').addEventListener('change', function() {
    const algo = this.value;
    document.getElementById('quantumDiv').classList.toggle('hidden', algo !== '3');
    updateProcessInputs();
});

// Generate process input fields dynamically
document.getElementById('numProcesses').addEventListener('input', updateProcessInputs);

function updateProcessInputs() {
    const num = parseInt(document.getElementById('numProcesses').value) || 0;
    const algo = document.getElementById('algorithm').value;
    const showPriority = algo === '4';
    const container = document.getElementById('processInputs');
    container.innerHTML = '';

    for (let i = 0; i < num; i++) {
        let inputs = `
            <div class="grid grid-cols-1 md:grid-cols-${showPriority ? '3' : '2'} gap-4">
                <div>
                    <label class="block text-gray-700">P${i + 1} Arrival Time:</label>
                    <input type="number" id="arrival${i}" min="0" class="w-full p-3 border rounded mt-2" placeholder="e.g., 0" required>
                </div>
                <div>
                    <label class="block text-gray-700">P${i + 1} Burst Time:</label>
                    <input type="number" id="burst${i}" min="1" class="w-full p-3 border rounded mt-2" placeholder="e.g., 5" required>
                </div>
        `;
        if (showPriority) {
            inputs += `
                <div>
                    <label class="block text-gray-700">P${i + 1} Priority:</label>
                    <input type="number" id="priority${i}" min="0" class="w-full p-3 border rounded mt-2" placeholder="e.g., 2" required>
                </div>
            `;
        }
        inputs += `</div>`;
        container.innerHTML += inputs;
    }

    // Add real-time validation
    document.querySelectorAll('input[type="number"]').forEach(input => {
        input.addEventListener('input', function() {
            if (!this.value || parseInt(this.value) < parseInt(this.min)) {
                this.classList.add('input-error');
            } else {
                this.classList.remove('input-error');
            }
        });
    });
}

// Handle form submission
document.getElementById('submitBtn').addEventListener('click', async function() {
    const num = parseInt(document.getElementById('numProcesses').value);
    const algo = document.getElementById('algorithm').value;
    const quantum = algo === '3' ? document.getElementById('quantum').value : 0;

    let inputData = `${num}\n${algo}\n${quantum}\n`;
    for (let i = 0; i < num; i++) {
        const arrival = document.getElementById(`arrival${i}`).value;
        const burst = document.getElementById(`burst${i}`).value;
        const priority = algo === '4' ? document.getElementById(`priority${i}`).value : '0';
        if (!arrival || !burst || (algo === '4' && !priority)) {
            alert(`Please fill all fields for Process P${i + 1}`);
            return;
        }
        inputData += `${arrival} ${burst} ${priority}\n`;
    }

    document.getElementById('loading').classList.remove('hidden');
    try {
        const output = await runSimulation(inputData);
        displayResults(output);
    } catch (error) {
        alert("Error running simulation: " + error.message);
    } finally {
        document.getElementById('loading').classList.add('hidden');
    }
});

// Run simulation by sending input to Flask server
async function runSimulation(inputData) {
    const response = await fetch('http://127.0.0.1:5001/run', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ input: inputData })
    });
    if (!response.ok) throw new Error('Simulation failed');
    const data = await response.json();
    return data.output;
}

// Display results with dynamic Gantt chart
function displayResults(output) {
    const lines = output.trim().split('\n');
    const ganttChart = document.getElementById('ganttChart');
    const metrics = document.getElementById('metrics');
    ganttChart.innerHTML = '';
    metrics.innerHTML = '';

    const colors = ['bg-blue-500', 'bg-green-500', 'bg-purple-500', 'bg-red-500', 'bg-yellow-500', 'bg-teal-500'];
    if (lines[0].startsWith('|')) {
        const processes = lines[0].split('|').filter(p => p.trim());
        const times = lines[1].split(/\s+/).filter(t => t.trim());
        for (let i = 0; i < processes.length; i++) {
            const start = parseInt(times[i]);
            const end = parseInt(times[i + 1]);
            const width = (end - start) * 20;
            const color = colors[i % colors.length];
            ganttChart.innerHTML += `
                <div class="gantt-bar ${color}" style="width: ${width}px" data-tooltip="P${processes[i].trim().split(' ')[1]}: ${start}-${end}">${processes[i]}</div>
            `;
        }
    }

    metrics.innerHTML = lines.slice(2).map(line => `<p>${line}</p>`).join('');
    document.getElementById('results').classList.remove('hidden');
    document.getElementById('ganttChart').classList.remove('hidden');
    document.getElementById('metrics').classList.add('hidden');
}

// Toggle between Gantt and Metrics
document.getElementById('showGantt').addEventListener('click', function() {
    document.getElementById('ganttChart').classList.remove('hidden');
    document.getElementById('metrics').classList.add('hidden');
});

document.getElementById('showMetrics').addEventListener('click', function() {
    document.getElementById('ganttChart').classList.add('hidden');
    document.getElementById('metrics').classList.remove('hidden');
});

// Reset form
document.getElementById('resetBtn').addEventListener('click', function() {
    document.getElementById('numProcesses').value = '';
    document.getElementById('algorithm').value = '1';
    document.getElementById('quantum').value = '';
    document.getElementById('processInputs').innerHTML = '';
    document.getElementById('results').classList.add('hidden');
    document.getElementById('quantumDiv').classList.add('hidden');
});