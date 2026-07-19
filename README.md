# Java DSA Stock Prediction System

This is a third-year college DSA project written in Java.

The program performs an educational stock trend prediction using historical closing prices. It does not use real-time market data and should not be treated as financial advice.

## DSA Concepts Used

- `HashMap` for storing stock symbols and prices
- `ArrayList` for dynamic historical-price storage
- `Queue` / sliding window for moving averages
- `PriorityQueue` / min-heap for top K prices
- Binary search for price searching
- Bubble sort
- Merge sort
- Arrays and recursion
- Time-complexity comparison

## Prediction Logic

The project combines:

1. Short moving average
2. Long moving average
3. Recent price momentum
4. Weighted signal calculation

Possible outputs:

- BULLISH
- BEARISH
- SIDEWAYS

## Project Files

```text
src/
├── StockPredictionApp.java
├── StockAnalyzer.java
└── Models.java
```

## Requirements

- Java 17 or newer

## Compile

Open terminal inside the `src` folder:

```bash
javac StockPredictionApp.java StockAnalyzer.java Models.java
```

## Run

```bash
java StockPredictionApp
```

## Sample Stocks

- TCS
- INFY
- RELIANCE

You can add more prices from the menu.

## Complexity

### Moving Average

- Time: `O(n)`
- Space: `O(w)`, where `w` is the window size

### Binary Search

- Sorting: `O(n log n)`
- Searching: `O(log n)`

### Top K Prices

- Time: `O(n log k)`
- Space: `O(k)`

### Bubble Sort

- Time: `O(n²)`

### Merge Sort

- Time: `O(n log n)`
- Space: `O(n)`

## Viva Questions

1. Why is a HashMap used?
2. How does a sliding-window queue work?
3. Why is a min-heap used for top K values?
4. What is the complexity of binary search?
5. Why must the data be sorted before binary search?
6. Compare Bubble Sort and Merge Sort.
7. What is the difference between prediction and guaranteed forecasting?
8. How is momentum calculated?
