# f = open("measurements1.txt")
# print("Opened Measurements")
# registers = {}
# for line in f:
#     vals = line.split("-->")
#     if vals[0] not in registers:
#         registers[vals[0]] = [vals[1]]
#     else:
#         registers[vals[0]].append(vals[1])
#
# f.close()
# print(len(registers["0x01"]))


# Cable 1 (green marker small and fixed) loss:
# 1.07dB (at 865.9MHz)
# 1.07dB (at 865.9MHz)
# 1.07dB (at 865.9MHz)
# 1.07dB (at 865.9MHz)
# 1.08dB (at 865.9MHz)
# 1.08dB (at 865.7MHz)
# 1.08dB (at 865.7MHz)
# 1.08dB (at 865.7MHz)
# 1.08dB (at 867.5MHz)
# 1.08dB (at 867.5MHz)
# 1.08dB (at 867.5MHz)
# 1.08dB (at 867.5MHz)

# Cable 2 (green marker big and not fixed) loss:
# 1.16dB (at 867.5MHz)
# 1.16dB (at 867.5MHz)
# 1.15dB (at 867.5MHz)
# 1.15dB (at 867.5MHz)
# 1.15dB (at 867.5MHz)
# 1.17dB (at 867.5MHz)
# 1.16dB (at 867.5MHz)
# 1.14dB (at 865.9MHz)
# 1.14dB (at 865.9MHz)
# 1.14dB (at 865.9MHz)
# 1.15dB (at 865.9MHz)


# setup

#       WARP ---- cable 1 ------ splitter ------- cable 2 -------- spectrum analyser
#                                   ||
#                                   ||
#                                   sig gen

# Time: 14:21
# Measured frequency: 865.9MHz, started in 1dBm increments, then moved to 5dBm increments
# so that you can see it more easily when plotted:
# -10
# -15
# -20
# -25
# -30
# squiggle then -35
# -40
# -45
# -50
# -55


# 14:28
# Same freq settings but this time measurement of power from spectrum analyser
# Multiple readings are because the power was varying heavily
# -5.42
# -10.43
# -10.42
# -15.46
# -15.46
# -20.38
# -20.38
# -25.40
# -30.40
# -35.39
# -35.40
# -40.40
# -40.38
# -45.42
# -45.39
# -50.38
# -50.40
# -50.35
# -55.42
# -55.38
# -55.36
# -60.42
# -60.34
# -60.29
# -65.22
# -65.30

import matplotlib.pyplot as plt
import numpy as np
from copy import deepcopy
from scipy.optimize import curve_fit

f = open("Mon13Jan1429_test_865.9MHz.log")
print("Opened Measurements")
real_measurements = []
errors = 0
for line in f:

    try:
        vals = line.split(",")
        reading_count = int(vals[0])
        # Start and end readings are within limits for file
        assert reading_count > 359022
        assert reading_count < 505980
        # TSR values match
        assert vals[1] == vals[7]
        # No config errors
        assert int(vals[9]) == 0
        # 0,1,2
        # 3,4,5 are x,y,z orientations
        adc_value = int(vals[6])
        # Must be under 2^12 and over 0
        assert adc_value > 0
        assert adc_value < 2 ** 12 + 1
        real_measurements.append(adc_value)

    except:
        errors += 1
        pass


f.close()


print("{} Errors reading from the file".format(errors))
print("{} Values read successfully".format(len(real_measurements)))


real_measurements = np.array(real_measurements)
grad_measurements = np.diff(real_measurements)
plt.figure(1)
plt.plot(range(len(real_measurements)), real_measurements)
plt.plot(range(len(grad_measurements)), grad_measurements)


signal_generator_output_levels = [
    -5.42,
    -10.43,
    -15.46,
    -20.38,
    -25.40,
    -30.40,
    -35.40,
    -40.39,
    -45.41,
    -50.39,
    -55.39,
    -60.35,
]

signal_generator_output_levels_index = 0

single_power_measurement_vals = []
a = []
prev_mean = 0
number_of_dud_levels = 0

data_to_be_fit_x = []
data_to_be_fit_y = []

for i, grad in enumerate(grad_measurements):
    a.append(real_measurements[i])
    if grad > 140:

        b = np.array(deepcopy(a))
        mean_val = np.mean(b)
        a = []

        if mean_val - prev_mean > 3:
            single_power_measurement_vals.append(b)
            print(b)
            print("Mean ADC Value: {}".format(mean_val))
            plt.figure(1)
            plt.annotate("{:.0f}".format(mean_val), (i, real_measurements[i]))

            data_to_be_fit_x.append(
                signal_generator_output_levels[signal_generator_output_levels_index]
            )
            data_to_be_fit_y.append(mean_val)

            signal_generator_output_levels_index += 1

            prev_mean = mean_val
        else:
            number_of_dud_levels += 1
            break

plt.figure(2)
plt.title("Average Measured ADC Values vs True Signal Power")
plt.ylabel("Raw ADC Reading [N/A]")
plt.xlabel("True Signal Power [dBm]")
data_to_be_fit_x = np.array(data_to_be_fit_x)
data_to_be_fit_y = np.array(data_to_be_fit_y)
plt.plot(data_to_be_fit_x, data_to_be_fit_y, "r+")


def lin(x, m, c):
    return m * x + c


# assuming ydata = f(xdata, *params)
popt, pcov = curve_fit(lin, data_to_be_fit_x, data_to_be_fit_y)

print(popt)
plt.plot(
    data_to_be_fit_x,
    lin(data_to_be_fit_x, *popt),
    "-b",
    label="ADC Reading = {:.1f} (True Power) - {:.1f}".format(*popt),
)
plt.legend()


# plt.figure(3)
# single_power_measurement_vals = np.array(single_power_measurement_vals)
# print(single_power_measurement_vals)
# plt.hist(
#     single_power_measurement_vals,
#     rwidth=1,
#     bins=range(
#         min(single_power_measurement_vals[0]), max(single_power_measurement_vals[-1])
#     ),
# )

# for index, measured_adc_vals in enumerate(single_power_measurement_vals):
#     real_power = signal_generator_output_levels[index]
#     plt.figure(3+index)
#     plt.title("Distribution of Measured Power Values")
#


plt.show()
