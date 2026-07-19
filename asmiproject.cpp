#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <deque>
#include <algorithm>
#include <cmath>
#include <limits>

using namespace std;

//=====================================================
//              STOCK RECORD STRUCTURE
//=====================================================

struct StockRecord
{
    string date;
    double open;
    double high;
    double low;
    double close;
    long long volume;
};

//=====================================================
//          NODE FOR PRIORITY QUEUE
//=====================================================

struct PriceNode
{
    double price;
    string date;

    bool operator<(const PriceNode& other) const
    {
        return price < other.price;
    }
};

//=====================================================
//          STOCK PREDICTION CLASS
//=====================================================

class StockPredictionSystem
{
private:
    vector<StockRecord> stocks;
    unordered_map<string, int> dateIndex;

    // Helper: Trim whitespace from both ends of a string
    static string trim(const string& s)
    {
        size_t start = s.find_first_not_of(" \t\r\n");
        if (start == string::npos) return "";
        size_t end = s.find_last_not_of(" \t\r\n");
        return s.substr(start, end - start + 1);
    }

    // Helper: Safe conversion to double
    static bool toDouble(const string& s, double& value)
    {
        try {
            size_t idx;
            string t = trim(s);
            value = stod(t, &idx);
            // Ensure whole string was parsed
            return idx == t.size();
        } catch (...) {
            return false;
        }
    }

    // Helper: Safe conversion to long long
    static bool toLongLong(const string& s, long long& value)
    {
        try {
            size_t idx;
            string t = trim(s);
            value = stoll(t, &idx);
            return idx == t.size();
        } catch (...) {
            return false;
        }
    }

public:
    StockPredictionSystem() = default;

    // Load CSV file into stocks vector
    bool loadCSV(const string& filename)
    {
        ifstream file(filename);
        if (!file.is_open())
        {
            cout << "\nUnable to open file: " << filename << endl;
            return false;
        }

        stocks.clear();
        dateIndex.clear();

        string line;
        // Read and skip header
        if (!getline(file, line)) {
            cout << "\nFile is empty or invalid.\n";
            return false;
        }

        int lineNumber = 1;
        while (getline(file, line))
        {
            ++lineNumber;
            if (trim(line).empty())
                continue;

            stringstream ss(line);
            vector<string> tokens;
            string temp;
            while (getline(ss, temp, ','))
                tokens.push_back(trim(temp));
            if (tokens.size() != 6)
            {
                cout << "Skipping invalid row " << lineNumber << ": wrong number of columns.\n";
                continue;
            }
            StockRecord rec;
            rec.date = tokens[0];
            if (!toDouble(tokens[1], rec.open) ||
                !toDouble(tokens[2], rec.high) ||
                !toDouble(tokens[3], rec.low) ||
                !toDouble(tokens[4], rec.close) ||
                !toLongLong(tokens[5], rec.volume))
            {
                cout << "Skipping invalid row " << lineNumber << ": parse error.\n";
                continue;
            }
            // No duplicate dates allowed
            if (dateIndex.count(rec.date))
            {
                cout << "Duplicate date at row " << lineNumber << ": " << rec.date << ". Skipped.\n";
                continue;
            }
            dateIndex[rec.date] = static_cast<int>(stocks.size());
            stocks.push_back(rec);
        }
        file.close();
        cout << "\nLoaded " << stocks.size() << " stock records successfully.\n";
        return !stocks.empty();
    }

    // Display all stock records in a table
    void displayAllStocks() const
    {
        if (stocks.empty())
        {
            cout << "\nNo Data Found.\n";
            return;
        }
        cout << "\n";
        cout << left
             << setw(15) << "Date"
             << setw(12) << "Open"
             << setw(12) << "High"
             << setw(12) << "Low"
             << setw(12) << "Close"
             << setw(15) << "Volume" << endl;
        cout << string(78, '-') << endl;
        cout << fixed << setprecision(2);
        for (const auto& s : stocks)
        {
            cout << left
                 << setw(15) << s.date
                 << setw(12) << s.open
                 << setw(12) << s.high
                 << setw(12) << s.low
                 << setw(12) << s.close
                 << setw(15) << s.volume
                 << endl;
        }
    }

    // Search and display a record by date
    void searchByDate(const string& date) const
    {
        string key = trim(date);
        auto it = dateIndex.find(key);
        if (it == dateIndex.end())
        {
            cout << "\nRecord not found for date: " << key << endl;
            return;
        }
        const StockRecord& s = stocks[it->second];
        cout << fixed << setprecision(2);
        cout << "\nDate   : " << s.date << '\n'
             << "Open   : " << s.open << '\n'
             << "High   : " << s.high << '\n'
             << "Low    : " << s.low << '\n'
             << "Close  : " << s.close << '\n'
             << "Volume : " << s.volume << endl;
    }

    // Return number of loaded records
    int size() const
    {
        return static_cast<int>(stocks.size());
    }

    // Calculate moving average of closing prices with given period
    vector<double> calculateMovingAverage(int period) const
    {
        vector<double> result;
        if (period <= 0 || period > static_cast<int>(stocks.size()))
            return result;
        double sum = 0.0;
        deque<double> window;
        for (const auto& stock : stocks)
        {
            window.push_back(stock.close);
            sum += stock.close;
            if (static_cast<int>(window.size()) > period)
            {
                sum -= window.front();
                window.pop_front();
            }
            if (static_cast<int>(window.size()) == period)
                result.push_back(sum / period);
        }
        return result;
    }

    // Display moving average for the given period
    void displayMovingAverage(int period) const
    {
        if (period <= 0 || period > static_cast<int>(stocks.size()))
        {
            cout << "\nInvalid period for moving average.\n";
            return;
        }
        vector<double> ma = calculateMovingAverage(period);
        cout << fixed << setprecision(2);
        cout << "\n" << period << "-Day Moving Average\n";
        cout << "-------------------------------\n";
        int idx = period - 1;
        for (size_t i = 0; i < ma.size(); ++i)
        {
            cout << left << setw(15) << stocks[idx + i].date
                 << " : " << ma[i] << endl;
        }
    }

    // Display statistics: highest, lowest, averages, return
    void displayStatistics() const
    {
        if (stocks.empty())
        {
            cout << "\nNo Data.\n";
            return;
        }
        double highest = stocks[0].high;
        double lowest = stocks[0].low;
        string highDate = stocks[0].date;
        string lowDate = stocks[0].date;
        double totalClose = 0.0;
        long long totalVolume = 0;
        for (const auto& s : stocks)
        {
            totalClose += s.close;
            totalVolume += s.volume;
            if (s.high > highest)
            {
                highest = s.high;
                highDate = s.date;
            }
            if (s.low < lowest)
            {
                lowest = s.low;
                lowDate = s.date;
            }
        }
        double avgClose = totalClose / stocks.size();
        double avgVolume = static_cast<double>(totalVolume) / stocks.size();
        double overallReturn = (stocks.back().close - stocks.front().close) / stocks.front().close * 100.0;
        cout << fixed << setprecision(2);
        cout << "\n========== STOCK STATISTICS ==========\n";
        cout << "Total Records   : " << stocks.size() << endl;
        cout << "Highest Price   : Rs. " << highest << " on " << highDate << endl;
        cout << "Lowest Price    : Rs. " << lowest << " on " << lowDate << endl;
        cout << "Average Close   : Rs. " << avgClose << endl;
        cout << "Average Volume  : " << avgVolume << endl;
        cout << "Overall Return  : " << overallReturn << " %" << endl;
    }

    //-------------------------------------------------
    // Highest Closing Prices
    //-------------------------------------------------

    void highestClosingPrices(int count)
    {
        priority_queue<PriceNode> pq;

        for(auto &stock:stocks)
        {
            pq.push(
            {
                stock.close,
                stock.date
            });
        }

        count=min(count,(int)stocks.size());

        cout<<"\nHighest Closing Prices\n";
        cout<<"-----------------------------\n";

        for(int i=1;i<=count;i++)
        {
            PriceNode current=pq.top();
            pq.pop();

            cout<<i
                <<". "
                <<current.date
                <<"  Rs. "
                <<fixed
                <<setprecision(2)
                <<current.price
                <<endl;
        }
    }

    //-------------------------------------------------
    // Lowest Closing Prices
    //-------------------------------------------------

    void lowestClosingPrices(int count)
    {
        priority_queue<
            pair<double,string>,
            vector<pair<double,string>>,
            greater<pair<double,string>>
        > pq;

        for(auto &stock:stocks)
        {
            pq.push(
            {
                stock.close,
                stock.date
            });
        }

        count=min(count,(int)stocks.size());

        cout<<"\nLowest Closing Prices\n";
        cout<<"-----------------------------\n";

        for(int i=1;i<=count;i++)
        {
            auto current=pq.top();
            pq.pop();

            cout<<i
                <<". "
                <<current.second
                <<"  Rs. "
                <<fixed
                <<setprecision(2)
                <<current.first
                <<endl;
        }
    }

    //-------------------------------------------------
    // Daily Price Change Analysis
    //-------------------------------------------------

    void dailyChangeAnalysis()
    {
        if(stocks.size()<2)
        {
            cout<<"\nNot Enough Data\n";
            return;
        }

        cout<<fixed<<setprecision(2);

        cout<<"\nDaily Price Changes\n";
        cout<<"----------------------------------------\n";

        for(int i=1;i<stocks.size();i++)
        {
            double change=
                stocks[i].close-
                stocks[i-1].close;

            double percent=
                (change/
                stocks[i-1].close)
                *100;

            cout<<stocks[i].date
                <<" : ";

            if(change>=0)
                cout<<"UP ";
            else
                cout<<"DOWN ";

            cout<<change
                <<" ("
                <<percent
                <<"%)"
                <<endl;
        }
    }
    //-------------------------------------------------
    // Predict Next Price using Linear Regression
    //-------------------------------------------------

    double predictNextPriceLinearRegression(int recentDays = 10)
    {
        if(stocks.size() < 2)
            return -1;

        int n = min(recentDays, (int)stocks.size());
        int start = stocks.size() - n;

        double sumX = 0;
        double sumY = 0;
        double sumXY = 0;
        double sumXX = 0;

        for(int i = 0; i < n; i++)
        {
            double x = i + 1;
            double y = stocks[start + i].close;

            sumX += x;
            sumY += y;
            sumXY += x * y;
            sumXX += x * x;
        }

        double denominator = n * sumXX - sumX * sumX;

        if(fabs(denominator) < 1e-9)
            return stocks.back().close;

        double slope =
            (n * sumXY - sumX * sumY) /
            denominator;

        double intercept =
            (sumY - slope * sumX) / n;

        return intercept + slope * (n + 1);
    }

    //-------------------------------------------------
    // Predict Next Day
    //-------------------------------------------------

    void predictNextDay()
    {
        if(stocks.size() < 2)
        {
            cout << "\nNot enough data.\n";
            return;
        }

        int days;

        cout << "\nRecent Days for Prediction : ";
        cin >> days;

        if(days < 2)
        {
            cout << "Minimum 2 days required.\n";
            return;
        }

        double prediction =
            predictNextPriceLinearRegression(days);

        double current =
            stocks.back().close;

        double change =
            prediction - current;

        double percent =
            (change/current) * 100;

        cout << fixed << setprecision(2);

        cout << "\n========== PREDICTION ==========\n";

        cout << "Last Date : "
             << stocks.back().date << endl;

        cout << "Current Close : Rs. "
             << current << endl;

        cout << "Predicted Close : Rs. "
             << prediction << endl;

        cout << "Expected Change : "
             << percent << "%\n";

        cout << "\nTrading Signal : ";

        if(percent > 1.0)
            cout << "BUY";

        else if(percent < -1.0)
            cout << "SELL";

        else
            cout << "HOLD";

        cout << endl;
    }

    //-------------------------------------------------
    // 5-Day vs 10-Day Trend
    //-------------------------------------------------

    void analyzeTrend()
    {
        if(stocks.size() < 10)
        {
            cout << "\nNeed at least 10 records.\n";
            return;
        }

        vector<double> ma5 =
            calculateMovingAverage(5);

        vector<double> ma10 =
            calculateMovingAverage(10);

        double shortMA = ma5.back();
        double longMA = ma10.back();
        double latest = stocks.back().close;

        cout << fixed << setprecision(2);

        cout << "\n========== TREND ==========\n";

        cout << "Current Price : "
             << latest << endl;

        cout << "5 Day MA : "
             << shortMA << endl;

        cout << "10 Day MA : "
             << longMA << endl;

        if(shortMA > longMA &&
           latest > shortMA)
        {
            cout << "Strong Bullish Trend\n";
        }

        else if(shortMA > longMA)
        {
            cout << "Bullish Trend\n";
        }

        else if(shortMA < longMA &&
                latest < shortMA)
        {
            cout << "Strong Bearish Trend\n";
        }

        else if(shortMA < longMA)
        {
            cout << "Bearish Trend\n";
        }

        else
        {
            cout << "Sideways Trend\n";
        }
    }

    //-------------------------------------------------
    // Calculate Volatility
    //-------------------------------------------------

    void calculateVolatility()
    {
        if(stocks.size() < 2)
        {
            cout << "\nNot enough data.\n";
            return;
        }

        double mean = 0;

        for(auto &s : stocks)
            mean += s.close;

        mean /= stocks.size();

        double variance = 0;

        for(auto &s : stocks)
        {
            variance +=
                pow(s.close - mean,2);
        }

        variance /= stocks.size();

        double sd = sqrt(variance);

        cout << fixed << setprecision(2);

        cout << "\nMarket Volatility\n";
        cout << "-------------------------\n";

        cout << "Mean Closing Price : "
             << mean << endl;

        cout << "Standard Deviation : "
             << sd << endl;

        if(sd < 2)
            cout << "Very Stable Market\n";

        else if(sd < 5)
            cout << "Moderately Volatile\n";

        else
            cout << "Highly Volatile Market\n";
    }

    //-------------------------------------------------
    // Best & Worst Trading Day
    //-------------------------------------------------

    void bestWorstDay()
    {
        if(stocks.size() < 2)
            return;

        double best = -1e9;
        double worst = 1e9;

        string bestDate;
        string worstDate;

        for(int i = 1; i < stocks.size(); i++)
        {
            double change =
                stocks[i].close -
                stocks[i-1].close;

            if(change > best)
            {
                best = change;
                bestDate = stocks[i].date;
            }

            if(change < worst)
            {
                worst = change;
                worstDate = stocks[i].date;
            }
        }

        cout << fixed << setprecision(2);

        cout << "\nBest Trading Day\n";
        cout << bestDate
             << "  +" << best << endl;

        cout << "\nWorst Trading Day\n";
        cout << worstDate
             << "  " << worst << endl;
    }

    //-------------------------------------------------
    // Consecutive Gain/Loss Analysis
    //-------------------------------------------------

    void consecutiveTrend()
    {
        int gain = 0;
        int loss = 0;

        int maxGain = 0;
        int maxLoss = 0;

        for(int i = 1; i < stocks.size(); i++)
        {
            if(stocks[i].close >
               stocks[i-1].close)
            {
                gain++;
                loss = 0;
            }

            else
            {
                loss++;
                gain = 0;
            }

            maxGain = max(maxGain,gain);
            maxLoss = max(maxLoss,loss);
        }

        cout << "\nLongest Gain Streak : "
             << maxGain
             << " Days\n";

        cout << "Longest Loss Streak : "
             << maxLoss
             << " Days\n";
    }
    //-------------------------------------------------
    // Sort by Closing Price (Descending)
    //-------------------------------------------------

    void sortByClosingPrice()
    {
        if(stocks.empty())
        {
            cout<<"\nNo Data Available\n";
            return;
        }

        vector<StockRecord> temp=stocks;

        sort(temp.begin(),temp.end(),

        [](StockRecord a,StockRecord b)
        {
            return a.close>b.close;
        });

        cout<<"\n========== SORTED BY CLOSE ==========\n";

        for(auto &s:temp)
        {
            cout<<left
                <<setw(15)<<s.date
                <<"Rs. "
                <<fixed
                <<setprecision(2)
                <<s.close
                <<endl;
        }
    }

    //-------------------------------------------------
    // Sort by Volume
    //-------------------------------------------------

    void sortByVolume()
    {
        vector<StockRecord> temp=stocks;

        sort(temp.begin(),temp.end(),

        [](StockRecord a,StockRecord b)
        {
            return a.volume>b.volume;
        });

        cout<<"\n========== SORTED BY VOLUME ==========\n";

        for(auto &s:temp)
        {
            cout<<left
                <<setw(15)<<s.date
                <<s.volume
                <<endl;
        }
    }

    //-------------------------------------------------
    // Highest Volume Day
    //-------------------------------------------------

    void highestVolumeDay()
    {
        if(stocks.empty())
            return;

        StockRecord best=stocks[0];

        for(auto &s:stocks)
        {
            if(s.volume>best.volume)
                best=s;
        }

        cout<<"\nHighest Volume Day\n";
        cout<<"-----------------------------\n";

        cout<<"Date : "<<best.date<<endl;
        cout<<"Volume : "<<best.volume<<endl;
        cout<<"Close : "<<best.close<<endl;
    }

    //-------------------------------------------------
    // Lowest Volume Day
    //-------------------------------------------------

    void lowestVolumeDay()
    {
        if(stocks.empty())
            return;

        StockRecord low=stocks[0];

        for(auto &s:stocks)
        {
            if(s.volume<low.volume)
                low=s;
        }

        cout<<"\nLowest Volume Day\n";
        cout<<"-----------------------------\n";

        cout<<"Date : "<<low.date<<endl;
        cout<<"Volume : "<<low.volume<<endl;
        cout<<"Close : "<<low.close<<endl;
    }

    //-------------------------------------------------
    // Top Daily Gainers
    //-------------------------------------------------

    void topGainers()
    {
        priority_queue<pair<double,string>> pq;

        for(int i=1;i<stocks.size();i++)
        {
            double gain=
            stocks[i].close-
            stocks[i-1].close;

            pq.push({gain,stocks[i].date});
        }

        cout<<"\nTop 5 Gainers\n";
        cout<<"----------------------------\n";

        for(int i=1;i<=5 && !pq.empty();i++)
        {
            auto cur=pq.top();

            pq.pop();

            cout<<i
                <<". "
                <<cur.second
                <<"  +"
                <<fixed
                <<setprecision(2)
                <<cur.first
                <<endl;
        }
    }

    //-------------------------------------------------
    // Top Daily Losers
    //-------------------------------------------------

    void topLosers()
    {
        priority_queue<
        pair<double,string>,
        vector<pair<double,string>>,
        greater<pair<double,string>>
        > pq;

        for(int i=1;i<stocks.size();i++)
        {
            double loss=
            stocks[i].close-
            stocks[i-1].close;

            pq.push({loss,stocks[i].date});
        }

        cout<<"\nTop 5 Losers\n";
        cout<<"----------------------------\n";

        for(int i=1;i<=5 && !pq.empty();i++)
        {
            auto cur=pq.top();

            pq.pop();

            cout<<i
                <<". "
                <<cur.second
                <<" "
                <<fixed
                <<setprecision(2)
                <<cur.first
                <<endl;
        }
    }

    //-------------------------------------------------
    // Simple Buy/Sell Score
    //-------------------------------------------------

    void recommendationScore()
    {
        double prediction=
        predictNextPriceLinearRegression(10);

        double current=
        stocks.back().close;

        double score=
        ((prediction-current)/current)*100;

        cout<<"\nRecommendation Score\n";
        cout<<"---------------------------\n";

        cout<<"Score : "
            <<fixed
            <<setprecision(2)
            <<score
            <<"%\n";

        if(score>3)
            cout<<"★★★★★ Strong Buy\n";

        else if(score>1)
            cout<<"★★★★ Buy\n";

        else if(score>-1)
            cout<<"★★★ Hold\n";

        else if(score>-3)
            cout<<"★★ Sell\n";

        else
            cout<<"★ Strong Sell\n";
    }

    //-------------------------------------------------
    // Export Prediction Report
    //-------------------------------------------------

    void exportReport()
    {
        ofstream fout("prediction_report.txt");

        if(!fout.is_open())
        {
            cout<<"\nCannot Create Report\n";
            return;
        }

        fout<<"DSA STOCK PREDICTION REPORT\n";
        fout<<"===========================\n\n";

        fout<<"Records : "
            <<stocks.size()
            <<endl;

        fout<<"Current Close : "
            <<stocks.back().close
            <<endl;

        double next=
        predictNextPriceLinearRegression(10);

        fout<<"Predicted Price : "
            <<next
            <<endl;

        fout<<"Prediction Difference : "
            <<next-stocks.back().close
            <<endl;

        fout.close();

        cout<<"\nReport Saved Successfully\n";
    }

    //-------------------------------------------------
    // Display Last 10 Records
    //-------------------------------------------------

    void lastTenRecords()
    {
        cout << "\nLast 10 Trading Days\n";
        cout << "---------------------------\n";
        int start = max(0, (int)stocks.size() - 10);
        for (int i = start; i < stocks.size(); i++)
        {
            cout << stocks[i].date
                 << "  "
                 << stocks[i].close
                 << endl;
        }
    }

    //-------------------------------------------------
    // Display User Menu
    //-------------------------------------------------
    void displayMenu() const
    {
        cout << "\n========== STOCK PREDICTION SYSTEM MENU ==========\n";
        cout << " 1.  Display All Records\n";
        cout << " 2.  Search by Date\n";
        cout << " 3.  Moving Average\n";
        cout << " 4.  Statistics\n";
        cout << " 5.  Highest Closing Prices\n";
        cout << " 6.  Lowest Closing Prices\n";
        cout << " 7.  Daily Change Analysis\n";
        cout << " 8.  Predict Next Day\n";
        cout << " 9.  Trend Analysis\n";
        cout << "10.  Calculate Volatility\n";
        cout << "11.  Best/Worst Trading Day\n";
        cout << "12.  Consecutive Trend Analysis\n";
        cout << "13.  Sort by Closing Price\n";
        cout << "14.  Sort by Volume\n";
        cout << "15.  Highest Volume Day\n";
        cout << "16.  Lowest Volume Day\n";
        cout << "17.  Top Gainers\n";
        cout << "18.  Top Losers\n";
        cout << "19.  Recommendation Score\n";
        cout << "20.  Export Report\n";
        cout << "21.  Last 10 Records\n";
        cout << "22.  Exit\n";
        cout << "==================================================\n";
        cout << "Enter your choice (1-22): ";
    }
}; // End of class StockPredictionSystem

//-------------------------------------------------
// Main Function
//-------------------------------------------------
int main()
{
    StockPredictionSystem sps;
    string filename = "stock_data.csv";
    if (!sps.loadCSV(filename))
    {
        cout << "\nExiting - Could not load stock data.\n";
        return 1;
    }

    int choice = 0;
    do
    {
        sps.displayMenu();
        cin >> choice;
        if (cin.fail())
        {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            cout << "\nInvalid input. Please enter a number between 1 and 22.\n";
            continue;
        }
        switch (choice)
        {
            case 1:
                sps.displayAllStocks();
                break;
            case 2:
            {
                cout << "\nEnter date (YYYY-MM-DD): ";
                string date;
                cin >> date;
                sps.searchByDate(date);
                break;
            }
            case 3:
            {
                cout << "\nEnter period for moving average: ";
                int period;
                cin >> period;
                if (cin.fail() || period <= 0)
                {
                    cin.clear();
                    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    cout << "\nInvalid period entered.\n";
                    break;
                }
                sps.displayMovingAverage(period);
                break;
            }
            case 4:
                sps.displayStatistics();
                break;
            case 5:
            {
                cout << "\nHow many top highest closing prices? ";
                int n;
                cin >> n;
                if (cin.fail() || n <= 0)
                {
                    cin.clear();
                    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    cout << "\nInvalid number entered.\n";
                    break;
                }
                sps.highestClosingPrices(n);
                break;
            }
            case 6:
            {
                cout << "\nHow many lowest closing prices? ";
                int n;
                cin >> n;
                if (cin.fail() || n <= 0)
                {
                    cin.clear();
                    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    cout << "\nInvalid number entered.\n";
                    break;
                }
                sps.lowestClosingPrices(n);
                break;
            }
            case 7:
                sps.dailyChangeAnalysis();
                break;
            case 8:
                sps.predictNextDay();
                break;
            case 9:
                sps.analyzeTrend();
                break;
            case 10:
                sps.calculateVolatility();
                break;
            case 11:
                sps.bestWorstDay();
                break;
            case 12:
                sps.consecutiveTrend();
                break;
            case 13:
                sps.sortByClosingPrice();
                break;
            case 14:
                sps.sortByVolume();
                break;
            case 15:
                sps.highestVolumeDay();
                break;
            case 16:
                sps.lowestVolumeDay();
                break;
            case 17:
                sps.topGainers();
                break;
            case 18:
                sps.topLosers();
                break;
            case 19:
                sps.recommendationScore();
                break;
            case 20:
                sps.exportReport();
                break;
            case 21:
                sps.lastTenRecords();
                break;
            case 22:
                cout << "\nThank you for using Stock Prediction System. Goodbye!\n";
                break;
            default:
                cout << "\nInvalid choice. Please select a valid menu option (1-22).\n";
                break;
        }
    } while (choice != 22);
    return 0;
}