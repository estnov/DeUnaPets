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
    string date_format = "YYYY-MM-DD"; // Puedes ajustar el formato de fecha si es necesario
    double variability_factor = 0.2; // Factor de variabilidad para gastos imprevistos
} config;


struct Date {
    int year;
    int month;
    int day;

    // Constructor desde string con validación de formato de fecha
    Date(const string& date_str) {
        // Verificar que el formato sea correcto: YYYY-MM-DD
        if (date_str.length() != 10 || date_str[4] != '-' || date_str[7] != '-') {
            throw invalid_argument("Formato de fecha inválido: " + date_str);
        }

        try {
            // Extraer el año, mes y día de la cadena
            year = stoi(date_str.substr(0, 4));  // Año
            month = stoi(date_str.substr(5, 2)); // Mes
            day = stoi(date_str.substr(8, 2));   // Día
        } catch (const exception& e) {
            throw invalid_argument("Error al parsear la fecha: " + date_str);
        }

        // Validar rangos de mes y día
        if (month < 1 || month > 12) {
            throw invalid_argument("Mes fuera de rango: " + date_str);
        }

        // Validar el día según el mes
        if (day < 1 || day > days_in_month(year, month)) {
            throw invalid_argument("Día fuera de rango para el mes: " + date_str);
        }
    }

    // Constructor para crear fechas directamente
    Date(int y, int m, int d) : year(y), month(m), day(d) {
        if (month < 1 || month > 12) {
            throw invalid_argument("Mes fuera de rango");
        }
        if (day < 1 || day > days_in_month(year, month)) {
            throw invalid_argument("Día fuera de rango para el mes");
        }
    }

    // Función para obtener el número de días en un mes
    static int days_in_month(int year, int month) {
        if (month == 2) {
            // Febrero: verificar si es año bisiesto
            if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
                return 29;
            } else {
                return 28;
            }
        } else if (month == 4 || month == 6 || month == 9 || month == 11) {
            return 30;
        } else {
            return 31;
        }
    }

    // Función para obtener el mes anterior
    Date previous_month() const {
        if (month == 1) return Date(year - 1, 12, day);
        return Date(year, month - 1, day);
    }

    // Función para mostrar la fecha (opcional, pero útil para debugging)
    string getFecha() const {
        return to_string(year) + "-" + (month < 10 ? "0" : "") + to_string(month) + "-" + (day < 10 ? "0" : "") + to_string(day);
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
        
        // Identificar gastos recurrentes
        identify_recurrent_ant_expenses(ant_expenses);

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
            try {
                process_line(line);
            } catch (const invalid_argument& e) {
                cerr << "Error procesando línea: " << line << " - " << e.what() << endl;
            }
        }
    }

    string clean_line(const string& line) {
        stringstream ss(line);
        string field;
        vector<string> fields;
        while (getline(ss, field, config.csv_delimiter)) {
            fields.push_back(field);
        }
        if (fields.size() != 5) {
            throw invalid_argument("Línea inválida: " + line);
        }
        // Limpiar la fecha si es necesario
        size_t pos = fields[4].find_last_of("0123456789-");
        if (pos != string::npos && pos >= 9) {
            fields[4] = fields[4].substr(pos - 9, 10);
        }
        // Reconstruir la línea
        stringstream cleaned_line;
        for (size_t i = 0; i < fields.size(); ++i) {
            if (i > 0) cleaned_line << config.csv_delimiter;
            cleaned_line << fields[i];
        }
        return cleaned_line.str();
    }

    void process_line(const string& line) {
        string cleaned_line = clean_line(line);
        stringstream ss(cleaned_line);
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
                   amounts.begin() + amounts.size() / 4, 
                   amounts.end());
        threshold = amounts[amounts.size() / 4];
    }

    vector<Transaction> classify_ant_expenses() {
        vector<Transaction> ant_expenses;
        ant_expenses.reserve(transactions.size() / 4);
        
        for (const auto& t : transactions) {
            if (t.amount <= threshold) {
                ant_expenses.push_back(t);
            }
        }
        
        // Imprimir los gastos hormiga
        print_ant_expenses(ant_expenses);
        
        return ant_expenses;
    }

    // Función para imprimir los gastos hormiga detectados
    void print_ant_expenses(const vector<Transaction>& ant_expenses) {
        cout << "Gastos hormiga detectados:" << endl;
        for (const auto& t : ant_expenses) {
            cout << "ID: " << t.id << ", Descripción: " << t.description
                 << ", Monto: $" << fixed << setprecision(2) << t.amount
                 << ", Recipiente: " << t.recipient
                 << ", Fecha: " << t.date.year << "-" << setw(2) << setfill('0') << t.date.month << "-"
                 << setw(2) << setfill('0') << t.date.day << endl;
        }
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

    // Función para identificar gastos recurrentes
    void identify_recurrent_ant_expenses(const vector<Transaction>& ant_expenses) {
        map<string, int> description_count;

        // Contar cuántas veces aparece cada descripción
        for (const auto& t : ant_expenses) {
            description_count[t.description]++;
        }

        // Mostrar los gastos recurrentes
        cout << "\nGastos hormiga recurrentes (misma descripción):" << endl;
        for (const auto& [desc, count] : description_count) {
            if (count > 3) { // Umbral de recurrencia (ajustable)
                cout << "Descripción: " << desc << ", Veces: " << count << endl;
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
        cout << "\nPredicción de gastos hormiga para el próximo mes: $" << total << endl;
        generate_plot_data(predicted);
    }

    void generate_plot_data(const vector<double>& data) {
        ofstream file("prediccion.dat");
        for (size_t i = 0; i < data.size(); ++i) {
            file << (i + 1) << " " << data[i] << "\n";
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
