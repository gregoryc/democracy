import pandas_datareader as pdr
from datetime import datetime
import sys


print(pdr.get_data_yahoo(symbols=sys.argv[1], start=datetime(1700, 1, 1), end=datetime(2027, 1, 1)).to_string())
