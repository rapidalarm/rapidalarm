import numpy as np
from scipy.io.wavfile import read, write
from scipy.ndimage import maximum_filter, minimum_filter
from scipy.signal import iirfilter, lfilter, freqz, spectrogram
import matplotlib.pyplot as plt

"""
1. high pass 7KHz
2. absolute value
3. sum all samples within each 0.1s window
4. minimum filter, window size 3
5. measure waveform duration above fixed threshold
"""

samp_rate, x = read('audio.wav')
samp_rate = samp_rate // 4
x = x[::4]
# dont need full bit depth
# x = x.astype('int8')
t = np.linspace(0, len(x) / samp_rate, len(x))
plt.subplot(3, 1, 1)
f_spec, t_spec, sxx_spec = spectrogram(x, samp_rate)
plt.pcolormesh(t_spec, f_spec, sxx_spec)

# ----- step 1 -----

# normalized cutoff frequency
norm_cutoff = 5000 / samp_rate
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
