# T-Digital Test Task
Entrance task from the AlgoQuant company for Mikhail Pravdukhin.

## Project navigation

- [OrderBook](https://github.com/beforeyougo/TDigitalTestTask/tree/main/OrderBook): Visual Studio C++ project for the task of processing order book updates
    - [OrderBook/OrderBook/data](https://github.com/beforeyougo/TDigitalTestTask/tree/main/OrderBook/OrderBook/data): data used by the project
    - [OrderBook/OrderBook/results](https://github.com/beforeyougo/TDigitalTestTask/tree/main/OrderBook/OrderBook/results): directory with the resulting file
    - [OrderBook/OrderBook/tests](https://github.com/beforeyougo/TDigitalTestTask/tree/main/OrderBook/OrderBook/tests): simple tests that were used to debug the project
- [MidPriceForecast](https://github.com/beforeyougo/TDigitalTestTask/tree/main/MidPriceForecast): Jupyter notebook for the task of forecasing the mid price change
---

## Contents

- [OrderBook C++ project](#orderbook-c-project)
- [MidPriceForecast Jupyter notebook](#midpriceforecast-jupyter-notebook)


## OrderBook C++ project

### Problem
The task is to write a C++ programm that takes two csv files as input:  
* sync shots - contains the initial state of the orderbook
* updates - contains consecutive updates of the order book


Both files have the same structure: *TimeStamp,OrderType(bid or ask),Price,Quantity*.  
The programm must process all sync shots and updates and log the best bid and ask prices for each unique timestamp to the results.csv file. All updates happened before the first sync shot shoud be ignored. For the set of same timestamp the programm must log information only for the last update in the sequence. If an update has the same timestamp as the current sync shot, the sync shot should be processed first.  

In addition to best prices, the programm should calculate information metrics using the current order book state. These metrics will be used to forecast the mid price movement in the [python part](#midpriceforecast-jupyter-notebook).

### Solution
#### How to efficiently store orders?
It is essential to take into account following points while developing the code structure for order storage:  
* Order book is sorted in a descending order by the price value
* Each price level is unique in the order book
* If order book receives an update with zero quantity, corresponding price level is deleted

The programm uses the stl map container to store orders of each type - one map for bids and one map for asks. The key is the price value converted to cents, the value is the order quantity. What are the advantages of such approach:
* Map is memory efficient: is uses O(n) memory
* Updating of the existing order, inserting new order and deleting have O(logn) complexity in the worst case

### What features are calculated from the order book?
1. Average Volume  
$AV = \frac{\sum\limits_{i=1}^n{q_i}}{n}, q_i$ is quantity of order $i$  
Calculated for the whole order book and for bids and ask separately
2. Volume Weighted Average Price  
$VWAP = \frac{\sum\limits_{i=1}^n{p_iq_i}}{\sum\limits_{i=1}^n{q_i}}, p_i$ is price of order $i$  
Calculated for the whole order book and for bids and ask separately
3. Volume Weighted Average Squared Price  
$VWASP = \frac{\sum\limits_{i=1}^n{p_i^2q_i}}{\sum\limits_{i=1}^n{q_i}}$  
Calculated for the whole order book and for bids and ask separately
4. Volume Imbalance  
$b = \sum\limits_{bids}{q_i}, a = \sum\limits_{asks}{q_i}$  
$VI = \frac{b - a}{b + a}$  
$-1 \leq VI \leq 1$  
Shows the dominance of buyers of sellers in terms of volume.
4. Dollar Imbalance  
$d_i = |p_i - p_{mid}|, p_mid = \frac{best bid + best ask}{2}$  
$b = \sum\limits_{bids}{d_i}, a = \sum\limits_{asks}{d_i}$  
$DI = \frac{b - a}{b + a}$  
$-1 \leq DI \leq 1$  
Shows the dominance of buyers of sellers in terms of money.  

### How to build the project
This is C++ Visual Studio project. Therefore, it is highly recommended that you create it using the Visual Studio environment, because no makefile is provided. The project is written in C++20 standard.

### How to run the project
The easiest way is to run it using the Visual Studio envirnoment. The program accepts two command line arguments: path to the sync shots file and path to the updates file. Both arguments are already stated in the commited configuration file (OrderBook.vcxproj.user). By default the program creates the *results* folder in the executable directory and saves the *results.csv* file there. But it is possible to specify where you want to save the resulting file passing the third argument.

Possible way to run the program using the Windows command line:  

    $ start OrderBook.exe <path to syncshots file> <path to updates file> <path to resulting folder (optional)> 
  
## MidPriceForecast Jupyter notebook

### Problem
The task is to use features to predict the future mid price change.

### Solution
The main idea of the solution is to use the Random Forest model to predict the mid price using the dataset preprocessed by the [C++ project](#orderbook-c-project). The model is trained on the separate training dataset and validated using the out-of-bag sample. Then its R2 score is evaluated on the testing dataset. The described procedure is performed for differend periods of forecasting: 300 ms, 1s, 5s, 30s, 1min. For more details, see [MidPriceForecast/MidPriceForecast.ipynb](https://github.com/beforeyougo/TDigitalTestTask/blob/main/MidPriceForecast/MidPriceForecast.ipynb).
