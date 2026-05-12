#include "Asset.h"
#include "Stock.h"
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <algorithm>

Stock::Stock(double initial_stock_value)
{
    value = 0.0;
    price_multiplier_from_t_0 = 1.0;
    price_multiplier = 1.0;
    price_per_stock = initial_stock_value;
    quantity = 0.0;
    timeCounter = 0;
}

Stock::Stock(std::string past_stock_data_file, int referenceYear, int referenceMonth)
{
    // throw an error if reference year or month given is invalid
    if (referenceYear < 1)
    {
        throw std::runtime_error("Invalid reference year! It is less than 1!");
    }
    else if (referenceMonth < 1)
    {
        throw std::runtime_error("Invalid reference month! It is less than 1!");
    }
    else if (referenceMonth > 12)
    {
        throw std::runtime_error("Invalid reference month! It is greater than 12!");
    }

    // note that for the purpose of this study, there is only one .csv file used, which is the S&P500 monthly price data from 1871 to Mar 2026,
    // so I am not adding in error handling for different file formats, but I am
    // adding in error handling for empty lines and invalid data within the file
    // This is just temporary code to read in the data, I will add in a more robust model to
    // predict stock price movement later on
    // For the csv file, the format for each row is ""MMM DD, YYYY", price".
    // The logic to read each row will focus on reading rows of that particular format.

    std::ifstream file(past_stock_data_file);
    std::string line;
    double price;
    // The idea behind counter is to enable us to fill up the vector containing stock price
    // multipliers, which is used to predict future stock price movement.
    int counter = 0;
    int obtainedYear, obtainedMonth; // this is for storing the date of the stock price data
    // as we read through every line of the csv file.

    while (std::getline(file, line))
    {
        if (line.empty())
            continue;

        std::istringstream ss(line);

        std::string dateStr, priceStr;
        double price = 0.0;

        // Step 1: Check for opening quote since date is given in string format with quotes
        if (ss.peek() != '"')
        {
            std::cerr << "Invalid format (missing quote): " << line << std::endl;
            continue;
        }
        ss.get(); // consume '"' This is needed to prevent the getline function later from stopping
        // at opening quote instead of closing quote

        // Step 2: Read date in string until closing quote
        if (!std::getline(ss, dateStr, '"'))
        {
            std::cerr << "Failed to read date: " << line << std::endl;
            continue;
        }

        // Step 3: Parse date string into std::tm structure
        std::tm date = {};
        std::istringstream dateStream(dateStr);

        if (!(dateStream >> std::get_time(&date, "%B %d, %Y")))
        {
            std::cerr << "Failed to parse date: " << dateStr << std::endl;
            continue;
        }
        obtainedYear = date.tm_year + 1900;
        obtainedMonth = date.tm_mon + 1;

        if (obtainedYear < referenceYear || (obtainedYear == referenceYear && obtainedMonth < referenceMonth))
        {
            // skip line since data is earlier than reference date.
            continue;
        }
        else if (counter == 0 && (obtainedYear > referenceYear || (obtainedYear == referenceYear && obtainedMonth > referenceMonth)))
        {
            std::cerr << "Reference date is earlier than the first data point date, cannot run simulation, throwing exception." << std::endl;
            throw std::runtime_error("Reference date is earlier than the first data point date");
        }
        else if (counter == 0)
        {
            firstYear = obtainedYear;
            firstMonth = obtainedMonth;
            // std::cout << "first month and year are " << firstMonth << " " << firstYear << std::endl;
        }

        // Step 4, check if it is a comma next
        if (ss.peek() != ',')
        {
            std::cerr << "Invalid format (missing comma): " << line << std::endl;
            continue;
        }
        ss.get(); // consume ','

        // Step 5: Read price, which may contain commas as thousand separators.
        // Hence, the price is read as string first.
        if (!(ss >> priceStr))
        {
            std::cerr << "Failed to read price: " << line << std::endl;
            continue;
        }
        // Step 6: Remove commas and quotes from price string, then convert to double
        priceStr.erase(std::remove(priceStr.begin(), priceStr.end(), ','), priceStr.end()); // remove commas from price string
        priceStr.erase(std::remove(priceStr.begin(), priceStr.end(), '"'), priceStr.end()); // remove quotes from price string if any
        try
        {
            price = std::stod(priceStr);
        }
        catch (const std::invalid_argument &e)
        {
            std::cerr << "Invalid price format: " << priceStr << " in line: " << line << std::endl;
            continue;
        }
        catch (const std::out_of_range &e)
        {
            std::cerr << "Price out of range: " << priceStr << " in line: " << line << std::endl;
            continue;
        }

        // Step 7: Store values
        past_stock_prices.push_back(price);

        // For the first data point, we set price multiplier to 1.0 since we are using the first
        // data point as the base point to calculate future price movement.
        // For subsequent data points, we calculate price multiplier based
        // on price change from previous data point.
        if (counter == 0)
        {
            past_stock_price_multipliers.push_back(1.0);
        }
        else
        {
            double prev = past_stock_prices[counter - 1];
            if (prev == 0.0)
            {
                std::cerr << "Division by zero avoided at line: " << line << std::endl;
                past_stock_price_multipliers.push_back(1.0);
            }
            else
            {
                past_stock_price_multipliers.push_back(price / prev);
            }
        }

        counter++;
    }

    // std::cout << "first month and year are " << firstMonth << " , " << firstYear << std::endl;
    // std::cout << "prices are" << std::endl;
    // for (double price : past_stock_prices)
    // {
    //     std::cout << price << std::endl;
    // }

    value = 0.0;
    price_multiplier_from_t_0 = 1.0;
    price_multiplier = past_stock_price_multipliers[0];
    price_per_stock = past_stock_prices[0];
    quantity = 0.0;
    timeCounter = 0;
}

void Stock::updateValue()
{
    price_per_stock = past_stock_prices[timeCounter];
    value = quantity * price_per_stock;
}

void Stock::updateStockMultiplier() // currently a placeholder, this function is used to update the stock multiplier
{
    // price_multiplier = past_stock_price_multipliers[timeCounter];
    // price_multiplier_from_t_0 *= price_multiplier;
    price_multiplier = past_stock_prices[timeCounter] / past_stock_prices[timeCounter - 1];
    price_multiplier_from_t_0 *= price_multiplier;
}

void Stock::updateParameters()
{
    timeCounter++;
    updateStockMultiplier();
    updateValue();
}

double Stock::purchaseStock(double stock_quantity) // remember that stock value has to be updated first before purchasing stock,
                                                   // to get the updated stock value before purchase
{
    quantity += stock_quantity;
    double cost_price = stock_quantity * price_per_stock;
    value += cost_price; // assuming no transaction fees
    return cost_price;   // count towards expenses
}

double Stock::purchaseStockByPrice(double inputCash)
{
    double stockQuantity = inputCash / price_per_stock;
    value += inputCash;
    quantity += stockQuantity;
    return inputCash; // return number of stocks that are purchased from the inputCash
}

double Stock::sellStock(double stock_quantity) // remember that stock value has to be updated first
{
    quantity -= stock_quantity;
    double cash_gained = stock_quantity * price_per_stock;
    value -= cash_gained;
    return cash_gained; // count towards cash inflow
}