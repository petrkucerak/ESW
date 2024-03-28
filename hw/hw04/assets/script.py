import json
import math

import numpy as np
import scipy.stats as stats


# loads the JMH result JSON into a dictionary where keys are benchmark names without packages
# and values are numpy 2D arrays
def load_jmh_measurements(path: str) -> dict[str, np.array]:
    measurements = {}
    with open(path, 'rt', encoding='utf-8') as file:
        jmh_data = json.load(file)
        for impl_data in jmh_data:
            name = impl_data['benchmark']
            name = name[name.rindex('.') + 1:]  # remove package names
            measurements[name] = np.array(
            impl_data['primaryMetric']['rawData'])
    return measurements

confidence = 0.95

all_measurements = load_jmh_measurements('../matrix_multiplication_benchmark_20240328_083932.json')
impl_measurements = all_measurements['measureMultiply']

# r2, r1
execution_count, iteration_count = np.shape(impl_measurements)

mean = np.mean(impl_measurements)

# mean of execution variances
S1 = np.mean(np.var(impl_measurements, axis=1, ddof=1))
# variance of execution means
S2 = np.var(np.mean(impl_measurements, axis=1), ddof=1)

# (1-alpha/2)-quantile of t-distribution with given degrees of freedom
alpha = 1 - confidence
t_quantile = stats.t.ppf(1 - alpha / 2, df=execution_count - 1)

# half-width of the confidence interval
h = t_quantile * math.sqrt(S2 / execution_count)

print(f'{mean} +- {h} ms/op')
print(f'[{mean - h}, {mean + h}]')
