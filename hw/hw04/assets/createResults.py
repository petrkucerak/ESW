import json
import shutil as s

file_name = "matrix_multiplication_benchmark_20240328_083739.json"

results = {
    "performance": [
       {
           "impl_name": "measureMultiply",
           "average": 274.45720624999996,
           "cf_lb": 271.6351655689061,
           "cf_ub": 273.27924693109395
       },
        {
           "impl_name": "measureMultiply1D",
           "average": 274.45720624999996,
           "cf_lb": 271.6351655689061,
           "cf_ub": 273.27924693109395
       },
        {
           "impl_name": "measureMultiplyTrans",
           "average": 274.45720624999996,
           "cf_lb": 271.6351655689061,
           "cf_ub": 273.27924693109395
       }
    ],
    "comparisons": [
        {
            "impl_1_name": "measureMultiply",
            "impl_2_name": "measureMultiply1D",
            "ratio": 0.9404346124708851,
            "cf_lb": 0.9374657053490532,
            "cf_ub": 0.943403519592717
        },
        {
            "impl_1_name": "measureMultiply",
            "impl_2_name": "measureMultiplyTrans",
            "ratio": 0.9404346124708851,
            "cf_lb": 0.9374657053490532,
            "cf_ub": 0.943403519592717
        },
        {
            "impl_1_name": "measureMultiply1D",
            "impl_2_name": "measureMultiply",
            "ratio": 0.9404346124708851,
            "cf_lb": 0.9374657053490532,
            "cf_ub": 0.943403519592717
        },
        {
            "impl_1_name": "measureMultiply1D",
            "impl_2_name": "measureMultiplyTrans",
            "ratio": 0.9404346124708851,
            "cf_lb": 0.9374657053490532,
            "cf_ub": 0.943403519592717
        },
        {
            "impl_1_name": "measureMultiplyTrans",
            "impl_2_name": "measureMultiply",
            "ratio": 0.9404346124708851,
            "cf_lb": 0.9374657053490532,
            "cf_ub": 0.943403519592717
        },
        {
            "impl_1_name": "measureMultiplyTrans",
            "impl_2_name": "measureMultiply1D",
            "ratio": 0.9404346124708851,
            "cf_lb": 0.9374657053490532,
            "cf_ub": 0.943403519592717
        }
    ]
}

# Load data
with open("../" + file_name, "r") as f:
    data = json.load(f)

# Create the measurements.json file
s.copy2("../" + file_name, "../measurements.json")

print(results)

# Create the results.json file
with open("../results.json", "w+") as f:
    f.write(json.dumps(results))