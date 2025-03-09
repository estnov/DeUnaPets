#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <iomanip>
#include <cmath>
#include <numeric>
#include <memory>
#include <stdexcept>
#include <random>

using namespace std;

// Configuración ajustable mediante parámetros
const struct Config {
    string csv_path = "Data/transacciones_ficticias_persona_0.csv";
    char csv_delimiter = ',';
    double percentile = 25.0;
    int prediction_days = 30;
    string date_format = "YYYY-MM-DD";
    double variability_factor = 0.2; // Factor de variabilidad para gastos imprevistos
} config;

// Estructura optimizada para manejo de fechas
struct Date {
    int year;
    int month;
    int day;
    
    // Constructor desde string
    Date(const string& date_str) {
        if (date_str.length() != 10 || date_str[4] != '-' || date_str[7] != '-') {
            throw invalid_argument("Formato de fecha inválido");
        }
        year = stoi(date_str.substr(0, 4));
        month = stoi(date_str.substr(5, 2));
        day = stoi(date_str.substr(8, 2));
    }
    
    // Nuevo constructor para crear fechas directamente
    Date(int y, int m, int d) : year(y), month(m), day(d) {}
    
    Date previous_month() const {
        if (month == 1) return Date(year - 1, 12, day);
        return Date(year, month - 1, day);
    }
};

// Clase para manejar transacciones
class Transaction {
public:
    string id;
    string description;
    double amount;
    string recipient;
    Date date;
    
    Transaction(const string& id, const string& desc, double amount, 
                const string& recipient, const string& date_str)
        : id(id), description(desc), amount(amount), 
          recipient(recipient), date(date_str) {}
};

// Clase principal para análisis de datos
class FinancialAnalyzer {
    vector<Transaction> transactions;
    vector<double> amounts;
    double threshold;
    
public:
    explicit FinancialAnalyzer(const string& filename) {
        load_data(filename);
        calculate_threshold();
    }
    
    void analyze() {
        auto ant_expenses = classify_ant_expenses();
        auto daily_totals = calculate_daily_totals(ant_expenses);
        
        if (daily_totals.size() < 2) {
            merge_previous_month_data(daily_totals);
        }
        
        // En lugar de usar regresión lineal, se emplea un modelo de series de tiempo:
        vector<double> predicted = perform_time_series_forecast(daily_totals);
        predict_next_month(predicted);
    }

private:
    void load_data(const string& filename) {
        ifstream file(filename);
        if (!file) throw runtime_error("No se pudo abrir el archivo: " + filename);
        
        string line;
        getline(file, line); // Saltar encabezado
        
        while (getline(file, line)) {
            process_line(line);
        }
    }

    void process_line(const string& line) {
        stringstream ss(line);
        vector<string> fields;
        string field;
        
        while (getline(ss, field, config.csv_delimiter)) {
            fields.push_back(move(field));
        }
        
        if (fields.size() != 5) {
            cerr << "Línea inválida: " << line << endl;
            return;
        }
        
        try {
            transactions.emplace_back(
                fields[0], fields[1], stod(fields[2]), 
                fields[3], fields[4]
            );
            amounts.push_back(stod(fields[2]));
        } catch (const exception& e) {
            cerr << "Error procesando línea: " << line << " - " << e.what() << endl;
        }
    }

    void calculate_threshold() {
        if (amounts.empty()) throw runtime_error("Datos insuficientes");
        
        nth_element(amounts.begin(), 
                   amounts.begin() + amounts.size()/4, 
                   amounts.end());
        threshold = amounts[amounts.size()/4];
    }

    vector<Transaction> classify_ant_expenses() {
        vector<Transaction> ant_expenses;
        ant_expenses.reserve(transactions.size()/4);
        
        for (const auto& t : transactions) {
            if (t.amount <= threshold) {
                ant_expenses.push_back(t);
            }
        }
        return ant_expenses;
    }

    map<int, double> calculate_daily_totals(const vector<Transaction>& expenses) {
        map<int, double> daily_totals;
        for (const auto& e : expenses) {
            daily_totals[e.date.day] += e.amount;
        }
        return daily_totals;
    }

    void merge_previous_month_data(map<int, double>& daily_totals) {
        const Date last_date = transactions.back().date;
        Date prev_month = last_date.previous_month();
        
        for (const auto& t : transactions) {
            if (t.date.year == prev_month.year && 
                t.date.month == prev_month.month &&
                t.amount <= threshold) {
                daily_totals[t.date.day] += t.amount;
            }
        }
    }

    // Función que implementa un suavizamiento exponencial simple para pronosticar la serie
    vector<double> perform_time_series_forecast(const map<int, double>& daily_totals) {
        // Extraer la serie en orden (suponiendo que los días representan el orden cronológico)
        vector<double> series;
        for (const auto& [day, total] : daily_totals) {
            series.push_back(total);
        }
        // Suavizamiento exponencial simple
        double alpha = 0.3; // Factor de suavizamiento (entre 0 y 1)
        double forecast = series[0]; // Inicializamos con el primer valor
        for (double actual : series) {
            forecast = alpha * actual + (1 - alpha) * forecast;
        }
        
        // Opcional: incluir variabilidad adicional similar a la original
        random_device rd;
        mt19937 gen(rd());
        // Se utiliza una distribución log-normal centrada en 1 para simular variabilidad
        lognormal_distribution<> dis(0.0, config.variability_factor);
        
        // Generar la predicción para los días futuros (se asume una predicción constante)
        vector<double> predicted;
        predicted.reserve(config.prediction_days);
        for (int i = 0; i < config.prediction_days; ++i) {
            double adjusted_forecast = forecast * dis(gen);
            adjusted_forecast = max(adjusted_forecast, 0.0);
            predicted.push_back(adjusted_forecast);
        }
        return predicted;
    }

    // Función para mostrar la predicción y generar datos para graficar
    void predict_next_month(const vector<double>& predicted) {
        double total = accumulate(predicted.begin(), predicted.end(), 0.0);
        cout << fixed << setprecision(2);
        cout << "Predicción de gastos hormiga para el próximo mes: $" << total << endl;
        generate_plot_data(predicted);
    }

    void generate_plot_data(const vector<double>& data) {
        ofstream file("prediccion.dat");
        for (size_t i = 0; i < data.size(); ++i) {
            file << (i+1) << " " << data[i] << "\n";
        }
    }
};

int main() {
    try {
        FinancialAnalyzer analyzer(config.csv_path);
        analyzer.analyze();
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
