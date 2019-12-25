import matplotlib.pyplot as plt
import numpy as np
import scipy.io.wavfile
import scipy.signal
from scipy.stats import pearsonr, spearmanr


# Opening the files. The actual lag is 3648083 frames.
file1 = "audio/youtube.wav"
file2 = "audio/recorded.wav"
sample_rate = 48000
rate1, audio1 = scipy.io.wavfile.read(file1)
rate2, audio2 = scipy.io.wavfile.read(file2)

# Zero padding
n = 10 * sample_rate  # Seconds of audio
data1 = audio1[:n]
data1 = np.pad(data1, (0,n), 'constant')
data2 = audio2[:n]
data2 = np.pad(data2, (0,n), 'constant')
#  plt.plot(data1)
#  plt.plot(data2)
#  plt.show()


# METHOD 1: cross-correlation method from numpy that calculates a
# time-dependent result with the cross-correlation coefficients (slower).
# print("Executing np correlate")
# out = np.correlate(data1, data2, "full")
# plt.plot(out)
# plt.show()

# METHOD 2: using the formula with FFT. We can either use FFT and IFFT, or
# save half the computation time by using RFFT and IRFFT, since the wave only
# has real values.
print("Executing with the formula")
fft1 = np.fft.rfft(data1)
fft2 = np.fft.rfft(data2)
products = []
for i in range(fft1.size):
    mag = abs(fft2[i])
    products.append(complex(fft1[i].real * mag, fft1[i].imag * mag)) 
result = np.fft.irfft(products)
#  plt.plot(result)
#  plt.show()

# Getting the peak
confidence = result[0]
lag = 0
for i in range(result.size):
    if abs(result[i]) > confidence:
        confidence = result[i]
        lag = i
print(lag / (sample_rate / 1000), "milliseconds")

result1 = data1[:data1.size//2]
result2 = np.roll(data2[:data2.size//2], -lag)

# Calculating the Pearson coefficient
print("Executing scipy.stats.pearsonr")
corr, pvalue = pearsonr(result1, result2)
print(f"Finished with a p-value of {pvalue}: {corr}")

# Calculating the Pearson coefficient with numpy
print("Executing np.corrcoef")
np.corrcoef(result1, result2)
print(f"Finished with a p-value of {pvalue}: {corr}")

# Calculating the Spearman coefficient
print("Executing scipy.stats.spearmanr")
corr, pvalue = spearmanr(result1, result2)
print(f"Finished with a p-value of {pvalue}: {corr}")

# Plotting the results
plt.plot(result1)
plt.plot(result2)
plt.show()