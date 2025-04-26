from flask import Flask, render_template, jsonify, request
import subprocess
import os

# Create Flask app
app = Flask(__name__)

@app.route('/')
def home():
    return render_template('index.html')

@app.route('/run', methods=['POST'])
def run_simulation():
    data = request.json
    if not data or 'input' not in data:
        return jsonify({"error": "Missing input data"}), 400

    # Write input data to input.txt
    with open('input.txt', 'w') as f:
        f.write(data['input'])

    # Run the C++ scheduler
    try:
        subprocess.run(['./scheduler'], check=True)
    except subprocess.CalledProcessError as e:
        return jsonify({"error": "Scheduler execution failed", "details": str(e)}), 500

    # Read the output from output.txt
    if os.path.exists('output.txt'):
        with open('output.txt', 'r') as f:
            output = f.read()
        return jsonify({"output": output})
    else:
        return jsonify({"error": "Output file not generated"}), 500

if __name__ == "__main__":
    port = int(os.environ.get('PORT', 5000))  # Render ke liye
    app.run(host='0.0.0.0', port=port)
