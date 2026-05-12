#pragma once
#include "Asset.h"
#include <vector>
class Stock : public Asset
{
public:
    Stock(double initial_stock_value);

    /**
     * @brief Construct a new Stock object
     * which is used to predict future stock price movement. Constructor takes in filename of
     * the past stock price data, and the reference year and month, which is the
     * starting time of the simulation. The constructor would read in past stock price data from
     * the file, and calculate stock price multipliers based on the past stock price data.
     * However, the multipliers would only start when the date associated with the stock price data
     * is the same as the reference year and month, which is the starting time of the simulation.
     * Data points associated with earlier dates would be ignored.
     * If date given in inputs is earlier than what the data file has, the constructor would
     * throw an exception since simulation cannot be run without stock price data.
     * @param past_stock_data_file
     * the filename containing the past stock price data
     * @param referenceYear
     * starting year of the simulation
     * @param referenceMonth
     * starting month of the simulation
     * @throws std::runtime_error if reference year and month is earlier than the first data
     * and if referenceYear and month is less than 1.
     */
    Stock(std::string past_stock_data_file, int referenceYear, int referenceMonth);
    std::vector<double> past_stock_prices;            // this is for storing past stock price data, which is used to predict future stock price movement
    std::vector<double> past_stock_price_multipliers; // this is for storing past stock price multipliers, which is used to predict future stock price movement
    double price_multiplier_from_t_0;                 // multiplier to see how much the stock has scaled since t = 0, for logging purposes
    double price_multiplier;                          // multiplier to see stock scaling from previous t to current t
    double price_per_stock;
    double quantity; // number of the stock the person has
    int timeCounter; // this is to track how many time cycles have passed since start of simulation
    void updateValue(double change) override;
    void updateStockMultiplier(); // plan is to initialise a model to predict how the price would move, could be a random number for simplicity sake

    /**
     * @brief Updates the stock parameters at the start of each time cycle
     * This function is to be done at the start of the time cycle
     * to update the stock multiplier and stock value based on the stock price movement at the
     * start of the time cycle, before any purchase or sale of stock is done within the time cycle
     * This is important to ensure that the purchase and sale of stock within the time cycle is done
     * based on the updated stock price after stock price movement at the start of the time cycle
     *
     */
    void updateParameters();
    double purchaseStock(double stock_quantity);
    double purchaseStockByPrice(double inputCash);
    double sellStock(double stock_quantity);

    int firstYear, firstMonth; // this is for storing the date of the first stock price data,
                               // which is used to calculate the date of the current stock price data
                               // based on the number of time cycles that have passed
};