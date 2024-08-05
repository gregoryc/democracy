import yfinance as yf
import pandas as pd
with open("/home/a/tsx/us_stocks") as file:
    lines = file.read().strip().split("\n")
from os import system

system ('cd /home/a/tsx/us_stock_list; rm AAPL.csv TSLA.csv NVDA.csv AMZN.csv META.csv MSFT.csv GOOG.csv')
for l in [lines]:
    for stock in l:
        stock=stock.replace(" ", "").replace("^",'')
        start_date= '1970-01-01'
        end_date = '2025-01-01'


        # Define the output file path
        file_path = f"us_stock_list/{stock}.csv"

        try:
            with open(file_path) as f:
                exists=True
        except:
            exists = False

        if exists:
            #print(f"Skip {stock}")
            continue

        # Download the stock data
        print(stock)
        data = yf.download(stock, start=start_date, end=end_date)    
        # Save the data to a CSV file
        try:
            data.to_csv(file_path)
            print(f"Data successfully saved to {file_path}")
        except Exception as e:
            print(f"An error occurred: {e}")

    input("Continue? ")
