from flask import Flask, render_template, request, redirect, url_for

app = Flask(__name__)

# Duck object for simulation
class Duck:
    def __init__(self, name):
        self.name = name
        self.alarm_time = None
        self.gap_between_alarms = None

# Simulated main duck
main_duck = Duck('Goose')

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
    app.run(debug=True)
