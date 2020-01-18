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
# 1.076666667 average
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
# 1.151818182 average

# 0.0751dB difference
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
import scipy.stats as stats

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
plt.title("Raw Measurements Taken")
plt.xlabel("Measurement Number")
plt.ylabel("ADC Value")
# plt.plot(range(len(grad_measurements)), grad_measurements)


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
    -65.26,
]

single_power_measurement_vals = []
a = []


data_to_be_fit_x = []
data_to_be_fit_y = []


# real_measurements[39456]
auto_index_finish = 0
# Separating the different power measurements using threshold on gradient
measurement_index = 0
for i, grad in enumerate(grad_measurements):
    a.append(real_measurements[i])
    if grad > 140:
        b = np.array(deepcopy(a))
        a = []

        if measurement_index < 10:
            single_power_measurement_vals.append(b)
            print(
                "Saving at index: {:5} = {: 4.2f}dBm Mean ADC Value: {:04.0f}".format(
                    i, signal_generator_output_levels[measurement_index], np.mean(b)
                )
            )
            plt.figure(1)
            plt.plot(i, real_measurements[i], "or")
            # plt.annotate("{:.0f}".format(np.mean(b)), (i, real_measurements[i]))
            plt.annotate(
                "{}dBm".format(signal_generator_output_levels[measurement_index]),
                (i, real_measurements[i]),
            )
            measurement_index += 1
            auto_index_finish = i
        else:
            break

# Manually splice last two values since they are very noisy
end_55 = 39455
end_60 = 45090

# 55
plt.plot(auto_index_finish + 1, real_measurements[auto_index_finish + 1], "og")
plt.plot(end_55, real_measurements[end_55], "og")
b = np.array(real_measurements[auto_index_finish + 1 : end_55 + 1])

assert b[-1] == real_measurements[end_55]
single_power_measurement_vals.append(b)


# 60
plt.plot(end_55 + 1, real_measurements[end_55 + 1], "og")
plt.plot(end_60, real_measurements[end_60], "og")

b = np.array(real_measurements[end_55 + 1 : end_60 + 1])
single_power_measurement_vals.append(b)
# 65
plt.plot(end_60 + 1, real_measurements[end_60 + 1], "og")
plt.plot(len(real_measurements), real_measurements[-1], "og")
b = np.array(real_measurements[end_60 + 1 :])

single_power_measurement_vals.append(b)


plt.figure(2)
plt.title("Average Measured ADC Values vs True Signal Power")
plt.ylabel("Raw ADC Reading [N/A]")
plt.xlabel("True Signal Power [dBm]")

y_err = np.empty((2, len(single_power_measurement_vals)))

for i, vals in enumerate(single_power_measurement_vals):
    data_to_be_fit_x.append(signal_generator_output_levels[i])
    data_to_be_fit_y.append(np.mean(vals))

    y_err[0, i] = np.mean(vals) - np.amin(vals)
    y_err[1, i] = np.amax(vals) - np.mean(vals)
    print(
        "Reading at {: 4.2f}dBm, Minimum data point at index {}, length {}".format(
            signal_generator_output_levels[i],
            np.where(vals == np.amin(vals)),
            len(vals),
        )
    )


data_to_be_fit_x = np.array(data_to_be_fit_x)
data_to_be_fit_y = np.array(data_to_be_fit_y)
plt.errorbar(data_to_be_fit_x, data_to_be_fit_y, yerr=y_err, fmt="r+")


def lin(x, m, c):
    return m * x + c


# assuming ydata = f(xdata, *params)
popt, pcov = curve_fit(lin, data_to_be_fit_x[:-3], data_to_be_fit_y[:-3])
# print(data_to_be_fit_x[:-3])
print("Gradient: {}, Intercept: {}".format(*popt))
plt.plot(
    data_to_be_fit_x,
    lin(data_to_be_fit_x, *popt),
    "-b",
    label="ADC Reading = {:.1f} (True Power) + {:.1f}".format(*popt),
)
plt.legend()


plt.figure(3)

plt.hist(
    single_power_measurement_vals[-3:],
    bins=range(
        min(single_power_measurement_vals[-3]), max(single_power_measurement_vals[-1])
    ),
    width=1,
)
plt.title("Histogram of Measurements for -55.39 ,-60.35 -65.26dBm")
plt.xlabel("ADC Value")
plt.ylabel("Occurence")
plt.figure(4)

# 5 % Hypothesis Test
alpha = 0.05
print(len(signal_generator_output_levels))
# enumerate(single_power_measurement_vals[:-3]):
for j, good_measurements in enumerate(single_power_measurement_vals[-3:]):
    i = j + 10
    stat, pvalue = stats.normaltest(good_measurements)
    mean = np.mean(good_measurements)
    std = np.std(good_measurements)
    name = "{: 4.2f}dBm".format(signal_generator_output_levels[i])
    normal = pvalue < alpha
    print(
        "For {:>9} measurements: n={:5d} mean={:4.0f} std={:4.2f}, Normal Fit: {} stat={} pvalue={} ".format(
            name, len(good_measurements), mean, std, normal, stat, pvalue
        )
    )
    plt.hist(
        good_measurements,
        bins=range(int(mean) - 100, int(mean) + 100),
        width=1,
        label="{}dBm".format(signal_generator_output_levels[i]),
        density=True,
    )
    if normal:
        x = np.linspace(mean - 3 * std, mean + 3 * std, 100)
        y = stats.norm.pdf(x, loc=mean, scale=std)
        plt.plot(
            x,
            y,
            "r",
            label="{}dBm Normal Fit".format(signal_generator_output_levels[i]),
        )
    # zero_mean = good_measurements - np.mean(good_measurements)
    # # n = len(good_measurements)
    # # S = np.std(good_measurements, ddof=1)
    # # t_dist = zero_mean / (S / np.sqrt(n))
    # s = np.std(good_measurements)
    # dist = zero_mean / s
    #
    # plt.hist(
    #     dist,
    #     bins=np.linspace(-3, 3, 40),
    #     # bins=range(int(np.floor(min(t_dist))), int(np.ceil(max(t_dist)))),
    #     label="{}dBm".format(signal_generator_output_levels[i]),
    # )
plt.title("Histogram of Noisiest Measurements")
plt.xlabel("ADC Value")
plt.ylabel("Occurence")
plt.legend()
# plt.figure(4)
# plt.hist(single_power_measurement_vals[-2], bins=50)
# plt.figure(5)
# plt.hist(single_power_measurement_vals[-1], bins=50)
# single_power_measurement_vals = np.array(single_power_measurement_vals)

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
