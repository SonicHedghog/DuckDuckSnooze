import datetime
import re
import subprocess
import time
from flask import Flask, render_template, request, redirect, url_for

app = Flask(__name__)

# Duck object for simulation
class Duck:
    def __init__(self, name):
        self.name = name
        self.alarm_time = None
        self.gap_between_alarms = None

    def cry():
        return

# Simulated main duck
main_duck = Duck('Goose')

# get ip addresses
result = subprocess.run(['arp', '-a'], capture_output=True, text=True)

# Regex to find IP addresses
ducks = re.findall(r'\d+\.\d+\.\d+\.\d+', result.stdout)

# Method to be run in background
def check_number():
    global main_duck
    global ducks
    while True:
        if main_duck.alarm_time.upper() == datetime.now().strftime('%H:%M %p'):
            main_duck.cry()

            for x in ducks:
                time.sleep(main_duck.gap_between_alarms)
                request.get(f"http://{x}/5/on")

            while main_duck.alarm_time.upper() == datetime.now().strftime('%H:%M %p'):
                time.sleep(1)
        time.sleep(1)  # Check every second

# Settings page for the main duck
@app.route('/', methods=['GET', 'POST'])
def duck_settings():
    if request.method == 'POST':
        # Handle setting alarm time and gap between ducks
        main_duck.alarm_time = request.form.get('alarm_time')
        main_duck.gap_between_alarms = request.form.get('gap_between_alarms')
        return redirect(url_for('duck_settings'))

    return render_template('settings.html', duck=main_duck)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)
