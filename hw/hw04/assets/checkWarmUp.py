import json
import numpy as np
import matplotlib.pyplot as plt

file_name = "matrix_multiplication_benchmark_20240328_083932.json"


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


def get_data(measure_type):
    all_measurements = load_jmh_measurements("../" + file_name)
    impl_measurements = np.array(all_measurements[measure_type]).flatten()

    # print(impl_measurements)
    return impl_measurements

def create_graph(measure_type):
    fig, ax = plt.subplots()
    ax.plot(get_data(measure_type), "+",
            color="blue", markersize="8")
    ax.set_xlabel("iterations", fontsize=8)
    ax.set_ylabel("time", fontsize=8)
    ax.set_title(measure_type)
    plt.grid()
    plt.show()

create_graph("measureMultiply")
create_graph("measureMultiply1D")
create_graph("measureMultiplyTrans")