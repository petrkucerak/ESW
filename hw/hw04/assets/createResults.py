import json
import shutil as s
import math

import numpy as np
import scipy.stats as stats

file_name = "matrix_multiplication_benchmark_20240328_083932.json"
confidence = 0.95

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


def get_avg(measure_type):
    all_measurements = load_jmh_measurements("../" + file_name)
    impl_measurements = all_measurements[measure_type]
    mean = np.mean(impl_measurements)
    return mean


def get_bounds(measure_type):
    all_measurements = load_jmh_measurements("../" + file_name)
    impl_measurements = all_measurements[measure_type]

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

    return [mean - h, mean + h, h]


def get_ratio(measure_type_1, measure_type_2):
    return get_avg(measure_type_2) / get_avg(measure_type_1)


def get_comp_bounds(measure_type_1, measure_type_2):
    ratio_old = get_avg(measure_type_1)
    ratio_new = get_avg(measure_type_2)
    h_old = get_bounds(measure_type_1)[2]
    h_new = get_bounds(measure_type_2)[2]

    # source https://kar.kent.ac.uk/33611/45/p63-kaliber.pdf [10.1]
    root = math.pow(math.pow(ratio_old*ratio_new, 2) - ((math.pow(ratio_old, 2) -
                    math.pow(h_old, 2))*(math.pow(ratio_new, 2) - math.pow(h_new, 2))), 1/2)

    ub = ((ratio_old * ratio_new) + root) / \
        (math.pow(ratio_old, 2) - math.pow(h_old, 2))
    lb = ((ratio_old * ratio_new) - root) / \
        (math.pow(ratio_old, 2) - math.pow(h_old, 2))

    return [lb, ub]

results = {
    "performance": [
        {
            "impl_name": "measureMultiply",
            "average": get_avg("measureMultiply"),
            "cf_lb": get_bounds("measureMultiply")[0],
            "cf_ub": get_bounds("measureMultiply")[1]
        },
        {
            "impl_name": "measureMultiply1D",
            "average": get_avg("measureMultiply1D"),
            "cf_lb": get_bounds("measureMultiply1D")[0],
            "cf_ub": get_bounds("measureMultiply1D")[1]
        },
        {
            "impl_name": "measureMultiplyTrans",
            "average": get_avg("measureMultiplyTrans"),
            "cf_lb": get_bounds("measureMultiplyTrans")[0],
            "cf_ub": get_bounds("measureMultiplyTrans")[1]
        }
    ],
    "comparisons": [
        {
            "impl_1_name": "measureMultiply",
            "impl_2_name": "measureMultiply1D",
            "ratio": get_ratio("measureMultiply", "measureMultiply1D"),
            "cf_lb": get_comp_bounds("measureMultiply", "measureMultiply1D")[0],
            "cf_ub": get_comp_bounds("measureMultiply", "measureMultiply1D")[1]
        },
        {
            "impl_1_name": "measureMultiply",
            "impl_2_name": "measureMultiplyTrans",
            "ratio": get_ratio("measureMultiply", "measureMultiplyTrans"),
            "cf_lb": get_comp_bounds("measureMultiply", "measureMultiplyTrans")[0],
            "cf_ub": get_comp_bounds("measureMultiply", "measureMultiplyTrans")[1]
        },
        {
            "impl_1_name": "measureMultiply1D",
            "impl_2_name": "measureMultiply",
            "ratio": get_ratio("measureMultiply1D", "measureMultiply"),
            "cf_lb": get_comp_bounds("measureMultiply1D", "measureMultiply")[0],
            "cf_ub": get_comp_bounds("measureMultiply1D", "measureMultiply")[1]
        },
        {
            "impl_1_name": "measureMultiply1D",
            "impl_2_name": "measureMultiplyTrans",
            "ratio": get_ratio("measureMultiply1D", "measureMultiplyTrans"),
            "cf_lb": get_comp_bounds("measureMultiply1D", "measureMultiplyTrans")[0],
            "cf_ub": get_comp_bounds("measureMultiply1D", "measureMultiplyTrans")[1]
        },
        {
            "impl_1_name": "measureMultiplyTrans",
            "impl_2_name": "measureMultiply",
            "ratio": get_ratio("measureMultiplyTrans", "measureMultiply"),
            "cf_lb": get_comp_bounds("measureMultiplyTrans", "measureMultiply")[0],
            "cf_ub": get_comp_bounds("measureMultiplyTrans", "measureMultiply")[1]
        },
        {
            "impl_1_name": "measureMultiplyTrans",
            "impl_2_name": "measureMultiply1D",
            "ratio": get_ratio("measureMultiplyTrans", "measureMultiply1D"),
            "cf_lb": get_comp_bounds("measureMultiplyTrans", "measureMultiply1D")[0],
            "cf_ub": get_comp_bounds("measureMultiplyTrans", "measureMultiply1D")[1]
        }
    ]
}


# Create the measurements.json file
s.copy2("../" + file_name, "../measurements.json")

# Create the results.json file
with open("../results.json", "w+") as f:
    f.write(json.dumps(results))
