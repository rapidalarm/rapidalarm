import numpy as np
from scipy.io.wavfile import read, write
from scipy.ndimage import maximum_filter, minimum_filter
from scipy.signal import iirfilter, lfilter, freqz, spectrogram, peak_widths, find_peaks
import matplotlib.pyplot as plt

"""
1. high pass 7KHz
2. absolute value
3. sum all samples within each 0.1s window
4. minimum filter, window size 3
5. peak width computation, (need more robust and faster method for micro)
"""

samp_rate, x = read('audio.wav')
samp_rate = samp_rate // 4
x = x[::4]
# dont need full bit depth
# x = x.astype('int8')
t = np.linspace(0, len(x) / samp_rate, len(x))
plt.figure()
plt.subplot(3, 1, 1)
plt.title('Unfiltered and Filtered Audio Spectrograms')
f_spec, t_spec, sxx_spec = spectrogram(x, samp_rate)
plt.pcolormesh(t_spec, f_spec, sxx_spec)

# ----- step 1 -----

# normalized cutoff frequency
norm_cutoff = 7000 / samp_rate
# ripple db
ripple = 1
# 7th order chebyshev, 8 taps
num, denom = iirfilter(4, norm_cutoff, ripple, ftype='cheby1', btype='highpass')

plt.subplot(3, 1, 2)
w, h = freqz(num, denom, samp_rate)
plt.plot(samp_rate * w / np.pi, abs(h))
plt.grid()

# apply filter
f = lfilter(num, denom, x)

plt.subplot(3, 1, 3)
f_spec, t_spec, sxx_spec = spectrogram(f, samp_rate)
plt.pcolormesh(t_spec, f_spec, sxx_spec)
plt.show()

# ----- step 2 -----

window_size = int(0.1 * samp_rate)
num_windows = len(f) // window_size
# start times of each window, for plotting
tt = t[:num_windows * window_size:window_size]
# split signal into 0.1s windows
y = np.vstack(np.split(f[:num_windows * window_size], num_windows))

# ----- step 3 -----

# drop 2 least significant bits so sum over each window fits into int16
if x.dtype == 'int8':
    f = f.astype('int8') >> 2
# compute sum
s = np.sum(abs(y), axis=1)

# ----- step 4 -----

mm = minimum_filter(s, 3)

# ----- step 5 -----

# peaks at least 1.5 seconds apart
peaks, _ = find_peaks(mm, distance=1.5 * samp_rate / window_size, prominence=1e6)
# measure pulse width at 50% of its height
results = peak_widths(mm, peaks, 0.5)
widths = results[0]

flow_rate = 40 / 60 # L / s
tidal_volumes = flow_rate * (widths * window_size) / samp_rate
print('Average Tidal Volume:', tidal_volumes.mean())
print('Std. Tidal Volume:', tidal_volumes.std())

plt.figure(figsize=(8, 4))
plt.plot(mm)
plt.plot(peaks, mm[peaks], "x")
plt.hlines(*results[1:], color="C2")
plt.show()

plt.grid()
plt.show()

# plt.plot(t[:num_windows * window_size:window_size], np.linalg.norm(y, axis=1))
# plt.plot(t[:num_windows * window_size:window_size], np.sum(abs(y), axis=1))
# plt.plot()
# plt.show()
