import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import lti, lsim

sample_rate = 100

rate = 12 # (breaths / min)
time_inspiration = 2 # (s)
pressure_level = 20 # peak pressure (cm H20)
peep = 5 # (cm H20)
simulated_cycles = 5

lung_compliance = 0.3 # (L / cm H20)
# FIXME
airway_resistance = 0.1 # (cm H20 / L / s)

# ----- generate mouth pressure signal -----

total_cycle_time = 60 / rate
t = np.linspace(
    0,
    simulated_cycles * total_cycle_time,
    int(simulated_cycles * total_cycle_time * sample_rate)
)

v_mouth = np.zeros(int(total_cycle_time * sample_rate))
# rect
# v_mouth[:time_inspiration * sample_rate] = pressure_level
# ramp
v_mouth[:time_inspiration * sample_rate] = np.linspace(
    0,
    pressure_level,
    time_inspiration * sample_rate
)
v_mouth += peep
v_mouth = np.tile(v_mouth, simulated_cycles)

# ----- simulate pressures/flows/volumes -----

# simulate lung pressure
num = [0, 1]
denom = [airway_resistance / lung_compliance, 1]
_, v_lungs, _ = lsim((num, denom), v_mouth, t)

# simulate airway flow
num = [airway_resistance / lung_compliance, 0]
denom = [airway_resistance / lung_compliance, 1]
_, i_airway, _ = lsim((num, denom), v_mouth, t)

# simulate lung volume
num = [0, airway_resistance / lung_compliance, 0]
denom = [airway_resistance / lung_compliance, 1, 0]
_, q_lungs, _ = lsim((num, denom), v_mouth, t)

plt.close()
plt.figure(figsize=(5, 8))
plt.subplot(4, 1, 1)
plt.plot(t, v_mouth)
plt.title('Mouth Pressure')
plt.ylabel('cm H20')
plt.grid()
plt.subplot(4, 1, 2)
plt.plot(t, i_airway)
plt.title('Airway Flow')
plt.ylabel('L / s')
plt.grid()
plt.subplot(4, 1, 3)
plt.plot(t, v_lungs)
plt.title('Lung Pressure')
plt.ylabel('cm H20')
plt.grid()
plt.subplot(4, 1, 4)
plt.plot(t, q_lungs)
plt.title('Lung Volume')
plt.ylabel('L')
plt.xlabel('time')
plt.grid()
plt.tight_layout()
plt.show()
